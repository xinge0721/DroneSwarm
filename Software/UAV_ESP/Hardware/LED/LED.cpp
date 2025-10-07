#include "LED.h"
#include "LED.h"


//  ================ 构造函数（自动初始化） ================
LED::LED(gpio_num_t pin)
{
    LED_pin = pin;
    is_on = false;
    
    // 配置GPIO为输出模式
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // 初始状态为关闭
    gpio_set_level(LED_pin, 0);
    
}

//  ================ 开启LED ================
void LED::on()
{
    gpio_set_level(LED_pin, 1);
    is_on = true;
}

//  ================ 关闭LED ================
void LED::off()
{
    gpio_set_level(LED_pin, 0);
    is_on = false;
}

//  ================ 翻转状态 ================
void LED::toggle()
{
    if (is_on) {
        off();
    } else {
        on();
    }
}
