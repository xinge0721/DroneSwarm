/**
 * @file data_processing.cpp
 * @brief 无人机数据处理模块实现文件
 * @details 该文件实现了无人机数据的多格式解析功能，支持字符串、JSON和二进制格式
 * @author DroneSwarm Team
 * @date 2025
 */

#include "data_processing.h"

/**
 * @brief 解析字符串格式的无人机数据
 * @param data 输入的字符串数据，格式为 "key1=value1,key2=value2,..."
 * @details 支持的数据字段：
 *          - id: 无人机编号
 *          - roll, pitch, yaw: MPU6050姿态数据
 *          - x, y, z: GPS位置数据
 *          - batt: 电池电压数据
 *          - pid0_kp~pid3_kd: 四个电机的PID参数
 * @note 输入字符串应遵循 key=value 的格式，多个键值对用逗号分隔
 * @example "id=1,x=100,y=200,z=50,batt=12,pid0_kp=10"
 */
void DataProcessing::ParseData(const std::string& data)
{
    // 使用字符串流进行解析，按逗号分割每个键值对
    std::stringstream ss(data);
    std::string item;
    
    // 逐个解析每个键值对
    while (std::getline(ss, item, ','))
    {
        // 查找等号位置，分离键和值
        size_t pos = item.find('=');
        if (pos != std::string::npos)
        {
            std::string key = item.substr(0, pos);      // 提取键名
            std::string value = item.substr(pos + 1);   // 提取键值
            
            try {
                // 根据键名设置对应的数据成员
                if (key == "id") {
                    id = static_cast<uint8_t>(std::stoi(value));    // 无人机编号
                }
                // MPU6050姿态数据解析
                else if (key == "roll") {
                    roll = static_cast<int16_t>(std::stoi(value));  // 横滚角
                }
                else if (key == "pitch") {
                    pitch = static_cast<int16_t>(std::stoi(value)); // 俯仰角
                }
                else if (key == "yaw") {
                    yaw = static_cast<int16_t>(std::stoi(value));   // 偏航角
                }
                // GPS位置数据解析
                else if (key == "x") {
                    x = static_cast<float>(std::stoi(value));     // 经度
                }
                else if (key == "y") {
                    y = static_cast<float>(std::stoi(value));     // 纬度
                }
                else if (key == "z") {
                    z = static_cast<float>(std::stoi(value));     // 海拔高度
                }
                // 电池电压数据解析
                else if (key == "batt") {
                    batt = static_cast<uint8_t>(std::stoi(value));  // 电池电压
                }
                // 一号电机PID参数解析
                else if (key == "pid0_kp") {
                    pid[0].kp = static_cast<uint8_t>(std::stoi(value)); // 比例系数
                }
                else if (key == "pid0_ki") {
                    pid[0].ki = static_cast<uint8_t>(std::stoi(value)); // 积分系数
                }
                else if (key == "pid0_kd") {
                    pid[0].kd = static_cast<uint8_t>(std::stoi(value)); // 微分系数
                }
                // 二号电机PID参数解析
                else if (key == "pid1_kp") {
                    pid[1].kp = static_cast<uint8_t>(std::stoi(value));
                }
                else if (key == "pid1_ki") {
                    pid[1].ki = static_cast<uint8_t>(std::stoi(value));
                }
                else if (key == "pid1_kd") {
                    pid[1].kd = static_cast<uint8_t>(std::stoi(value));
                }
                // 三号电机PID参数解析
                else if (key == "pid2_kp") {
                    pid[2].kp = static_cast<uint8_t>(std::stoi(value));
                }
                else if (key == "pid2_ki") {
                    pid[2].ki = static_cast<uint8_t>(std::stoi(value));
                }
                else if (key == "pid2_kd") {
                    pid[2].kd = static_cast<uint8_t>(std::stoi(value));
                }
                // 四号电机PID参数解析
                else if (key == "pid3_kp") {
                    pid[3].kp = static_cast<uint8_t>(std::stoi(value));
                }
                else if (key == "pid3_ki") {
                    pid[3].ki = static_cast<uint8_t>(std::stoi(value));
                }
                else if (key == "pid3_kd") {
                    pid[3].kd = static_cast<uint8_t>(std::stoi(value));
                }
                // 未知键名则忽略
            }
            catch (const std::exception& e) {
                // 数值转换失败，抛出异常给上层处理
                throw std::runtime_error("字符串解析失败，键[" + key + "]的值[" + value + "]无效: " + e.what());
            }
        }
    }
}
/**
 * @brief 解析JSON格式的无人机数据
 * @param data 输入的JSON对象，包含无人机各项数据
 * @details 支持两种PID数据格式：
 *          1. 数组格式: "pid": [{"kp":1,"ki":2,"kd":3}, ...]
 *          2. 键值对格式: "pid0_kp":1, "pid0_ki":2, "pid0_kd":3, ...
 * @note 所有字段都是可选的，函数会检查字段存在性和数据类型
 * @example 
 *   {
 *     "id": 1,
 *     "roll": 10, "pitch": 5, "yaw": 180,
 *     "x": 100, "y": 200, "z": 50,
 *     "batt": 12,
 *     "pid": [{"kp":10,"ki":5,"kd":2}, {"kp":8,"ki":4,"kd":1}]
 *   }
 */
