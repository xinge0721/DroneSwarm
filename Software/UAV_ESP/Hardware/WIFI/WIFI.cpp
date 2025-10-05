#include "WIFI.h"

// 静态成员变量定义
EventGroupHandle_t WIFI::s_wifi_event_group = nullptr;
const int WIFI::WIFI_CONNECTED_BIT = BIT0;
bool WIFI::is_connected = false;

static const char *TAG = "WIFI";

// 静态事件处理函数
void WIFI::wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        is_connected = false;
        ESP_LOGI(TAG, "WIFI连接断开，正在重连...");
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "获得IP地址:" IPSTR, IP2STR(&event->ip_info.ip));
        is_connected = true;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

//  ================ 连接WIFI ================
// 默认连接"xinge"网络，密码为"99999999"
// 构造函数直接完成所有初始化和连接工作
WIFI::WIFI(const char* ssid, const char* password)
    : ssid(ssid), password(password)
{
    ESP_LOGI(TAG, "开始连接WIFI: %s", ssid);
    
    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 创建事件组
    if (s_wifi_event_group == nullptr) {
        s_wifi_event_group = xEventGroupCreate();
    }

    // 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());
    
    // 创建默认事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // 创建默认WIFI STA
    esp_netif_create_default_wifi_sta();

    // 初始化WIFI
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册事件处理程序
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    // 配置WIFI参数
    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, ssid);
    strcpy((char*)wifi_config.sta.password, password);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // 等待连接完成
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                          WIFI_CONNECTED_BIT,
                                          pdFALSE,
                                          pdFALSE,
                                          10000 / portTICK_PERIOD_MS);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WIFI连接成功!");
    } else {
        ESP_LOGE(TAG, "WIFI连接超时");
    }
}

//  ================ 断开WIFI ================
WIFI::~WIFI()
{
    if (is_connected) {
        esp_wifi_disconnect();
        esp_wifi_stop();
        is_connected = false;
    }
    
    if (s_wifi_event_group) {
        vEventGroupDelete(s_wifi_event_group);
        s_wifi_event_group = nullptr;
    }
}

//  ================ 判断是否连接WIFI ================
bool WIFI::is_wifi_connected()
{
    return is_connected;
}

//  ================ 获取IP地址 ================
const char* WIFI::get_ip_address()
{
    static char ip_str[16];
    if (!is_connected) {
        strcpy(ip_str, "未连接");
        return ip_str;
    }

    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif == nullptr) {
        strcpy(ip_str, "获取失败");
        return ip_str;
    }

    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
        sprintf(ip_str, IPSTR, IP2STR(&ip_info.ip));
    } else {
        strcpy(ip_str, "获取失败");
    }
    
    return ip_str;
}


//  ================发送一个字节（byte）数据===============
void WIFI::send_byte(uint8_t data)
{
    // 发送一个字节（byte）数据
    ESP_LOGI(TAG, "发送字节: 0x%02X", data);
}

//  ================发送一个数组（array）数据===============
void WIFI::send_array(uint8_t* data, uint8_t length)
{
    // 发送一个数组（array）数据
    ESP_LOGI(TAG, "发送数组，长度: %d", length);
}

//  ================接受数据===============
uint8_t WIFI::receive_byte()
{
    // 接受一个字节（byte）数据
    ESP_LOGI(TAG, "接收字节");
    return 0;
}

//  ================接受一个数组（array）数据===============
uint8_t* WIFI::receive_array(uint8_t length)
{
    // 接受一个数组（array）数据
    ESP_LOGI(TAG, "接收数组，长度: %d", length);
    return nullptr;
}

//  ================接受全部数据===============
uint8_t* WIFI::receive_all()
{
    // 接受全部数据
    ESP_LOGI(TAG, "接收全部数据");
    return nullptr;
}