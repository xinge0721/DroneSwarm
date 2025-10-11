#include "delay.h"
#include "esp_system.h"
#include "soc/rtc.h"

// ============ SysTickDelay 静态变量定义 ============
bool SysTickDelay::initialized = false;
uint64_t SysTickDelay::tick_frequency = 1000000;  // 默认1MHz
uint32_t SysTickDelay::cpu_frequency_mhz = 0;     // 将自动检测
const char* SysTickDelay::TAG = "SysTickDelay";

// ============ SysTickDelay 类实现 ============

/**
 * @brief 获取当前CPU频率
 */
uint32_t SysTickDelay::get_cpu_freq_mhz() {
    if (cpu_frequency_mhz == 0) {
        // 自动检测CPU频率，兼容所有ESP32系列
        rtc_cpu_freq_config_t freq_config;
        rtc_clk_cpu_freq_get_config(&freq_config);
        cpu_frequency_mhz = freq_config.freq_mhz;
        
        ESP_LOGI(TAG, "检测到CPU频率: %lu MHz", cpu_frequency_mhz);
        
        // 支持的芯片型号
        #ifdef CONFIG_IDF_TARGET_ESP32
            ESP_LOGI(TAG, "芯片型号: ESP32 (Xtensa)");
        #elif defined(CONFIG_IDF_TARGET_ESP32S2)
            ESP_LOGI(TAG, "芯片型号: ESP32-S2 (Xtensa)");
        #elif defined(CONFIG_IDF_TARGET_ESP32S3)
            ESP_LOGI(TAG, "芯片型号: ESP32-S3 (Xtensa)");
        #elif defined(CONFIG_IDF_TARGET_ESP32C3)
            ESP_LOGI(TAG, "芯片型号: ESP32-C3 (RISC-V)");
        #elif defined(CONFIG_IDF_TARGET_ESP32C6)
            ESP_LOGI(TAG, "芯片型号: ESP32-C6 (RISC-V)");
        #elif defined(CONFIG_IDF_TARGET_ESP32H2)
            ESP_LOGI(TAG, "芯片型号: ESP32-H2 (RISC-V)");
        #else
            ESP_LOGI(TAG, "芯片型号: 未知ESP32系列");
        #endif
    }
    return cpu_frequency_mhz;
}

/**
 * @brief 初始化系统滴答
 */
void SysTickDelay::init(uint32_t freq) {
    if (initialized) {
        ESP_LOGW(TAG, "SysTick已经初始化");
        return;
    }
    
    tick_frequency = freq;
    get_cpu_freq_mhz();  // 检测CPU频率
    initialized = true;
    
    ESP_LOGI(TAG, "SysTick初始化完成 - 频率: %lu Hz", freq);
    ESP_LOGI(TAG, "系统滴答精度: %s", freq >= 1000000 ? "微秒级" : "毫秒级");
    ESP_LOGI(TAG, "esp_timer精度: 微秒级 (1μs)");
}

/**
 * @brief 系统滴答延时(类似STM32 HAL_Delay)
 */
void SysTickDelay::delay(uint32_t ms) {
    if (ms == 0) return;
    
    // 获取开始时间戳
    uint64_t start = get_tick();
    uint64_t target = start + (ms * 1000);  // 转换为微秒
    
    // 等待直到目标时间
    while (get_tick() < target) {
        // 短暂让出CPU，避免完全占用
        if (ms > 10) {
            vTaskDelay(1);  // 让出1个FreeRTOS tick
        }
    }
}

/**
 * @brief 微秒级延时  
 */
void SysTickDelay::delay_us(uint32_t us) {
    if (us == 0) return;
    
    if (us < 10) {
        // 对于很短的延时，使用ROM函数
        esp_rom_delay_us(us);
    } else {
        // 使用系统滴答延时
        uint64_t start = get_tick();
        uint64_t target = start + us;
        
        while (get_tick() < target) {
            // 对于长延时，偶尔让出CPU
            if (us > 1000 && (get_tick() - start) % 100 == 0) {
                taskYIELD();
            }
        }
    }
}


/**
 * @brief 等待指定时间后返回
 */
uint32_t SysTickDelay::wait_us(uint32_t us) {
    uint64_t start = get_tick();
    uint64_t target = start + us;
    uint64_t current = get_tick();
    
    if (current >= target) {
        return 0;  // 已超时
    }
    
    return (uint32_t)(target - current);
}

// ============ Delay 类实现 ============

/**
 * @brief 纳秒级延时实现 (跨平台兼容)
 */
void Delay::delay_ns(uint32_t ns) {
    // 自动获取CPU频率，兼容所有ESP32系列
    uint32_t cpu_freq_mhz = SysTickDelay::get_cpu_freq_mhz();
    
    // 计算需要的CPU周期数
    // cycles_per_ns = cpu_freq_mhz / 1000 (MHz转换为cycles/us，再除以1000得cycles/ns)
    uint32_t cycles = (ns * cpu_freq_mhz) / 1000;
    
    if (cycles < 10) {
        // 对于很短的延时，使用CPU循环
        // 使用通用的循环方式，兼容Xtensa和RISC-V架构
        for (volatile uint32_t i = 0; i < cycles; i++) {
            // 空循环，编译器不会优化掉volatile变量
            __asm__ __volatile__("" ::: "memory");
        }
    } else {
        // 对于较长的延时，使用微秒延时
        SysTickDelay::delay_us((ns + 999) / 1000);  // 向上取整到微秒
    }
}

// ============ C接口实现 ============
extern "C" {
    
void delay_us(uint32_t us) {
    SysTickDelay::delay_us(us);
}

void delay_ms(uint32_t ms) {
    SysTickDelay::delay(ms);
}

void i2c_delay(void) {
    SysTickDelay::delay_us(2);
}

// STM32风格的函数接口
uint32_t HAL_GetTick(void) {
    return SysTickDelay::get_tick_ms();
}

void HAL_Delay(uint32_t ms) {
    SysTickDelay::delay(ms);
}

}
