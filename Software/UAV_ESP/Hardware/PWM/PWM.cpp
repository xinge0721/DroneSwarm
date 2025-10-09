#include "PWM.h"

static const char *TAG = "PWM";

//  ================ 构造函数（自动初始化） ================
PWM::PWM(gpio_num_t pin1, gpio_num_t pin2, gpio_num_t pin3, gpio_num_t pin4)
{
    // 初始化引脚数组
    pwm_pins[0] = pin1;
    pwm_pins[1] = pin2;
    pwm_pins[2] = pin3;
    pwm_pins[3] = pin4;
    
    // 初始化通道数组
    pwm_channels[0] = LEDC_CHANNEL_0;
    pwm_channels[1] = LEDC_CHANNEL_1;
    pwm_channels[2] = LEDC_CHANNEL_2;
    pwm_channels[3] = LEDC_CHANNEL_3;
    
    is_initialized = false;
    
    ESP_LOGI(TAG, "开始初始化PWM - 4个通道");
    
    // 配置定时器
    ledc_timer_config_t ledc_timer = {
        .speed_mode = PWM_SPEED_MODE,
        .timer_num = PWM_TIMER,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
    
    // 配置四个PWM通道
    for (int i = 0; i < 4; i++) {
        ledc_channel_config_t ledc_channel = {
            .gpio_num = pwm_pins[i],
            .speed_mode = PWM_SPEED_MODE,
            .channel = pwm_channels[i],
            .timer_sel = PWM_TIMER,
            .duty = 0,
            .hpoint = 0
        };
        ledc_channel_config(&ledc_channel);
        ESP_LOGI(TAG, "PWM通道%d初始化完成，引脚: GPIO%d", i+1, pwm_pins[i]);
    }
    
    is_initialized = true;
    ESP_LOGI(TAG, "PWM初始化完成 - 频率: %dHz", PWM_FREQUENCY);
}

//  ================ 设置单个通道占空比 ================
void PWM::set_duty(int channel, uint32_t duty)
{
    if (!is_initialized) {
        ESP_LOGE(TAG, "PWM未初始化");
        return;
    }
    
    if (channel < 1 || channel > 4) {
        ESP_LOGE(TAG, "通道编号错误: %d (应为1-4)", channel);
        return;
    }
    
    // 限制占空比范围 (0-1023)
    if (duty > 1023) duty = 1023;
    
    ledc_set_duty(PWM_SPEED_MODE, pwm_channels[channel-1], duty);
    ledc_update_duty(PWM_SPEED_MODE, pwm_channels[channel-1]);
}

//  ================ 设置所有通道占空比 ================
void PWM::set_all_duty(uint32_t duty1, uint32_t duty2, uint32_t duty3, uint32_t duty4)
{
    set_duty(1, duty1);
    set_duty(2, duty2);
    set_duty(3, duty3);
    set_duty(4, duty4);
}

//  ================ 停止所有电机 ================
void PWM::stop_all()
{
    set_all_duty(0, 0, 0, 0);
    ESP_LOGI(TAG, "所有电机已停止");
}
