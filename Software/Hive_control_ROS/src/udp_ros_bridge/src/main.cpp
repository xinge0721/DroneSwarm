// 功能包：UDP与ROS桥接


#include "ros/ros.h"
#include "std_msgs/String.h" //普通文本类型的消息
#include <sstream>
#include "./UDP/UDP.h"
#include "./data_processing/data_processing.h"
#include "swarm_planner/swarm.h"
// UDP服务器（只使用二进制数据）
UDP udp_binary(9600);

// 二进制数据处理器（10架无人机）
DroneData<std::vector<uint8_t>> binary_processor(10);

// 路径规划结果 
// 返回给无人机
// 参数一 ： 无人机id
// 参数二 ： yaw/转向角度  
// 参数三 ： x单位最小距离
void stringCallback(const std_msgs::String::ConstPtr& msg)
{
    std::cout << "收到字符串消息: " << msg->data << std::endl;
}



int main(int argc, char  *argv[])
{   
    //设置编码
    setlocale(LC_ALL,"");

    //2.初始化 ROS 节点:命名(唯一)
    // 参数1和参数2 后期为节点传值会使用
    // 参数3 是节点名称，是一个标识符，需要保证运行后，在 ROS 网络拓扑中唯一
    ros::init(argc,argv,"swarm_data_processor");
    //3.实例化 ROS 句柄
    ros::NodeHandle nh;//该类封装了 ROS 中的一些常用功能

    //4.实例化 发布者 对象
    //泛型: 发布的消息类型
    //参数1: 要发布到的话题
    //参数2: 队列中最大保存的消息数，超出此阀值时，先进的先销毁(时间早的先销毁)
    ros::Publisher pub = nh.advertise<swarm_planner::swarm>("UDP",10);

    // 启动UDP服务器监听
    std::cout << "启动UDP服务器..." << std::endl;
    udp_binary.startListening();

    //逻辑(一秒10次)
    ros::Rate Sleep_time(1);
    swarm_planner::swarm ros_msg;

    //节点不死
    while (ros::ok())
    {
        try {

            // 获取二进制数据
            auto binary_queue = udp_binary.getMessageQueue();
            if (!binary_queue.empty()) {
                std::cout << "处理 " << binary_queue.size() << " 条二进制消息" << std::endl;
                binary_processor.ParseData(binary_queue);
            }

            // 处理数据
            for(auto &data : binary_processor.data)
            {
                std::cout << "当前id: "<<data.id<<" " <<std::endl;
                // 取出数据
                ros_msg.roll = data.roll;
                ros_msg.pitch = data.pitch;
                ros_msg.yaw = data.yaw;
                ros_msg.x = data.x;
                ros_msg.y = data.y;
                ros_msg.z = data.z;
                pub.publish(ros_msg);
            }

        }
        catch (const std::exception& e) {
            std::cerr << "数据处理错误: " << e.what() << std::endl;
        }

        //根据前面制定的发送贫频率自动休眠 休眠时间 = 1/频率；
        Sleep_time.sleep();
        //处理回调函数
        ros::spinOnce();
    }

    std::cout << "停止UDP服务器..." << std::endl;

    return 0;
}