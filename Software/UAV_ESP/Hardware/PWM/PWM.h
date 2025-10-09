#ifndef PWM_H
#define PWM_H


#include "driver/ledc.h"
#include "esp_log.h"

class PWM {
private:
    gpio_num_t pwm_pins[4];
    ledc_channel_t pwm_channels[4];
    bool is_initialized;
    
    static const int PWM_FREQUENCY = 1000;  // 1KHz频率，适合空心杯电机
    static const ledc_timer_bit_t PWM_RESOLUTION = LEDC_TIMER_10_BIT;  // 10位分辨率 (0-1023)
    static const ledc_timer_t PWM_TIMER = LEDC_TIMER_0;
    static const ledc_mode_t PWM_SPEED_MODE = LEDC_LOW_SPEED_MODE;

public:
    // 构造函数（自动初始化）
    PWM(gpio_num_t pin1 = GPIO_NUM_16, gpio_num_t pin2 = GPIO_NUM_17, 
        gpio_num_t pin3 = GPIO_NUM_18, gpio_num_t pin4 = GPIO_NUM_19);
    
    // 设置单个通道占空比 (0-1023)
    void set_duty(int channel, uint32_t duty);
    
    // 设置所有通道占空比
    void set_all_duty(uint32_t duty1, uint32_t duty2, uint32_t duty3, uint32_t duty4);
    
    // 停止所有电机
    void stop_all();
};


#endif
