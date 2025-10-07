#ifndef IIC_H
#define IIC_H

#include "driver/i2c.h"
#include "esp_log.h"


class IIC {
private:
    i2c_config_t _conf;

public:
    IIC(i2c_mode_t mode = I2C_MODE_MASTER, /*!< I2C模式 */
        gpio_num_t sda_io_num = 22, /*!< I2C数据线 SDA */
        gpio_num_t scl_io_num = 21, /*!< I2C时钟线 SCL */
        gpio_pullup_t sda_pullup_en = GPIO_PULLUP_ENABLE, /*!< I2C数据线拉高 */
        gpio_pullup_t scl_pullup_en = GPIO_PULLUP_ENABLE, /*!< I2C时钟线拉高 */
        uint32_t clk_speed = 100000 /*!< I2C时钟频率 */
    )
    {
        _conf.mode = mode;
        _conf.sda_io_num = sda_io_num;
        _conf.scl_io_num = scl_io_num;
        _conf.sda_pullup_en = sda_pullup_en;
        _conf.scl_pullup_en = scl_pullup_en;
        _conf.master.clk_speed = clk_speed;

        // 参数配置
        // 参数一：I2C_MASTER_NUM，I2C端口号
        // 参数二：_conf，I2C配置
        esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &_conf);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "I2C config failed: %s", esp_err_to_name(err));
            return err;
        }
        // 启动I2C
        // 参数一：I2C_MASTER_NUM，I2C端口号
        // 参数二：_conf.mode，I2C模式
        // 参数三：I2C_MASTER_RX_BUF_DISABLE，接收缓冲区大小
        // 参数四：I2C_MASTER_TX_BUF_DISABLE，发送缓冲区大小
        // 参数五：0，保留
        err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "I2C install failed: %s", esp_err_to_name(err));
            return err;
        }
        
        ESP_LOGI(TAG, "I2C initialized successfully");
        return ESP_OK;
    }


    esp_err_t Write(uint8_t device_addr, uint8_t reg_addr, uint8_t data);
    esp_err_t Read(uint8_t device_addr, uint8_t reg_addr, uint8_t *data);
};

/**
 * @brief 初始化I2C
 * @return ESP_OK: 成功, ESP_FAIL: 失败
 */
esp_err_t IIC_Init(void);

/**
 * @brief 写入数据到I2C设备
 * @param device_addr 设备地址
 * @param reg_addr 寄存器地址
 * @param data 要写入的数据
 * @return ESP_OK: 成功, ESP_FAIL: 失败
 */
esp_err_t IIC_Write_Byte(uint8_t device_addr, uint8_t reg_addr, uint8_t data);

/**
 * @brief 从I2C设备读取数据
 * @param device_addr 设备地址
 * @param reg_addr 寄存器地址
 * @param data 读取的数据存储位置
 * @return ESP_OK: 成功, ESP_FAIL: 失败
 */
esp_err_t IIC_Read_Byte(uint8_t device_addr, uint8_t reg_addr, uint8_t *data);

#endif
