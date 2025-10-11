// ====================== 头文件======================
#include "UDP.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

// ====================== 静态变量======================


// ====================== 构造函数======================
// 函数名：构造函数
// 参数一：服务器端口号
UDP::UDP(int port) : sockfd(-1), running(false), server_port(port) {
    // 初始化服务器地址
    // 清空服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    // 自动初始化UDP服务器
    if (!init(port)) {
        std::cerr << "Failed to auto-initialize UDP server on port " << port << std::endl;
    }

    // 创建socket
    // 参数一：地址族
    // 参数二：套接字类型
    // 参数三：协议类型
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }
    
    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    server_port = port;
    
    // 绑定socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket to port " << port << std::endl;
        close(sockfd);
        sockfd = -1;
        return;
    }
    
    std::cout << "UDP Server initialized on port " << port << std::endl;
    return;
}
// ====================== 析构函数 ======================
UDP::~UDP() {
    stop();
}


// ====================== 开始监听 ======================
void UDP::startListening() {
    if (sockfd < 0) {
        std::cerr << "UDP server not initialized" << std::endl;
        return;
    }
    
    running = true;
    std::cout << "UDP Server started listening..." << std::endl;

    // 在新线程中运行接收循环
    std::thread receive_thread(&UDP::receiveLoop, this);
    receive_thread.detach();
}

// ====================== 停止服务器 ======================
void UDP::stop() {
    running = false;
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
        std::cout << "UDP Server stopped" << std::endl;
    }
}

// ====================== 设置消息接收回调函数 ======================
// 函数名：设置消息接收回调函数
// 参数一：回调函数
// 参数二：消息
// 参数三：客户端IP
// 参数四：客户端端口

void UDP::setMessageCallback(std::function<void(const std::string&, const std::string&, int)> callback) {
    message_callback = callback;
}

// ====================== 发送消息到指定客户端 ======================
// 函数名：发送消息到指定客户端
// 参数一：消息
// 参数二：客户端IP
// 参数三：客户端端口
// 返回值：是否发送成功
bool UDP::sendTo(const std::string& message, const std::string& ip, int port) {
    if (sockfd < 0) {
        std::cerr << "UDP server not initialized" << std::endl;
        return false;
    }
    
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip << std::endl;
        return false;
    }
    
    ssize_t sent_bytes = sendto(sockfd, message.c_str(), message.length(), 0,
                               (struct sockaddr*)&client_addr, sizeof(client_addr));
    
    if (sent_bytes < 0) {
        std::cerr << "Failed to send message to " << ip << ":" << port << std::endl;
        return false;
    }
    
    std::cout << "Sent to " << ip << ":" << port << " - " << message << std::endl;
    return true;
}

// ====================== 获取接收到的消息 ======================
// 函数名：获取接收到的消息
// 参数一：消息
// 返回值：是否获取成功
bool UDP::getReceivedMessage(UDPMessage& message) {
    // 加锁
    std::lock_guard<std::mutex> lock(cache_mutex);
    if (message_cache.empty()) {
        return false;
    }
    // 获取消息缓存中的第一个消息
    message = message_cache.front();
    message_cache.pop();
    return true;
}

// ====================== 获取消息缓存数量 ======================
// 函数名：获取消息缓存数量
// 返回值：消息缓存数量
size_t UDP::getMessageCount() {
    std::lock_guard<std::mutex> lock(cache_mutex);
    return message_cache.size();
}

// ====================== 清空消息缓存 ======================
// 函数名：清空消息缓存
void UDP::clearMessageCache() {
    std::lock_guard<std::mutex> lock(cache_mutex);
    while (!message_cache.empty()) {
        message_cache.pop();
    }
    std::cout << "Message cache cleared" << std::endl;
}
// ====================== 接收循环 ======================
// 函数名：接收循环
void UDP::receiveLoop() {
    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr*)&client_addr, &client_len);
        
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            std::string message_data(buffer);
            std::string client_ip = inet_ntoa(client_addr.sin_addr);
            int client_port = ntohs(client_addr.sin_port);
            
            std::cout << "Received from " << client_ip << ":" << client_port 
                     << " - " << message_data << std::endl;
            
            // 如果设置了回调函数，直接调用
            if (message_callback) {
                message_callback(message_data, client_ip, client_port);
            } else {
                // 否则存入缓存
                UDPMessage msg;
                msg.data = message_data;
                msg.client_ip = client_ip;
                msg.client_port = client_port;
                
                std::lock_guard<std::mutex> lock(cache_mutex);
                message_cache.push(msg);
            }
        }
    }
}
