#include "TIME.h"

static const char *TAG = "TIME";

// ================ 定时器报警回调函数 ================
static bool IRAM_ATTR timer_alarm_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    TIME *timer_instance = (TIME *)user_ctx;
    
    if (timer_instance) {
        timer_instance->timer_flag = true;
        timer_instance->timer_count++;
        
        if (timer_instance->callback_function) {
            timer_instance->callback_function();
        }
    }
    
    return false;
}

//  ================ 构造函数 ================
TIME::TIME(timer_callback_t callback, uint32_t period_us)
{
    timer_handle = nullptr;
    timer_flag = false;
    timer_count = 0;
    is_initialized = false;
    is_running = false;
    callback_function = callback;
    timer_period_us = period_us;
    timer_frequency_hz = (period_us > 0) ? (1000000 / period_us) : 0;
}

//  ================ 析构函数 ================
TIME::~TIME()
{
    if (is_initialized) {
        if (is_running) {
            gptimer_stop(timer_handle);
        }
        gptimer_disable(timer_handle);
        gptimer_del_timer(timer_handle);
    }
}

//  ================ 初始化定时器 ================
bool TIME::init(timer_config_st *config)
{
    if (is_initialized) {
        return true;
    }
    
    if (config != nullptr) {
        timer_period_us = config->timer_period_us;
        timer_frequency_hz = (timer_period_us > 0) ? (1000000 / timer_period_us) : 0;
        if (config->callback != nullptr) {
            callback_function = config->callback;
        }
    }
    
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
        .intr_priority = 0,
    };
    
    esp_err_t ret = gptimer_new_timer(&timer_config, &timer_handle);
    if (ret != ESP_OK) {
        return false;
    }
    
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_alarm_callback,
    };
    ret = gptimer_register_event_callbacks(timer_handle, &cbs, this);
    if (ret != ESP_OK) {
        gptimer_del_timer(timer_handle);
        return false;
    }
    
    ret = gptimer_enable(timer_handle);
    if (ret != ESP_OK) {
        gptimer_del_timer(timer_handle);
        return false;
    }
    
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = timer_period_us,
        .reload_count = 0,
        .flags = {
            .auto_reload_on_alarm = true,
        }
    };
    ret = gptimer_set_alarm_action(timer_handle, &alarm_config);
    if (ret != ESP_OK) {
        gptimer_disable(timer_handle);
        gptimer_del_timer(timer_handle);
        return false;
    }
    
    is_initialized = true;
    return true;
}

//  ================ 启动定时器 ================
bool TIME::start()
{
    if (!is_initialized) {
        return false;
    }
    
    if (is_running) {
        return true;
    }
    
    esp_err_t ret = gptimer_start(timer_handle);
    if (ret != ESP_OK) {
        return false;
    }
    
    is_running = true;
    return true;
}

//  ================ 停止定时器 ================
bool TIME::stop()
{
    if (!is_initialized) {
        return false;
    }
    
    if (!is_running) {
        return true;
    }
    
    esp_err_t ret = gptimer_stop(timer_handle);
    if (ret != ESP_OK) {
        return false;
    }
    
    is_running = false;
    return true;
}

//  ================ 暂停定时器 ================
bool TIME::pause()
{
    return stop();
}

//  ================ 恢复定时器 ================
bool TIME::resume()
{
    return start();
}

//  ================ 重置计数器 ================
void TIME::reset_count()
{
    timer_count = 0;
}

//  ================ 获取计数值 ================
unsigned long TIME::get_count()
{
    return timer_count;
}

//  ================ 清除标志 ================
void TIME::clear_flag()
{
    timer_flag = false;
}

//  ================ 设置周期（微秒） ================
bool TIME::set_period_us(uint32_t period_us)
{
    if (!is_initialized) {
        timer_period_us = period_us;
        timer_frequency_hz = (period_us > 0) ? (1000000 / period_us) : 0;
        return true;
    }
    
    bool was_running = is_running;
    
    if (was_running) {
        stop();
    }
    
    timer_period_us = period_us;
    timer_frequency_hz = (period_us > 0) ? (1000000 / period_us) : 0;
    
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = timer_period_us,
        .reload_count = 0,
        .flags = {
            .auto_reload_on_alarm = true,
        }
    };
    esp_err_t ret = gptimer_set_alarm_action(timer_handle, &alarm_config);
    if (ret != ESP_OK) {
        return false;
    }
    
    if (was_running) {
        start();
    }
    
    return true;
}

//  ================ 获取周期（微秒） ================
uint32_t TIME::get_period_us()
{
    return timer_period_us;
}

//  ================ 获取频率（Hz） ================
uint32_t TIME::get_frequency_hz()
{
    return timer_frequency_hz;
}

//  ================ 设置回调函数 ================
void TIME::set_callback(timer_callback_t callback)
{
    callback_function = callback;
}

//  ================ 移除回调函数 ================
void TIME::remove_callback()
{
    callback_function = nullptr;
}

//  ================ 检查是否初始化 ================
bool TIME::is_init()
{
    return is_initialized;
}

//  ================ 检查是否运行中 ================
bool TIME::is_active()
{
    return is_running;
}

//  ================ 获取定时器句柄 ================
gptimer_handle_t TIME::get_handle()
{
    return timer_handle;
}

