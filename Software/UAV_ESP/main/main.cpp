#include "main.h"

uint8_t receive_buffer[255];



extern "C" int app_main(void)
{

    SysTickDelay::init();
    ESP_LOGI(TAG, "SysTickDelay初始化完成");
    //  ======================= MPU6050 初始化 =======================
    // SCL: GPIO9, SDA: GPIO8, INT: GPIO10
    // 在 main 函数中动态创建，避免全局对象初始化顺序问题
    mpu6050 = new MPU6050(GPIO_NUM_9, GPIO_NUM_8, GPIO_NUM_10, mpu6050_Interrupt_function, nullptr);
    ESP_LOGI(TAG, "MPU6050全局指针已初始化");

    // 测试：直接读取一次，验证IIC通信正常
    int16_t test_x, test_y, test_z, test_gx, test_gy, test_gz;
    mpu6050->GetData(&test_x, &test_y, &test_z, &test_gx, &test_gy, &test_gz);
    ESP_LOGI(TAG, "直接读取测试: AccX: %d, AccY: %d, AccZ: %d", test_x, test_y, test_z);
    ESP_LOGI(TAG, "直接读取测试: GyroX: %d, GyroY: %d, GyroZ: %d", test_gx, test_gy, test_gz);
    
    // 读取中断状态寄存器
    uint8_t int_status = mpu6050->ReadReg(0x3A);
    ESP_LOGI(TAG, "MPU6050中断状态寄存器: 0x%02X", int_status);
    
    while (true) {
        // 检查数据就绪标志（由中断设置）
        if (mpu6050_data_ready) {
            mpu6050_data_ready = false;  // 清除标志


            // 在任务上下文中读取数据（可以安全使用I2C）
            mpu6050->GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);
            Temperature = mpu6050->GetTemperature();
            if (interrupt_count % 100!=0)
            continue;
            ESP_LOGI(TAG, "[中断触发%lu次] AccX: %d, AccY: %d, AccZ: %d", interrupt_count, AccX, AccY, AccZ);
            ESP_LOGI(TAG, "GyroX: %d, GyroY: %d, GyroZ: %d, Temp: %.2f", GyroX, GyroY, GyroZ, Temperature);
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);  // 10ms轮询一次
    }
    return 0;
}


