#ifndef __MPU6050_H
#define __MPU6050_H

#include "esp_err.h"
#include "../IIC/IIC.h"

/**
 * @brief MPU6050六轴传感器驱动类
 * @note  封装了MPU6050的基本操作，包括读写寄存器、获取传感器数据等
 */
class MPU6050
{
private:
    IIC& _iic;  ///< I2C通信实例的引用

public:
    /**
     * @brief  构造函数
     * @param  iic_instance: IIC实例的引用
     */
    MPU6050(IIC& iic_instance);
    
    /**
     * @brief  析构函数
     */
    ~MPU6050() = default;
    
    /**
     * @brief  向MPU6050寄存器写入数据
     * @param  RegAddress: 寄存器地址
     * @param  Data: 要写入的数据
     * @retval ESP_OK: 成功, 其他: 失败
     */
    esp_err_t WriteReg(uint8_t RegAddress, uint8_t Data);
    
    /**
     * @brief  从MPU6050寄存器读取数据
     * @param  RegAddress: 寄存器地址
     * @retval 读取到的数据
     */
    uint8_t ReadReg(uint8_t RegAddress);
    
    /**
     * @brief  获取MPU6050设备ID
     * @retval 设备ID (正常应为0x68)
     */
    uint8_t GetID();
    
    /**
     * @brief  获取加速度和陀螺仪原始数据
     * @param  AccX,AccY,AccZ: 加速度X,Y,Z轴数据指针
     * @param  GyroX,GyroY,GyroZ: 陀螺仪X,Y,Z轴数据指针
     * @retval ESP_OK: 成功, 其他: 失败
     */
    esp_err_t GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
                     int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);
    
    /**
     * @brief  获取温度数据
     * @retval 温度值（摄氏度）
     */
    float GetTemperature();
    
    /**
     * @brief  将原始加速度数据转换为g值
     * @param  raw_data: 原始数据
     * @retval g值
     */
    float AccelToG(int16_t raw_data);
    
    /**
     * @brief  将原始陀螺仪数据转换为度/秒
     * @param  raw_data: 原始数据
     * @retval 度/秒值
     */
    float GyroToDPS(int16_t raw_data);
};

// 为了兼容C风格的调用，保留一些全局函数声明（可选）
#ifdef __cplusplus
extern "C" {
#endif

// 如果需要C风格的接口，可以在这里声明
// void MPU6050_Init(void);
// uint8_t MPU6050_GetID(void);
// void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
//                      int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);

#ifdef __cplusplus
}
#endif

#endif // __MPU6050_H