void DataProcessing::ParseData(const nlohmann::json& data)
{
    // 解析无人机编号，检查字段存在性和数据类型
    if (data.contains("id") && data["id"].is_number_integer()) {
        id = static_cast<uint8_t>(data["id"]);
    }
    
    // 解析MPU6050姿态数据
    if (data.contains("roll") && data["roll"].is_number_integer()) {
        roll = static_cast<int16_t>(data["roll"]);      // 横滚角
    }
    if (data.contains("pitch") && data["pitch"].is_number_integer()) {
        pitch = static_cast<int16_t>(data["pitch"]);    // 俯仰角
    }
    if (data.contains("yaw") && data["yaw"].is_number_integer()) {
        yaw = static_cast<int16_t>(data["yaw"]);        // 偏航角
    }
    
    // 解析GPS位置数据
    if (data.contains("x") && data["x"].is_number_integer()) {
        x = static_cast<float>(data["x"]);            // 经度
    }
    if (data.contains("y") && data["y"].is_number_integer()) {
        y = static_cast<float>(data["y"]);            // 纬度
    }
    if (data.contains("z") && data["z"].is_number_integer()) {
        z = static_cast<float>(data["z"]);            // 海拔高度
    }
    
    // 解析电池电压数据
    if (data.contains("batt") && data["batt"].is_number_integer()) {
        batt = static_cast<uint8_t>(data["batt"]);      // 电池电压
    }
    
    // 解析PID数据 - 优先处理数组格式
    if (data.contains("pid") && data["pid"].is_array()) {
        auto pid_array = data["pid"];
        // 遍历PID数组，最多处理4个电机的PID参数
        for (size_t i = 0; i < std::min(static_cast<size_t>(4), pid_array.size()); i++) {
            if (pid_array[i].is_object()) {
                // 解析第i个电机的PID参数
                if (pid_array[i].contains("kp") && pid_array[i]["kp"].is_number_integer()) {
                    pid[i].kp = static_cast<uint8_t>(pid_array[i]["kp"]);   // 比例系数
                }
                if (pid_array[i].contains("ki") && pid_array[i]["ki"].is_number_integer()) {
                    pid[i].ki = static_cast<uint8_t>(pid_array[i]["ki"]);   // 积分系数
                }
                if (pid_array[i].contains("kd") && pid_array[i]["kd"].is_number_integer()) {
                    pid[i].kd = static_cast<uint8_t>(pid_array[i]["kd"]);   // 微分系数
                }
            }
        }
    }
    else {
        // 如果没有数组形式，则检查键值对形式的PID数据
        for (int i = 0; i < 4; i++) {
            std::string prefix = "pid" + std::to_string(i) + "_";
            // 解析第i个电机的PID参数
            if (data.contains(prefix + "kp") && data[prefix + "kp"].is_number_integer()) {
                pid[i].kp = static_cast<uint8_t>(data[prefix + "kp"]);
            }
            if (data.contains(prefix + "ki") && data[prefix + "ki"].is_number_integer()) {
                pid[i].ki = static_cast<uint8_t>(data[prefix + "ki"]);
            }
            if (data.contains(prefix + "kd") && data[prefix + "kd"].is_number_integer()) {
                pid[i].kd = static_cast<uint8_t>(data[prefix + "kd"]);
            }
        }
    }
}

