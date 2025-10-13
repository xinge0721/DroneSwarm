#include "UDP.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>

// ====================== 构造函数 ======================
UDP::UDP(int port) : sockfd(-1), running(false), server_port(port) {
    // 初始化服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    
    // 创建UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "创建socket失败" << std::endl;
        return;
    }
    
    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网卡
    server_addr.sin_port = htons(port);
    
    // 绑定socket到端口
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "绑定端口失败: " << port << std::endl;
        close(sockfd);
        sockfd = -1;
        return;
    }
    
    std::cout << "UDP服务器初始化成功，端口: " << port << std::endl;
}

// ====================== 析构函数 ======================
UDP::~UDP() {
    stop();
}

// ====================== 开始监听 ======================
void UDP::startListening() {
    if (sockfd < 0) {
        std::cerr << "UDP服务器未初始化" << std::endl;
        return;
    }
    
    running = true;
    std::cout << "UDP服务器开始监听，端口: " << server_port << std::endl;
    
    // 在新线程中运行接收循环
    std::thread receive_thread(&UDP::receiveLoop, this);
    receive_thread.detach();
}

// ====================== 停止服务 ======================
void UDP::stop() {
    running = false;
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
        std::cout << "UDP服务器已停止" << std::endl;
    }
}

// ====================== 发送数据 ======================
bool UDP::sendTo(const std::vector<uint8_t>& data, const std::string& ip, int port) {
    if (sockfd < 0) {
        std::cerr << "UDP未初始化" << std::endl;
        return false;
    }
    
    // 设置目标地址
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    
    // 转换IP地址
    if (inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr) <= 0) {
        std::cerr << "无效IP地址: " << ip << std::endl;
        return false;
    }
    
    // 发送数据
    ssize_t sent_bytes = sendto(sockfd, data.data(), data.size(), 0,
                                (struct sockaddr*)&client_addr, sizeof(client_addr));
    
    if (sent_bytes < 0) {
        std::cerr << "发送失败到 " << ip << ":" << port << std::endl;
        return false;
    }
    
    std::cout << "发送成功到 " << ip << ":" << port << " (" << data.size() << " 字节)" << std::endl;
    return true;
}

// ====================== 获取消息队列 ======================
std::queue<std::vector<uint8_t>> UDP::getMessageQueue() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    std::queue<std::vector<uint8_t>> result;
    // 使用swap避免拷贝，同时清空原队列
    result.swap(message_queue);
    return result;
}

// ====================== 获取消息数量 ======================
size_t UDP::getMessageCount() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    return message_queue.size();
}

// ====================== 接收循环（在独立线程中运行）======================
void UDP::receiveLoop() {
    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        
        // 阻塞接收数据
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                   (struct sockaddr*)&client_addr, &client_len);
        
        if (recv_len > 0) {
            // 获取客户端信息
            std::string client_ip = inet_ntoa(client_addr.sin_addr);
            int client_port = ntohs(client_addr.sin_port);
            
            std::cout << "收到来自 " << client_ip << ":" << client_port 
                     << " (" << recv_len << " 字节)" << std::endl;
            
            // 将接收到的数据转换为vector并存入队列
            std::vector<uint8_t> data(buffer, buffer + recv_len);
            
            std::lock_guard<std::mutex> lock(queue_mutex);
            message_queue.push(data);
        }
    }
}
