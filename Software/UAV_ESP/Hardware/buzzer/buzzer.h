#ifndef BUZZER_H
#define BUZZER_H

#include "driver/gpio.h"
#include "esp_log.h"

class Buzzer {
private:
    gpio_num_t buzzer_pin;
    bool is_on;

public:
    // 构造函数（自动初始化）
    Buzzer(gpio_num_t pin = GPIO_NUM_25);
    
    // 开启蜂鸣器
    void on();
    
    // 关闭蜂鸣器
    void off();
    
    // 翻转状态
    void toggle();
};

#endif // BUZZER_H
