#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

//  =============================== 头文件调用 ==================
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <json.hpp>

//  ============================= 公共变量声明 ==================
#define PID_Mood 0//PID模式
#define GPS_Mood 1//坐标模式
#define ADC_Mood 2//电池电压模式

// ============================= 类声明 ==========================
// 无人机参数类
class DataProcessing {

    uint8_t id = 0;//无人机编号
    // MPU6050数据
    uint8_t roll = 0;//横滚角
    uint8_t pitch = 0;//俯仰角
    uint8_t yaw = 0;//偏航角

    // GPS数据
    uint8_t x = 0;//经度
    uint8_t y = 0;//纬度
    uint8_t z = 0;//高度
    // ADC数据
    uint8_t batt = 0;//电池电压
    // PID数据
    PID pid[4] = {0};

    struct PID
    {
        uint8_t kp = 0;//PID比例系数
        uint8_t ki = 0;//PID积分系数
        uint8_t kd = 0;//PID微分系数
    };

    // 更新
    void ParseData(const std::string& data);
    void ParseData(const nlohmann::json& data);
    void ParseData(const uint8_t* data, size_t size);
};

// 无人机数据
class DroneData
{
    DataProcessing* data = NULL;
public:
    //  =================== 构造函数 ===================
    // 参数一：无人机数量
    DroneData(const int cont = 0)
    {
        this->data = new DataProcessing[cont];
    }
    //  =================== 析构函数 ===================
    ~DroneData()
    {
        if (data != NULL)
        {
            delete[] data;
        }
    }

    //  =================== 遍历缓存 ===================
    // 参数1：消息缓存
    void ParseData(const std::queue<UDPMessage> &message_cache);
};


#endif