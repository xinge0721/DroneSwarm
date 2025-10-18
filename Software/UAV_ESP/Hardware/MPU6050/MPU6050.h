#ifndef __MPU6050_H
#define __MPU6050_H

#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "../System/delay/delay.h"

struct MPU6050_date
{
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
    int16_t GyroX;
    int16_t GyroY;
    int16_t GyroZ;
    float Temperature;
};

/**
 * @brief MPU6050六轴传感器驱动类
 * @note  封装了MPU6050的基本操作，内部自动完成IIC和硬件初始化
 */
class MPU6050
{
public:
    /**
     * @brief  构造函数（自动完成IIC初始化、MPU6050硬件初始化、中断配置）
     * @param  scl_pin: I2C时钟引脚
     * @param  sda_pin: I2C数据引脚
     * @param  int_pin: 外部中断引脚，默认GPIO_NUM_NC表示不使用中断
     * @param  isr_handler: 中断回调函数，默认nullptr
     * @param  isr_arg: 中断回调函数参数，默认nullptr
     */
    MPU6050(gpio_num_t scl_pin, 
            gpio_num_t sda_pin,
            gpio_num_t int_pin = GPIO_NUM_NC, 
            void (*isr_handler)(void*) = nullptr,
            void* isr_arg = nullptr);
    
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