/**
 * @brief 解析二进制格式的无人机数据包
 * @param data 输入的二进制数据指针
 * @details 数据包格式：
 *   [包头0][包头1][状态位][数据长度][参数位1][参数位2]...[参数位n][校验位][包尾]
 *    0xEE   0xEE   status   length    param1    param2      paramn   checksum 0xFF
 * 
 * @note 状态位定义：
 *   - 0x00: 姿态数据 (roll, pitch, yaw) - 6字节
 *   - 0x01: GPS数据 (x, y, z) - 6字节  
 *   - 0x02: ADC数据 (电池电压) - 1字节
 *   - 0x03: 无人机编号 - 1字节
 *   - 0x04~0x07: 电机PID数据 (kp, ki, kd) - 3字节
 * 
 * @note 校验算法：
 *   校验位 = (状态位 + 数据长度 + 参数位1 + 参数位2 + ... + 参数位n) & 0xFF
 * 
 * @example GPS数据包 (16位格式):
 *   [0xEE][0xEE][0x01][0x06][0x00][100][0x00][200][0x00][50][checksum][0xFF]
 *   表示GPS坐标 x=100, y=200, z=50 (每个坐标16位，length=6)
 *
 * @example 姿态数据包 (16位格式):
 *   [0xEE][0xEE][0x00][0x06][0x00][10][0xFF][350][0x01][80][checksum][0xFF]  
 *   表示姿态 roll=10, pitch=-6, yaw=336 (每个角度16位，length=6)
 */
void DataProcessing::ParseData(const uint8_t* data)
{
    uint8_t check = 0;  // 校验和计算变量
    
    // 验证包头是否正确 (0xEE 0xEE) 或者数据为空
    if (data == nullptr || data[0] != 0xEE || data[1] != 0xEE)
    {
        return; // 包头错误，丢弃数据包
    }
    
    // 提取状态位和数据长度
    uint8_t status = data[2];       // 数据类型标识
    uint8_t length = data[3];       // 参数数据长度

    check += status +length;

    // 计算校验和：累加所有参数位
    for (int i = 0; i < length; i++)
    {
        check += data[4 + i];
    }
    
    
    // 验证校验位和包尾
    if (data[4 + length] != (check & 0xFF) || data[5 + length] != 0xFF)
    {
        return; // 校验失败或包尾错误，丢弃数据包
    }

    // 根据状态位解析不同类型的数据
    switch (status)
    {
        case 0x00: // 姿态数据解析
            // 最高位是符号位
            roll = data[4] << 8 | data[5]; // 横滚角 高八位+低八位
            pitch = data[6] << 8 | data[7]; // 俯仰角 高八位+低八位
            yaw = data[8] << 8 | data[9]; // 偏航角 高八位+低八位
            break;
            
        case 0x01: // GPS位置数据解析
            // 注意：GPS数据为浮点数，一共四个字节
            x = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];    
            y = data[8] << 24 | data[9] << 16 | data[10] << 8 | data[11];    
            z = data[12] << 24 | data[13] << 16 | data[14] << 8 | data[15];    
            break;
            
        case 0x02: // 电池电压数据解析
            batt = data[4];
            break;
            
        case 0x03: // 无人机编号解析
            id = data[4];
            break;
            
        case 0x04: // 一号电机PID参数解析
            pid[0].kp = data[4];    // 比例系数
            pid[0].ki = data[5];    // 积分系数
            pid[0].kd = data[6];    // 微分系数
            break;
            
        case 0x05: // 二号电机PID参数解析
            pid[1].kp = data[4];
            pid[1].ki = data[5];
            pid[1].kd = data[6];
            break;
            
        case 0x06: // 三号电机PID参数解析
            pid[2].kp = data[4];
            pid[2].ki = data[5];
            pid[2].kd = data[6];
            break;
            
        case 0x07: // 四号电机PID参数解析
            pid[3].kp = data[4];
            pid[3].ki = data[5];
            pid[3].kd = data[6];
            break;
            
        default: // 未知状态位，忽略数据包
            break;
    }
}