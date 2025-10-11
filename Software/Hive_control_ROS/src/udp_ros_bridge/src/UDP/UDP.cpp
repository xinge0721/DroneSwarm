// ====================== 头文件======================
#include "UDP.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

// ====================== 模板构造函数实现======================
template<typename DataType>
UDP<DataType>::UDP(int port) : sockfd(-1), running(false), server_port(port) {
    // 初始化服务器地址
    memset(&server_addr, 0, sizeof(server_addr));

    // 创建socket
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
    
    std::cout << "UDP模板服务器构建成功，端口: " << port << std::endl;
}

// ====================== 模板析构函数实现 ======================
template<typename DataType>
UDP<DataType>::~UDP() {
    stop();
}

// ====================== 开始监听实现 ======================
template<typename DataType>
void UDP<DataType>::startListening() {
    if (sockfd < 0) {
        std::cerr << "UDP server not initialized" << std::endl;
        return;
    }
    
    running = true;
    std::cout << "UDP模板服务器开始监听" << std::endl;

    // 在新线程中运行接收循环
    std::thread receive_thread(&UDP<DataType>::receiveLoop, this);
    receive_thread.detach();
}

// ====================== 停止服务器实现 ======================
template<typename DataType>
void UDP<DataType>::stop() {
    running = false;
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
        std::cout << "UDP模板服务器停止监听" << std::endl;
    }
}

// ====================== 设置回调函数实现 ======================
template<typename DataType>
void UDP<DataType>::setMessageCallback(std::function<void(const DataType&, const std::string&, int)> callback) {
    message_callback = callback;
}

// ====================== 发送消息实现 ======================
template<typename DataType>
bool UDP<DataType>::sendTo(const DataType& message, const std::string& ip, int port) {
    if (sockfd < 0) {
        std::cerr << "UDP 未初始化" << std::endl;
        return false;
    }
    
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr) <= 0) {
        std::cerr << "无效的IP地址: " << ip << std::endl;
        return false;
    }
    
    // 序列化数据
    std::vector<uint8_t> serialized_data = serializeData(message);
    
    ssize_t sent_bytes = sendto(sockfd, serialized_data.data(), serialized_data.size(), 0,
                               (struct sockaddr*)&client_addr, sizeof(client_addr));
    
    if (sent_bytes < 0) {
        std::cerr << "发送失败到 " << ip << ":" << port << std::endl;
        return false;
    }
    
    std::cout << "发送成功到 " << ip << ":" << port << " (数据长度: " << serialized_data.size() << ")" << std::endl;
    return true;
}

// ====================== 获取单个消息实现 ======================
template<typename DataType>
bool UDP<DataType>::getReceivedMessage(UDPMessage<DataType>& message) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    if (message_cache.empty()) {
        return false;
    }
    message = message_cache.front();
    message_cache.pop();
    return true;
}

// ====================== 获取消息队列实现 ======================
template<typename DataType>
std::queue<DataType> UDP<DataType>::getMessageQueue() {
    std::lock_guard<std::mutex> lock(cache_mutex);
    std::queue<DataType> data_queue;
    
    // 提取所有消息的数据部分
    while (!message_cache.empty()) {
        data_queue.push(message_cache.front().data);
        message_cache.pop();
    }
    
    return data_queue;
}

// ====================== 获取消息数量实现 ======================
template<typename DataType>
size_t UDP<DataType>::getMessageCount() {
    std::lock_guard<std::mutex> lock(cache_mutex);
    return message_cache.size();
}

// ====================== 清空消息缓存实现 ======================
template<typename DataType>
void UDP<DataType>::clearMessageCache() {
    std::lock_guard<std::mutex> lock(cache_mutex);
    while (!message_cache.empty()) {
        message_cache.pop();
    }
    std::cout << "消息缓存清空" << std::endl;
}

// ====================== 接收循环实现 ======================
template<typename DataType>
void UDP<DataType>::receiveLoop() {
    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr*)&client_addr, &client_len);
        
        if (recv_len > 0) {
            try {
                // 使用模板特化进行数据转换
                DataType message_data = convertRawData(buffer, recv_len);
                std::string client_ip = inet_ntoa(client_addr.sin_addr);
                int client_port = ntohs(client_addr.sin_port);
                
                std::cout << "收到来自 " << client_ip << ":" << client_port 
                         << " (数据长度: " << recv_len << ")" << std::endl;
                
                // 如果设置了回调函数，直接调用
                if (message_callback) {
                    message_callback(message_data, client_ip, client_port);
                } else {
                    // 否则存入缓存
                    UDPMessage<DataType> msg;
                    msg.data = message_data;
                    msg.client_ip = client_ip;
                    msg.client_port = client_port;
                    
                    std::lock_guard<std::mutex> lock(cache_mutex);
                    message_cache.push(msg);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "数据转换失败: " << e.what() << std::endl;
            }
        }
    }
}

// ====================== 数据转换特化实现 ======================
// 字符串类型特化
template<>
std::string UDP<std::string>::convertRawData(const char* buffer, size_t length) {
    return std::string(buffer, length);
}

template<>
std::vector<uint8_t> UDP<std::string>::serializeData(const std::string& data) {
    return std::vector<uint8_t>(data.begin(), data.end());
}

// JSON类型特化
template<>
nlohmann::json UDP<nlohmann::json>::convertRawData(const char* buffer, size_t length) {
    try {
        std::string json_str(buffer, length);
        return nlohmann::json::parse(json_str);
    } catch (const std::exception& e) {
        throw std::runtime_error("JSON解析失败: " + std::string(e.what()));
    }
}

template<>
std::vector<uint8_t> UDP<nlohmann::json>::serializeData(const nlohmann::json& data) {
    std::string json_str = data.dump();
    return std::vector<uint8_t>(json_str.begin(), json_str.end());
}

// 二进制类型特化
template<>
std::vector<uint8_t> UDP<std::vector<uint8_t>>::convertRawData(const char* buffer, size_t length) {
    return std::vector<uint8_t>(buffer, buffer + length);
}

template<>
std::vector<uint8_t> UDP<std::vector<uint8_t>>::serializeData(const std::vector<uint8_t>& data) {
    return data; // 二进制数据直接返回
}

// ====================== 显式实例化 ======================
template class UDP<std::string>;
template class UDP<nlohmann::json>;
template class UDP<std::vector<uint8_t>>;