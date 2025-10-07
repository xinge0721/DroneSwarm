#ifndef LED_H
#define LED_H


#include "driver/gpio.h"
#include "esp_log.h"

class LED {
private:
    gpio_num_t LED_pin;
    bool is_on;

public:
    // 构造函数（自动初始化）
    LED(gpio_num_t pin = GPIO_NUM_21);
    
    // 开启LED
    void on();
    
    // 关闭LED
    void off();
    
    // 翻转状态
    void toggle();    
};


#endif