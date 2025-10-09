#ifndef TIME_H
#define TIME_H

// ============ 头文件 ============
#include <Arduino.h>
#include "esp_log.h"

// ================ 回调函数类型定义 ================
typedef void (*timer_callback_t)(void);

// ================ 类定义 ================
class TIME {
private:
    hw_timer_t *timer_handle;
    bool is_initialized;
    timer_callback_t callback_function;  // 用户回调函数指针
    
public:
    // ================ 公共变量 ================
    volatile bool timer_flag;          // 定时器中断标志
    volatile unsigned long timer_count; // 定时器计数器
    
    // ================ 构造函数（自动初始化） ================
    TIME(timer_callback_t callback = nullptr);
    
    // ================ 启动定时器 ================
    void start(void);
    
    // ================ 停止定时器 ================
    void stop(void);
    
    // ================ 重置计数器 ================
    void reset_count(void);
    
    // ================ 获取计数值 ================
    unsigned long get_count(void);
    
    // ================ 设置回调函数 ================
    void set_callback(timer_callback_t callback);
    
    // ================ 移除回调函数 ================
    void remove_callback(void);
};

#endif
