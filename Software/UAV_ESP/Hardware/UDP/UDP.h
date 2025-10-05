#ifndef UDP_H
#define UDP_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

class UDP {
private:
    int socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    bool is_server;
    bool is_connected;

public:
    // 构造函数
    UDP();
    
    // 析构函数
    ~UDP();
    
    // 启动UDP服务器（上位机用）
    bool start_server(uint16_t port);
    
    // 连接UDP服务器（无人机用）
    bool connect_server(const char* server_ip, uint16_t port);
    
    // 发送数据
    bool send_data(uint8_t* data, uint8_t length);
    
    // 接收数据（非阻塞）
    int receive_data(uint8_t* buffer, uint8_t max_length);
    
    // 检查连接状态
    bool is_udp_connected();
    
    // 关闭连接
    void close_connection();
};

#endif // UDP_H
