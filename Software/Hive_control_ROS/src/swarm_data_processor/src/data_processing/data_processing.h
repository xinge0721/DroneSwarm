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
#include <sstream>      // std::stringstream
#include <algorithm>    // std::min
#include <stdexcept>    // std::runtime_error
#include "./../UDP/UDP.h"
//  ============================= 公共变量声明 ==================


// ============================= 类声明 ==========================
// 无人机参数类
class DataProcessing {
public:
    // PID控制参数类
    struct PID
    {
        uint8_t kp = 0;//PID比例系数
        uint8_t ki = 0;//PID积分系数
        uint8_t kd = 0;//PID微分系数
    };

    uint8_t id = 0;//无人机编号
    // MPU6050数据
    int16_t roll = 0;//横滚角
    int16_t pitch = 0;//俯仰角
    int16_t yaw = 0;//偏航角

    // GPS数据
    int16_t x = 0;//经度
    int16_t y = 0;//纬度
    int16_t z = 0;//高度
    // ADC数据
    uint8_t batt = 0;//电池电压
    // PID数据
    struct PID pid[4] = {0};



    // 更新
    void ParseData(const std::string& data);
    void ParseData(const nlohmann::json& data);
    void ParseData(const uint8_t* data);
};

// 无人机数据

template<typename T>
class DroneData
{
private:
    DataProcessing* data = NULL;

public:
    //  =================== 构造函数 ===================
    // 参数一：无人机数量
    DroneData(const int cont = 0)
    {
        if (cont <= 0)
        {
            return;
        }
        this->data = new DataProcessing[cont];//创建无人机数据数组
    }
    //  =================== 析构函数 ===================
    ~DroneData()
    {
        if (data != NULL)//释放无人机数据数组
        {
            delete[] data;
            data = NULL;
        }
    }

    //  =================== 遍历缓存 ===================
    /**
     * @brief 解析消息队列中的数据
     * @param message_cache 待解析的消息队列
     * @throws std::runtime_error 当数据解析失败或data指针为空时
     * @note 模板设计：传什么类型用什么类型，编译时确定调用哪个重载版本
     */
    void ParseData(std::queue<T> &message_cache)
    {
        // 检查data指针是否有效
        if (data == nullptr) {
            throw std::runtime_error("DroneData未正确初始化，data指针为空");
        }

        // 逐个解析队列中的消息
        while (!message_cache.empty())
        {
            try {
                // 调用DataProcessing对象的对应重载方法
                data->ParseData(message_cache.front());
                // 解析成功，弹出已处理的消息
                message_cache.pop();
            }
            catch (const std::exception& e) {
                // 单个消息解析失败，抛出具体错误信息
                throw std::runtime_error("数据解析失败: " + std::string(e.what()) + 
                                        " (数据类型可能不匹配)");
            }
        }
    }
};


#endif