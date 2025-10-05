/* ESP32 无人机通信示例
 * 
 * 简单的无人机和上位机通信例子
 * 无人机连接WIFI后，通过UDP和上位机通信
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "../Hardware/WIFI/WIFI.h"
#include "../Hardware/UDP/UDP.h"

static const char *TAG = "MAIN";

extern "C" int app_main(void)
{
    ESP_LOGI(TAG, "无人机通信系统启动");

    // 第一步：连接WIFI（构造函数自动连接）
    WIFI wifi("MERCURY_541F", "qwer1234z");  // 改成你的WIFI名和密码
    
    if (!wifi.is_wifi_connected()) {
        ESP_LOGE(TAG, "WIFI连接失败");
        return -1;
    }
    
    ESP_LOGI(TAG, "WIFI连接成功，IP: %s", wifi.get_ip_address());
    
    // 第二步：连接UDP服务器（上位机）
    UDP udp;
    if (!udp.connect_server("192.168.0.102", 8888)) {  // 你的电脑IP
        ESP_LOGE(TAG, "UDP连接失败");
        return -1;
    }
        
    // 第三步：主循环 - 收发数据
    uint8_t send_data[10];
    uint8_t receive_buffer[255];
    
    while (true) {
        // 发送数据给上位机
        for (int i = 0; i < 10; i++) {
            send_data[i] = i + 1;  // 发送 1,2,3...10
        }
        
        if (udp.send_data(send_data, 10)) {
            ESP_LOGI(TAG, "发送数据成功");
        }
        
        // 接收上位机指令
        int received = udp.receive_data(receive_buffer, 255);
        if (received > 0) {
            ESP_LOGI(TAG, "收到指令，长度: %d", received);
            
            // 打印收到的数据
            printf("收到数据: ");
            for (int i = 0; i < received; i++) {
                printf("%d ", receive_buffer[i]);
            }
            printf("\n");
        }
        
        // 等待1秒
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    return 0;
}