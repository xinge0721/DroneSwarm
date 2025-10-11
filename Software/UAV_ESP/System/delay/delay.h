#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/**
 * @brief 类似STM32 SysTick的系统滴答延时类
 * 基于ESP32高精度定时器实现，提供微秒级精度
 * 兼容ESP32-S3/C3/C6等所有ESP32系列芯片
 */
class SysTickDelay {
private:
    static bool initialized;
    static uint64_t tick_frequency;  // 滴答频率(Hz)
    static uint32_t cpu_frequency_mhz;  // CPU频率(MHz)
    static const char* TAG;
    
public:
    /**
     * @brief 初始化系统滴答
     * @param freq 滴答频率，默认1MHz (1μs精度)
     * @note 类似STM32的SysTick_Config()，自动检测CPU频率
     */
    static void init(uint32_t freq = 1000000);
    
    /**
     * @brief 获取当前CPU频率
     * @return CPU频率(MHz)
     * @note 自动检测，兼容所有ESP32系列
     */
    static uint32_t get_cpu_freq_mhz();
    
    /**
     * @brief 获取系统滴答数
     * @return 64位滴答计数 
     * @note 类似STM32的HAL_GetTick()，但精度更高
     */
    static inline uint64_t get_tick() {
        return esp_timer_get_time();  // 返回微秒数
    }
    
    /**
     * @brief 获取系统运行时间(毫秒)
     * @return 32位毫秒数，类似STM32 HAL_GetTick()
     */
    static inline uint32_t get_tick_ms() {
        return (uint32_t)(esp_timer_get_time() / 1000);
    }
    
    /**
     * @brief 系统滴答延时(类似STM32 HAL_Delay)
     * @param ms 延时时间(毫秒)
     * @note 基于系统滴答实现，类似STM32的HAL_Delay()
     */
    static void delay(uint32_t ms);
    
    /**
     * @brief 微秒级延时
     * @param us 延时时间(微秒)
     * @note 类似STM32的DWT延时，高精度
     */
    static void delay_us(uint32_t us);
    
    /**
     * @brief 检查延时是否超时
     * @param start_tick 开始时的滴答数
     * @param timeout_ms 超时时间(毫秒)
     * @return true=超时, false=未超时
     * @note 用于非阻塞延时判断
     */
    static inline bool is_timeout(uint64_t start_tick, uint32_t timeout_ms) {
        return (get_tick() - start_tick) >= (timeout_ms * 1000);
    }
    
    /**
     * @brief 等待指定时间后返回
     * @param us 等待时间(微秒)
     * @note 非阻塞等待，返回剩余时间
     */
    static uint32_t wait_us(uint32_t us);
};

/**
 * @brief 兼容性延时类 (保持原有接口)
 */
class Delay {
public:
    /**
     * @brief 微秒级延时
     * @param us 延时时间(微秒)
     */
    static inline void delay_us(uint32_t us) {
        SysTickDelay::delay_us(us);
    }
    
    /**
     * @brief 毫秒级延时
     * @param ms 延时时间(毫秒)  
     */
    static inline void delay_ms(uint32_t ms) {
        SysTickDelay::delay(ms);
    }
    
    /**
     * @brief I2C时序延时
     */
    static inline void i2c_delay() {
        SysTickDelay::delay_us(2);
    }
    
    /**
     * @brief SPI时序延时  
     */
    static inline void spi_delay() {
        SysTickDelay::delay_us(1);
    }
    
    /**
     * @brief 纳秒级延时
     * @param ns 延时时间(纳秒)
     */
    static void delay_ns(uint32_t ns);
};

// 兼容C风格的函数接口
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief C接口：微秒延时
 */
void delay_us(uint32_t us);

/**
 * @brief C接口：毫秒延时
 */
void delay_ms(uint32_t ms);

/**
 * @brief C接口：I2C延时
 */
void i2c_delay(void);

/**
 * @brief STM32风格：获取系统滴答数(毫秒)
 * @return 系统运行时间(毫秒)
 * @note 完全兼容STM32 HAL_GetTick()函数
 */
uint32_t HAL_GetTick(void);

/**
 * @brief STM32风格：系统延时
 * @param ms 延时时间(毫秒)
 * @note 完全兼容STM32 HAL_Delay()函数
 */
void HAL_Delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // DELAY_H
