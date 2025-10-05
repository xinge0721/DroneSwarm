#ifndef WIFI_H
#define WIFI_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

class WIFI {
private:
    const char* ssid;
    const char* password;
    static EventGroupHandle_t s_wifi_event_group;
    static const int WIFI_CONNECTED_BIT;
    static bool is_connected;
    
    static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data);

public:
    // 构造函数，直接连接WIFI，默认连接"xinge"网络，密码为"99999999"
    WIFI(const char* ssid = "xinge", const char* password = "99999999");
    
    // 析构函数
    ~WIFI();
    
    // 判断是否连接WIFI
    bool is_wifi_connected();
    
    // 获取IP地址
    const char* get_ip_address();
    
    // 原有的数据收发接口（保持兼容性）
    void send_byte(uint8_t data);
    void send_array(uint8_t* data, uint8_t length);
    uint8_t receive_byte();
    uint8_t* receive_array(uint8_t length);
    uint8_t* receive_all();
};

#endif // WIFI_H