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
    // 套接字文件描述符
    int socket_fd = -1;
    // 服务器地址结构
    struct sockaddr_in server_addr;
    // 连接状态标志
    bool is_connected = false;

public:
    // 构造函数
    UDP(const char* server_ip = "192.168.0.102", uint16_t port = 8888);
    
    // 析构函数
    ~UDP();
    
    // 发送数据到服务器
    bool send_data(uint8_t* data, uint8_t length);
    
    // 从服务器接收数据（非阻塞）
    int receive_data(uint8_t* buffer, uint8_t max_length);
    
};

#endif // UDP_H
