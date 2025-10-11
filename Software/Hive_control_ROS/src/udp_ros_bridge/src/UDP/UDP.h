#ifndef UDP_H
#define UDP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <functional>
#include <queue>
#include <mutex>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <json.hpp>

// 通用UDP消息结构模板
template<typename DataType>
struct UDPMessage {
    DataType data;              // 支持泛型数据
    std::string client_ip;
    int client_port;
};

template<typename DataType>
class UDP {
public:
    UDP(int port = 8888);
    ~UDP();
    
    // 初始化UDP服务端
    bool init(int port);
    // 开始监听
    void startListening();
    // 停止服务
    void stop();
    
    // 发送数据到指定客户端（模板化）
    bool sendTo(const DataType& message, const std::string& ip, int port);
    
    // 设置消息接收回调函数（模板化）
    void setMessageCallback(std::function<void(const DataType&, const std::string&, int)> callback);
    
    // 从缓存中获取接收到的消息
    bool getReceivedMessage(UDPMessage<DataType>& message);
    
    // 获取整个消息队列（与DroneData<T>配合使用）
    std::queue<DataType> getMessageQueue();
    
    // 获取缓存中消息数量
    size_t getMessageCount();
    
    // 清空消息缓存
    void clearMessageCache();

private:
    // 套接字文件描述符
    int sockfd;
    // 服务器地址结构
    struct sockaddr_in server_addr;
    // 运行状态标志
    bool running;
    // 服务器端口号
    int server_port;
    
    // 模板化消息接收回调函数
    std::function<void(const DataType&, const std::string&, int)> message_callback;
    
    // 模板化消息缓存队列
    std::queue<UDPMessage<DataType>> message_cache;
    // 缓存访问互斥锁
    std::mutex cache_mutex;
    
    // 接收循环
    void receiveLoop();
    
    // 数据转换辅助方法
    DataType convertRawData(const char* buffer, size_t length);
    std::vector<uint8_t> serializeData(const DataType& data);
};

// 显式实例化声明（支持常用类型）
template class UDP<std::string>;
template class UDP<nlohmann::json>;
template class UDP<std::vector<uint8_t>>;

#endif // UDP_H
