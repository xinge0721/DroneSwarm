#ifndef MAIN_H
#define MAIN_H
// =============================== 头文件调用 ==================
#include "ros/ros.h"
#include "std_msgs/String.h" //普通文本类型的消息
#include <sstream>
#include "./UDP/UDP.h"
#include "./data_processing/data_processing.h"
#include "swarm_planner/swarm.h"
#include "time.h"
#include "./SwarmRegistry/SwarmRegistry.h"

// =============================== 类声明 ==================
// 无人机注册表
extern SwarmRegistry swarm_registry;
// 无人机数据处理器
extern DroneData<std::vector<uint8_t>> binary_processor;
// UDP服务器
extern UDP udp_binary;
// =============================== 函数声明 ==================

#endif