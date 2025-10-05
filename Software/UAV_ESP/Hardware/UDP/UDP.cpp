#include "UDP.h"

static const char *TAG = "UDP";

//  ================ 创建UDP对象 ================
UDP::UDP()
{
    socket_fd = -1;
    is_server = false;
    is_connected = false;
    client_addr_len = sizeof(client_addr);
    
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    
    ESP_LOGI(TAG, "UDP对象创建完成");
}

//  ================ 释放UDP资源 ================
UDP::~UDP()
{
    close_connection();
}

//  ================ 启动UDP服务器 ================
bool UDP::start_server(uint16_t port)
{
    ESP_LOGI(TAG, "启动UDP服务器，端口: %d", port);
    
    // 创建套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        ESP_LOGE(TAG, "创建套接字失败");
        return false;
    }
    
    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // 绑定端口
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "端口绑定失败");
        close(socket_fd);
        socket_fd = -1;
        return false;
    }
    
    // 设置非阻塞
    int flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    is_server = true;
    is_connected = true;
    
    ESP_LOGI(TAG, "UDP服务器启动成功");
    return true;
}

//  ================ 连接UDP服务器 ================
bool UDP::connect_server(const char* server_ip, uint16_t port)
{
    ESP_LOGI(TAG, "连接UDP服务器: %s:%d", server_ip, port);
    
    // 创建套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        ESP_LOGE(TAG, "创建套接字失败");
        return false;
    }
    
    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        ESP_LOGE(TAG, "无效IP地址: %s", server_ip);
        close(socket_fd);
        socket_fd = -1;
        return false;
    }
    
    // 设置非阻塞
    int flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    is_server = false;
    is_connected = true;
    
    ESP_LOGI(TAG, "UDP连接成功");
    return true;
}

//  ================ 发送数据 ================
bool UDP::send_data(uint8_t* data, uint8_t length)
{
    if (!is_connected || socket_fd < 0) {
        ESP_LOGE(TAG, "UDP未连接");
        return false;
    }
    
    ssize_t sent_bytes;
    
    if (is_server) {
        // 服务器模式：发送给客户端
        sent_bytes = sendto(socket_fd, data, length, 0,
                           (struct sockaddr*)&client_addr, client_addr_len);
    } else {
        // 客户端模式：发送给服务器
        sent_bytes = sendto(socket_fd, data, length, 0,
                           (struct sockaddr*)&server_addr, sizeof(server_addr));
    }
    
    if (sent_bytes == length) {
        ESP_LOGD(TAG, "发送数据成功，长度: %d", length);
        return true;
    } else {
        ESP_LOGE(TAG, "发送数据失败");
        return false;
    }
}

//  ================ 接收数据 ================
int UDP::receive_data(uint8_t* buffer, uint8_t max_length)
{
    if (!is_connected || socket_fd < 0) {
        return -1;
    }
    
    ssize_t received_bytes;
    
    if (is_server) {
        // 服务器模式：接收客户端数据
        received_bytes = recvfrom(socket_fd, buffer, max_length, 0,
                                 (struct sockaddr*)&client_addr, &client_addr_len);
    } else {
        // 客户端模式：接收服务器数据
        socklen_t server_addr_len = sizeof(server_addr);
        received_bytes = recvfrom(socket_fd, buffer, max_length, 0,
                                 (struct sockaddr*)&server_addr, &server_addr_len);
    }
    
    if (received_bytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // 无数据
        } else {
            ESP_LOGE(TAG, "接收数据失败");
            return -1;
        }
    }
    
    ESP_LOGD(TAG, "接收数据成功，长度: %d", (int)received_bytes);
    return (int)received_bytes;
}

//  ================ 检查连接状态 ================
bool UDP::is_udp_connected()
{
    return is_connected && (socket_fd >= 0);
}

//  ================ 关闭连接 ================
void UDP::close_connection()
{
    if (socket_fd >= 0) {
        ESP_LOGI(TAG, "关闭UDP连接");
        close(socket_fd);
        socket_fd = -1;
        is_connected = false;
    }
}
