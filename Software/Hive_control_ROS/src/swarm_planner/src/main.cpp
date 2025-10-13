<<<<<<< HEAD
// 功能包：语音指挥+轨迹生成
=======
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <iostream>

using std::cout;
using std::endl;

void swarmPlannerCallback(const std_msgs::String::ConstPtr& msg)
{
    ROS_INFO("swarmPlannerCallback: %s", msg->data.c_str());
}

// 无人机数量
int drone_num = 0;

int main(int argc, char **argv)
{
    //设置编码
    setlocale(LC_ALL, "");
    // 初始化ROS节点
    ros::init(argc, argv, "swarm_planner");
    // 创建ROS句柄
    ros::NodeHandle nh;

    ros::NodeHandle node("~");

    //等待udp_ros_bridge节点初始化完毕
    // 因为udp_ros_bridge统计了无人机数量
    ros::NodeHandle nh_udp("udp_ros_bridge");
    while (ros::ok() && drone_num == 0)
    {
        node.param("drone_num", drone_num, 0);
        cout << "当前一共有" << drone_num << "架无人机" << endl;
        ros::Duration(1).sleep();
    }

    // 创建发布者
    // 发布路径规划结果
    ros::Publisher pub = nh.advertise<std_msgs::String>("pub_swarm_planner", 10);
    // 创建订阅者
    ros::Subscriber sub = nh.subscribe("sub_swarm_planner", 10, swarmPlannerCallback);


    while (ros::ok())
    {
        ros::spinOnce();
    }
    return 0;
}
>>>>>>> origin/moyu
