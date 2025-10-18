#include "sys.h"
#include "esp_log.h"

// ============ 算法定时器回调函数（前向声明） ============
void arithmetic_time(void);
static const char *TAG = "SYS";
// ============ 全局对象定义 ============
TIME PID_time(arithmetic_time, 10000);  // 10ms周期定时器
MPU6050* mpu6050 = nullptr;  // 全局指针，在 main 中初始化

int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
float Temperature;
volatile uint32_t interrupt_count = 0;  // 中断计数器，用于调试
volatile bool mpu6050_data_ready = false;  // 数据就绪标志

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

//  ========== MPU6050 中断回调函数 ============
void IRAM_ATTR mpu6050_Interrupt_function(void* arg)
{
    // ⚠️ ISR中禁止I2C通信（会使用信号量导致崩溃）
    // 只设置标志位，让主任务在非中断上下文中读取数据
    interrupt_count++;  // 计数，用于调试
    mpu6050_data_ready = true;  // 设置数据就绪标志
}