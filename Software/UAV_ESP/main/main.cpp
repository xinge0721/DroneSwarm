#include "main.h"


uint8_t receive_buffer[255];

extern "C" int app_main(void)
{

    // //  ================== WIFI 初始化 ================
    // ESP_LOGI(TAG, "无人机通信系统启动");

    // // 第一步：连接WIFI（构造函数自动连接）
    // WIFI wifi("MERCURY_541F", "qwer1234z");  // 改成你的WIFI名和密码
    
    // if (!wifi.is_wifi_connected()) {
    //     ESP_LOGE(TAG, "WIFI连接失败");
    //     return -1;
    // }
    
    // ESP_LOGI(TAG, "WIFI连接成功，IP: %s", wifi.get_ip_address());
    
    // // 第二步：连接UDP服务器（上位机）
    // UDP udp;
    // if (!udp.connect_server("192.168.0.102", 8888)) {  // 你的电脑IP
    //     ESP_LOGE(TAG, "UDP连接失败");
    //     return -1;
    // }
        

    // //  ======================= LED 初始化 =======================
    // LED led ();

    // //  ======================= 蜂鸣器 初始化 =====================
    // BEEP beep();

    // //  ======================= 电机 初始化 =======================
    // MOTOR motor();

    // //  ======================= GPS 初始化 =======================
    // GPS gps();

    //  ======================= MPU6050 初始化 =======================
    // SCL: GPIO9, SDA: GPIO8, INT: GPIO10
    MPU6050 mpu6050(GPIO_NUM_9, GPIO_NUM_8);

    int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
    while (true) {
        mpu6050.GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);
        ESP_LOGI(TAG, "AccX: %d, AccY: %d, AccZ: %d, GyroX: %d, GyroY: %d, GyroZ: %d", AccX, AccY, AccZ, GyroX, GyroY, GyroZ);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    return 0;
}
