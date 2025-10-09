#include "TIME.h"

static const char *TAG = "TIME";

// 全局TIME实例指针，用于中断回调
static TIME* timer_instance = nullptr;

// ================ 定时器中断服务程序 ================
void IRAM_ATTR timer_isr(void) {
    if (timer_instance) {
        timer_instance->timer_flag = true;    // 设置中断标志
        timer_instance->timer_count++;        // 计数器自增
        
        // 调用用户回调函数（如果已设置）
        if (timer_instance->callback_function) {
            timer_instance->callback_function();
        }
    }
}

//  ================ 构造函数（自动初始化） ================
TIME::TIME(timer_callback_t callback)
{
    timer_handle = NULL;
    timer_flag = false;
    timer_count = 0;
    is_initialized = false;
    callback_function = callback;  // 设置用户传入的回调函数
    timer_instance = this;  // 设置全局实例指针
    
    ESP_LOGI(TAG, "开始初始化定时器 - 10ms中断");
    
    // 创建定时器
    // 参数：定时器组(0), 分频器(80), 计数向上(true)
    timer_handle = timerBegin(0, 80, true);
    
    // 绑定中断服务程序
    timerAttachInterrupt(timer_handle, &timer_isr, true);
    
    // 设置定时时间：10000微秒 = 10ms
    // 参数：定时器对象, 时间(微秒), 重复触发(true)
    timerAlarmWrite(timer_handle, 10000, true);
    
    is_initialized = true;
    ESP_LOGI(TAG, "定时器初始化完成 - 频率: 100Hz");
    
    if (callback_function) {
        ESP_LOGI(TAG, "回调函数已在构造时设置");
    }
}

//  ================ 启动定时器 ================
void TIME::start()
{
    if (!is_initialized) {
        ESP_LOGE(TAG, "定时器未初始化");
        return;
    }
    
    timerAlarmEnable(timer_handle);
    ESP_LOGI(TAG, "定时器已启动");
}

//  ================ 停止定时器 ================
void TIME::stop()
{
    if (!is_initialized) {
        ESP_LOGE(TAG, "定时器未初始化");
        return;
    }
    
    timerAlarmDisable(timer_handle);
    ESP_LOGI(TAG, "定时器已停止");
}

//  ================ 重置计数器 ================
void TIME::reset_count()
{
    timer_count = 0;
    ESP_LOGI(TAG, "计数器已重置");
}

//  ================ 获取计数值 ================
unsigned long TIME::get_count()
{
    return timer_count;
}

//  ================ 设置回调函数 ================
void TIME::set_callback(timer_callback_t callback)
{
    callback_function = callback;
    ESP_LOGI(TAG, "回调函数已设置");
}

//  ================ 移除回调函数 ================
void TIME::remove_callback()
{
    callback_function = nullptr;
    ESP_LOGI(TAG, "回调函数已移除");
}
