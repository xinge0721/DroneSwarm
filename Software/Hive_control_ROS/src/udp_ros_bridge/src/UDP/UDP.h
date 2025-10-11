#ifndef UDP_H
#define UDP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <functional>
#include <queue>
#include <mutex>

// UDP消息结构
struct UDPMessage {
    std::string data;
    std::string client_ip;
    int client_port;
};

class UDP {
public:
    UDP(int port = 8888);
    ~UDP();
    
    // 开始监听
    void startListening();
    // 停止服务
    void stop();
    
    // 发送数据到指定客户端
    bool sendTo(const std::string& message, const std::string& ip, int port);
    
    // 设置消息接收回调函数（可选，如果不设置则数据存入缓存）
    void setMessageCallback(std::function<void(const std::string&, const std::string&, int)> callback);
    
    // 从缓存中获取接收到的消息
    bool getReceivedMessage(UDPMessage& message);
    
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
    
    // 消息接收回调函数
    std::function<void(const std::string&, const std::string&, int)> message_callback;
    
    // 消息缓存队列
    std::queue<UDPMessage> message_cache;
    // 缓存访问互斥锁
    std::mutex cache_mutex;
    
    // 接收循环
    void receiveLoop();
};

#endif // UDP_H
