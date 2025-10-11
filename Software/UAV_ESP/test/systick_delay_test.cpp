/**
 * @file systick_delay_test.cpp
 * @brief SysTickDelay系统测试程序
 * @note 测试类似STM32 SysTick的延时功能
 */

#include "../System/delay/delay.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "SysTickTest";

/**
 * @brief 测试基本延时功能
 */
void test_basic_delay() {
    ESP_LOGI(TAG, "=== 测试基本延时功能 ===");
    
    uint32_t start_time = HAL_GetTick();
    ESP_LOGI(TAG, "开始时间: %lu ms", start_time);
    
    // 测试STM32风格延时
    ESP_LOGI(TAG, "HAL_Delay(1000) 开始...");
    HAL_Delay(1000);
    ESP_LOGI(TAG, "HAL_Delay(1000) 完成，实际耗时: %lu ms", HAL_GetTick() - start_time);
    
    // 测试微秒延时
    start_time = HAL_GetTick();
    ESP_LOGI(TAG, "SysTickDelay::delay_us(500000) 开始...");
    SysTickDelay::delay_us(500000);  // 500ms
    ESP_LOGI(TAG, "微秒延时完成，实际耗时: %lu ms", HAL_GetTick() - start_time);
}

/**
 * @brief 测试精度和性能
 */
void test_precision_performance() {
    ESP_LOGI(TAG, "=== 测试精度和性能 ===");
    
    // 测试短延时精度
    const uint32_t test_delays[] = {1, 5, 10, 50, 100, 500, 1000};
    const int test_count = sizeof(test_delays) / sizeof(test_delays[0]);
    
    for (int i = 0; i < test_count; i++) {
        uint32_t target_us = test_delays[i];
        
        // 使用系统滴答测量
        uint64_t start_tick = SysTickDelay::get_tick();
        SysTickDelay::delay_us(target_us);
        uint64_t actual_us = SysTickDelay::get_tick() - start_tick;
        
        int32_t error = (int32_t)actual_us - (int32_t)target_us;
        float error_percent = (float)error / target_us * 100.0f;
        
        ESP_LOGI(TAG, "延时 %4lu μs -> 实际 %4llu μs, 误差: %+4ld μs (%.1f%%)", 
                 target_us, actual_us, error, error_percent);
    }
}

/**
 * @brief 测试超时功能
 */
void test_timeout_function() {
    ESP_LOGI(TAG, "=== 测试超时功能 ===");
    
    uint64_t start_tick = SysTickDelay::get_tick();
    uint32_t timeout_ms = 100;
    
    ESP_LOGI(TAG, "开始100ms超时测试...");
    
    while (!SysTickDelay::is_timeout(start_tick, timeout_ms)) {
        // 模拟做一些工作
        vTaskDelay(10 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "工作中... 已过去 %llu ms", 
                 (SysTickDelay::get_tick() - start_tick) / 1000);
    }
    
    ESP_LOGI(TAG, "超时检测完成，总耗时: %llu ms", 
             (SysTickDelay::get_tick() - start_tick) / 1000);
}

/**
 * @brief 测试兼容性接口
 */
void test_compatibility() {
    ESP_LOGI(TAG, "=== 测试兼容性接口 ===");
    
    uint32_t start = HAL_GetTick();
    
    // 测试C接口
    ESP_LOGI(TAG, "测试C接口延时...");
    delay_ms(200);
    ESP_LOGI(TAG, "delay_ms(200) 完成，耗时: %lu ms", HAL_GetTick() - start);
    
    // 测试I2C延时
    start = HAL_GetTick();
    ESP_LOGI(TAG, "测试I2C延时...");
    for (int i = 0; i < 1000; i++) {
        i2c_delay();  // 2μs × 1000 = 2ms
    }
    ESP_LOGI(TAG, "1000次i2c_delay()完成，耗时: %lu ms", HAL_GetTick() - start);
    
    // 测试Delay类接口
    start = HAL_GetTick();
    ESP_LOGI(TAG, "测试Delay类接口...");
    Delay::delay_ms(300);
    ESP_LOGI(TAG, "Delay::delay_ms(300) 完成，耗时: %lu ms", HAL_GetTick() - start);
}

/**
 * @brief 主测试函数
 */
void systick_delay_test_main() {
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "        SysTickDelay 系统测试");
    ESP_LOGI(TAG, "========================================");
    
    // 初始化SysTickDelay系统
    SysTickDelay::init(1000000);  // 1MHz，微秒精度
    
    ESP_LOGI(TAG, "系统启动时间: %lu ms", HAL_GetTick());
    ESP_LOGI(TAG, "系统启动时间: %llu μs", SysTickDelay::get_tick());
    
    // 运行各项测试
    test_basic_delay();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    test_precision_performance();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    test_timeout_function();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    test_compatibility();
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "           测试完成");
    ESP_LOGI(TAG, "========================================");
}
