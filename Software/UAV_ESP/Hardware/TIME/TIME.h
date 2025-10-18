#ifndef TIME_H
#define TIME_H

// ============ 头文件 ============
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gptimer.h"
#include "esp_attr.h"

// ================ 回调函数类型定义 ================
typedef void (*timer_callback_t)(void);

// ================ 定时器配置结构 ================
typedef struct {
    uint32_t timer_period_us;      // 定时器周期（微秒）
    bool auto_reload;              // 是否自动重载
    timer_callback_t callback;     // 回调函数指针
} timer_config_st;

// ================ 类定义 ================
class TIME {
public:
    // ================ 公共变量 ================
    volatile bool timer_flag;            // 定时器中断标志
    volatile unsigned long timer_count;  // 定时器计数器
    timer_callback_t callback_function;  // 用户回调函数指针
    
    // ================ 构造函数（默认10ms周期） ================
    TIME(timer_callback_t callback = nullptr, uint32_t period_us = 10000);
    
    // ================ 析构函数 ================
    ~TIME();
    
    // ================ 初始化定时器 ================
    bool init(timer_config_st *config = nullptr);
    
    // ================ 启动定时器 ================
    bool start(void);
    
    // ================ 停止定时器 ================
    bool stop(void);
    
    // ================ 暂停定时器 ================
    bool pause(void);
    
    // ================ 恢复定时器 ================
    bool resume(void);
    
    // ================ 重置计数器 ================
    void reset_count(void);
    
    // ================ 获取计数值 ================
    unsigned long get_count(void);
    
    // ================ 清除标志 ================
    void clear_flag(void);
    
    // ================ 设置周期（微秒） ================
    bool set_period_us(uint32_t period_us);
    
    // ================ 获取周期（微秒） ================
    uint32_t get_period_us(void);
    
    // ================ 获取频率（Hz） ================
    uint32_t get_frequency_hz(void);
    
    // ================ 设置回调函数 ================
    void set_callback(timer_callback_t callback);
    
    // ================ 移除回调函数 ================
    void remove_callback(void);
    
    // ================ 检查是否初始化 ================
    bool is_init(void);
    
    // ================ 检查是否运行中 ================
    bool is_active(void);
    
    // ================ 获取定时器句柄（供中断使用） ================
    gptimer_handle_t get_handle(void);

private:
    gptimer_handle_t timer_handle;       // 定时器句柄
    bool is_initialized;                 // 初始化标志
    bool is_running;                     // 运行状态标志
    uint32_t timer_period_us;            // 定时器周期（微秒）
    uint32_t timer_frequency_hz;         // 定时器频率（Hz）
};

#endif
