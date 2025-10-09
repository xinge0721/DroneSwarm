/**
 ******************************************************************************
 * @file    mpu6050.cpp
 * @brief   MPU6050六轴传感器驱动程序（ESP32 I2C版本）
 * @details 使用ESP32的I2C外设与MPU6050通信
 *          - I2C引脚：GPIO21(SDA), GPIO22(SCL)
 *          - I2C时钟：100kHz
 *          - 支持读取3轴加速度和3轴陀螺仪数据
 * 
 * @note    硬件连接说明：
 *          MPU6050    ESP32
 *          --------   ---------
 *          VCC   -->  3.3V
 *          GND   -->  GND
 *          SCL   -->  GPIO22 (内置上拉)
 *          SDA   -->  GPIO21 (内置上拉)
 *          AD0   -->  GND  (设置I2C地址为0x68)
 * 
 * @warning 【常见问题排查】
 *          1. 读取ID不是0x68 → 检查接线
 *          2. 通信失败 → 检查I2C初始化
 *          3. 数据全为0 → 检查MPU6050供电和初始化
 *          4. 数据跳变异常 → 检查电源稳定性
 * 
 * @version 2.0 (ESP32版本)
 * @date    2024
 ******************************************************************************
 */

#include "esp_log.h"
#include "../IIC/IIC.h"
#include "MPU6050_Reg.h"
#include "mpu6050.h"

static const char *TAG = "MPU6050";

// MPU6050的I2C设备地址 (7位地址)
// AD0引脚接GND时地址为0x68，接VCC时为0x69
#define MPU6050_ADDRESS     0x68

/**
 * @brief  MPU6050构造函数
 * @param  iic_instance: IIC实例的引用
 * @note   初始化MPU6050并配置各项参数
 */
MPU6050::MPU6050(IIC& iic_instance) : _iic(iic_instance)
{
    ESP_LOGI(TAG, "Initializing MPU6050...");
    
    // 配置电源管理寄存器1：选择时钟源，退出睡眠模式
    WriteReg(MPU6050_PWR_MGMT_1, 0x01);    // 0x01=选择X轴陀螺仪作为时钟源
    
    // 配置电源管理寄存器2：启用所有轴
    WriteReg(MPU6050_PWR_MGMT_2, 0x00);    // 0x00=启用所有加速度计和陀螺仪轴
    
    // 配置采样率分频器：采样率 = 陀螺仪输出率 / (1 + SMPLRT_DIV)
    WriteReg(MPU6050_SMPLRT_DIV, 0x09);    // 0x09=10分频，采样率=1kHz/10=100Hz
    
    // 配置数字低通滤波器
    WriteReg(MPU6050_CONFIG, 0x06);        // 0x06=5Hz低通滤波器，减少噪声
    
    // 配置陀螺仪量程：±2000°/s
    WriteReg(MPU6050_GYRO_CONFIG, 0x18);   // 0x18=±2000°/s满量程
    
    // 配置加速度计量程：±16g
    WriteReg(MPU6050_ACCEL_CONFIG, 0x18);  // 0x18=±16g满量程
    
    ESP_LOGI(TAG, "MPU6050 initialized successfully");
}

/**
 * @brief  向MPU6050指定寄存器写入数据
 * @param  RegAddress: 寄存器地址
 * @param  Data: 要写入的数据
 * @retval ESP_OK: 成功, 其他: 失败
 */
esp_err_t MPU6050::WriteReg(uint8_t RegAddress, uint8_t Data)
{
    esp_err_t ret = _iic.Write(MPU6050_ADDRESS, RegAddress, Data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Write register 0x%02X failed: %s", RegAddress, esp_err_to_name(ret));
    }
    return ret;
}

/**
 * @brief  从MPU6050指定寄存器读取数据
 * @param  RegAddress: 寄存器地址
 * @retval 读取到的数据
 */
uint8_t MPU6050::ReadReg(uint8_t RegAddress)
{
    uint8_t data = 0;
    esp_err_t ret = _iic.Read(MPU6050_ADDRESS, RegAddress, &data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Read register 0x%02X failed: %s", RegAddress, esp_err_to_name(ret));
        return 0;
    }
    return data;
}

