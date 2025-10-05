#include "buzzer.h"


//  ================ 构造函数（自动初始化） ================
Buzzer::Buzzer(gpio_num_t pin)
{
    buzzer_pin = pin;
    is_on = false;
    
    // 配置GPIO为输出模式
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << buzzer_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // 初始状态为关闭
    gpio_set_level(buzzer_pin, 0);
    
}

//  ================ 开启蜂鸣器 ================
void Buzzer::on()
{
    gpio_set_level(buzzer_pin, 1);
    is_on = true;
}

//  ================ 关闭蜂鸣器 ================
void Buzzer::off()
{
    gpio_set_level(buzzer_pin, 0);
    is_on = false;
}

//  ================ 翻转状态 ================
void Buzzer::toggle()
{
    if (is_on) {
        off();
    } else {
        on();
    }
}
