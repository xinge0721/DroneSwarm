#include "sys.h"
#include "esp_log.h"

// ============ 算法定时器回调函数（前向声明） ============
void arithmetic_time(void);
static const char *TAG = "SYS";
// ============ 全局对象定义 ============
TIME PID_time(arithmetic_time, 10000);  // 10ms周期定时器

// ============ 系统初始化 ============
int system_init(void)
{
    // 初始化定时器
    if (PID_time.init()) {
        // 启动定时器
        if (PID_time.start()) {
            return SYS_ERR_OK;
        } else {
            return SYS_ERR_START;
        }
    } else {
        return SYS_ERR_INIT;
    }
}

// ============ 算法定时器回调函数 ============
void arithmetic_time(void)
{
    // 此函数在定时器中断中调用
    // 可以在这里实现控制算法
    
    // 示例：每1000次（10秒）打印一次计数
    // static uint32_t count = 0;
    // count++;
    // if (count >= 1000) {
    //     count = 0;
    //     ESP_LOGI(TAG, "定时器运行正常 - 计数: %lu", time.get_count());
    // }
}