/**
 * @brief  获取MPU6050的设备ID
 * @retval 设备ID，正常情况下应该返回0x68
 * @note   这是测试MPU6050通信是否正常的最简单方法
 */
uint8_t MPU6050::GetID()
{
    uint8_t id = ReadReg(MPU6050_WHO_AM_I);
    ESP_LOGI(TAG, "MPU6050 ID: 0x%02X", id);
    return id;
}

/**
 * @brief  获取MPU6050的加速度和陀螺仪原始数据
 * @param  AccX,AccY,AccZ: 加速度X,Y,Z轴数据指针
 * @param  GyroX,GyroY,GyroZ: 陀螺仪X,Y,Z轴数据指针
 * @retval ESP_OK: 成功, 其他: 失败
 * @note   返回的是16位有符号原始数据，需要根据量程转换为实际物理值
 *         加速度转换：实际值(g) = 原始值 / 2048  (±16g量程时)
 *         陀螺仪转换：实际值(°/s) = 原始值 / 16.4  (±2000°/s量程时)
 */
esp_err_t MPU6050::GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
                          int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    if (!AccX || !AccY || !AccZ || !GyroX || !GyroY || !GyroZ) {
        return ESP_ERR_INVALID_ARG;
    }
    
    uint8_t DataH, DataL;
    
    // 读取加速度X轴数据（高字节在前，低字节在后）
    DataH = ReadReg(MPU6050_ACCEL_XOUT_H);
    DataL = ReadReg(MPU6050_ACCEL_XOUT_L);
    *AccX = (int16_t)((DataH << 8) | DataL);
    
    // 读取加速度Y轴数据
    DataH = ReadReg(MPU6050_ACCEL_YOUT_H);
    DataL = ReadReg(MPU6050_ACCEL_YOUT_L);
    *AccY = (int16_t)((DataH << 8) | DataL);
    
    // 读取加速度Z轴数据
    DataH = ReadReg(MPU6050_ACCEL_ZOUT_H);
    DataL = ReadReg(MPU6050_ACCEL_ZOUT_L);
    *AccZ = (int16_t)((DataH << 8) | DataL);
    
    // 读取陀螺仪X轴数据
    DataH = ReadReg(MPU6050_GYRO_XOUT_H);
    DataL = ReadReg(MPU6050_GYRO_XOUT_L);
    *GyroX = (int16_t)((DataH << 8) | DataL);
    
    // 读取陀螺仪Y轴数据
    DataH = ReadReg(MPU6050_GYRO_YOUT_H);
    DataL = ReadReg(MPU6050_GYRO_YOUT_L);
    *GyroY = (int16_t)((DataH << 8) | DataL);
    
    // 读取陀螺仪Z轴数据
    DataH = ReadReg(MPU6050_GYRO_ZOUT_H);
    DataL = ReadReg(MPU6050_GYRO_ZOUT_L);
    *GyroZ = (int16_t)((DataH << 8) | DataL);
    
    return ESP_OK;
}

/**
 * @brief  读取MPU6050温度数据
 * @retval 温度值（摄氏度）
 * @note   温度计算公式：Temperature = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
 */
float MPU6050::GetTemperature()
{
    uint8_t DataH, DataL;
    int16_t temp_raw;
    
    DataH = ReadReg(MPU6050_TEMP_OUT_H);
    DataL = ReadReg(MPU6050_TEMP_OUT_L);
    temp_raw = (int16_t)((DataH << 8) | DataL);
    
    // 根据数据手册的公式计算温度
    float temperature = (float)temp_raw / 340.0f + 36.53f;
    
    return temperature;
}

/**
 * @brief  将原始加速度数据转换为g值
 * @param  raw_data: 原始数据
 * @retval g值
 */
float MPU6050::AccelToG(int16_t raw_data)
{
    // ±16g量程时，LSB = 2048 LSB/g
    return (float)raw_data / 2048.0f;
}

/**
 * @brief  将原始陀螺仪数据转换为度/秒
 * @param  raw_data: 原始数据
 * @retval 度/秒值
 */
float MPU6050::GyroToDPS(int16_t raw_data)
{
    // ±2000°/s量程时，LSB = 16.4 LSB/(°/s)
    return (float)raw_data / 16.4f;
}
