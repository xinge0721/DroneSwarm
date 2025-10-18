/**
 ******************************************************************************
 * @file    mpu6050.cpp
 * @brief   MPU6050六轴传感器驱动程序（ESP32 I2C版本）
 * @details 使用ESP32的I2C外设与MPU6050通信
 *          - I2C引脚：可配置
 *          - I2C时钟：100kHz
 *          - 支持读取3轴加速度和3轴陀螺仪数据
 *          - 内部自动完成IIC和硬件初始化
 *          - 支持可选的外部中断
 * 
 * @note    硬件连接说明：
 *          MPU6050    ESP32
 *          --------   ---------
 *          VCC   -->  3.3V
 *          GND   -->  GND
 *          SCL   -->  可配置GPIO (内置上拉)
 *          SDA   -->  可配置GPIO (内置上拉)
 *          AD0   -->  GND  (设置I2C地址为0x68)
 *          INT   -->  可选GPIO (外部中断引脚)
 * 
 * @warning 【常见问题排查】
 *          1. 读取ID不是0x68 → 检查接线
 *          2. 通信失败 → 检查I2C初始化
 *          3. 数据全为0 → 检查MPU6050供电和初始化
 *          4. 数据跳变异常 → 检查电源稳定性
 * 
 * @version 3.0 (内部初始化版本)
 * @date    2024
 ******************************************************************************
 */

#include "esp_log.h"
#include "mpu6050.h"
#include "MPU6050_Reg.h"

static const char *TAG = "MPU6050";

// MPU6050的I2C设备地址 (7位地址)
// AD0引脚接GND时地址为0x68，接VCC时为0x69
#define MPU6050_ADDRESS     0x68

// I2C配置参数
#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_FREQ_HZ  100000

/**
 * @brief  MPU6050构造函数
 * @param  scl_pin: I2C时钟引脚
 * @param  sda_pin: I2C数据引脚
 * @param  int_pin: 外部中断引脚，默认GPIO_NUM_NC表示不使用中断
 * @param  isr_handler: 中断回调函数
 * @param  isr_arg: 中断回调函数参数
 * @note   自动完成I2C初始化、MPU6050硬件初始化、中断配置
 */
MPU6050::MPU6050(gpio_num_t scl_pin, gpio_num_t sda_pin, gpio_num_t int_pin, 
                    void (*isr_handler)(void*), void* isr_arg)
{
    ESP_LOGI(TAG, "Initializing MPU6050...");
    
    // ===== I2C初始化 =====
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed = I2C_MASTER_FREQ_HZ},
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    ESP_LOGI(TAG, "I2C initialized (SCL=%d, SDA=%d)", scl_pin, sda_pin);
    
    // ===== MPU6050硬件初始化 =====
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
    
    ESP_LOGI(TAG, "MPU6050 hardware initialized");
    
    // ===== 外部中断初始化（可选）=====
    if (int_pin != GPIO_NUM_NC) {
        // 配置MPU6050中断引脚：
        // bit7=0: 高电平有效
        // bit6=0: 推挽输出
        // bit5=1: Latch模式（保持中断直到读取INT_STATUS）
        // bit4=1: 读任何寄存器都清除中断
        WriteReg(MPU6050_INT_PIN_CFG, 0x30);   // 0x30 = Latch + 任意读清除
        
        // 使能数据就绪中断
        WriteReg(MPU6050_INT_ENABLE, 0x01);    // 0x01=使能DATA_RDY中断
        ESP_LOGI(TAG, "MPU6050 interrupt registers configured (DATA_RDY enabled)");
        
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << int_pin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_POSEDGE,  // 上升沿触发
        };
        gpio_config(&io_conf);
        gpio_install_isr_service(0);
        
        if (isr_handler) {
            gpio_isr_handler_add(int_pin, isr_handler, isr_arg);
            ESP_LOGI(TAG, "ESP32 GPIO interrupt enabled on GPIO%d", int_pin);
        }
    } else {
        ESP_LOGI(TAG, "MPU6050 interrupt not used (polling mode)");
    }
    
    ESP_LOGI(TAG, "MPU6050 initialization complete");
}



/**
 * @brief  向MPU6050指定寄存器写入数据
 * @param  RegAddress: 寄存器地址
 * @param  Data: 要写入的数据
 * @retval ESP_OK: 成功, 其他: 失败
 */
esp_err_t MPU6050::WriteReg(uint8_t RegAddress, uint8_t Data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, RegAddress, true);
    i2c_master_write_byte(cmd, Data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
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
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, RegAddress, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
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
