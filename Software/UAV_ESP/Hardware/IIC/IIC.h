#ifndef IIC_H
#define IIC_H

#include "driver/i2c.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_err.h"

class IIC {
private:
    i2c_config_t _conf;

public:
    IIC(i2c_mode_t mode = I2C_MODE_MASTER, /*!< I2C模式 */
        gpio_num_t sda_io_num = GPIO_NUM_8, /*!< I2C数据线 SDA */
        gpio_num_t scl_io_num = GPIO_NUM_9, /*!< I2C时钟线 SCL */
        gpio_pullup_t sda_pullup_en = GPIO_PULLUP_ENABLE, /*!< I2C数据线拉高 */
        gpio_pullup_t scl_pullup_en = GPIO_PULLUP_ENABLE, /*!< I2C时钟线拉高 */
        uint32_t clk_speed = 100000 /*!< I2C时钟频率 */
    );

    esp_err_t Write(uint8_t device_addr, uint8_t reg_addr, uint8_t data);
    esp_err_t Read(uint8_t device_addr, uint8_t reg_addr, uint8_t *data);


};
#endif


