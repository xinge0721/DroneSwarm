// 功能包：UDP与ROS桥接


#include "ros/ros.h"
#include "std_msgs/String.h" //普通文本类型的消息
#include <sstream>
#include "./UDP/UDP.h"
#include "./data_processing/data_processing.h"

// 创建不同类型的UDP服务器和数据处理器
// UDP服务器
UDP<std::string> udp_string(8888);          // 字符串数据
UDP<Json::Value> udp_json(8889);         // JSON数据  
<<<<<<< HEAD
UDP<std::vector<uint8_t>> udp_binary(8890); // 二进制数据
// 数据处理器
// 字符串10架无人机
=======
UDP<std::vector<uint8_t>> udp_binary(9600); // 二进制数据

>>>>>>> origin/moyu
DroneData<std::string> string_processor(10);
// JSON10架无人机
DroneData<Json::Value> json_processor(10);
// 二进制10架无人机
DroneData<std::vector<uint8_t>> binary_processor(10);





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
    ros::Publisher pub = nh.advertise<std_msgs::String>("UDP",10);

    // 启动所有UDP服务器监听
    std::cout << "启动UDP模板服务器..." << std::endl;
    udp_string.startListening();
    udp_json.startListening();
    udp_binary.startListening();

    //逻辑(一秒10次)
    ros::Rate r(10);

    //节点不死
    while (ros::ok())
    {
        try {
            // 处理字符串数据
            auto string_queue = udp_string.getMessageQueue();
            if (!string_queue.empty()) {
                std::cout << "处理 " << string_queue.size() << " 条字符串消息" << std::endl;
                string_processor.ParseData(string_queue);
            }
            
            // 处理JSON数据
            auto json_queue = udp_json.getMessageQueue();
            if (!json_queue.empty()) {
                std::cout << "处理 " << json_queue.size() << " 条JSON消息" << std::endl;
                json_processor.ParseData(json_queue);
            }
            
            // 处理二进制数据
            auto binary_queue = udp_binary.getMessageQueue();
            if (!binary_queue.empty()) {
                std::cout << "处理 " << binary_queue.size() << " 条二进制消息" << std::endl;
                binary_processor.ParseData(binary_queue);
            }

            // 发布处理结果到ROS话题
            std_msgs::String ros_msg;
            ros_msg.data = "UDP数据处理正常运行";
            pub.publish(ros_msg);
        }
        catch (const std::exception& e) {
            std::cerr << "数据处理错误: " << e.what() << std::endl;
        }

        //根据前面制定的发送贫频率自动休眠 休眠时间 = 1/频率；
        r.sleep();
        //处理回调函数
        ros::spinOnce();
    }

    std::cout << "停止UDP服务器..." << std::endl;

    return 0;
}