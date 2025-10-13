#ifndef UDP_H
#define UDP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <queue>
#include <mutex>
#include <vector>

/**
 * @brief UDP通信类
 * @note 简化版本，只处理原始字节数据，线程安全
 */
class UDP {
public:
    /**
     * @brief 构造函数
     * @param port 监听端口号，默认8888
     */
    UDP(int port = 8888);
    
    /**
     * @brief 析构函数，自动停止服务并清理资源
     */
    ~UDP();
    
    /**
     * @brief 开始监听UDP端口
     * @note 在新线程中运行接收循环
     */
    void startListening();
    
    /**
     * @brief 停止UDP服务
     * @note 关闭socket并停止接收线程
     */
    void stop();
    
    /**
     * @brief 发送数据到指定客户端
     * @param data 要发送的原始字节数据
     * @param ip 目标IP地址
     * @param port 目标端口号
     * @return 发送成功返回true，失败返回false
     */
    bool sendTo(const std::vector<uint8_t>& data, const std::string& ip, int port);
    
    /**
     * @brief 获取接收到的消息队列
     * @return 包含所有未处理消息的队列，获取后队列会被清空
     * @note 线程安全，使用swap操作避免拷贝
     */
    std::queue<std::vector<uint8_t>> getMessageQueue();
    
    /**
     * @brief 获取缓存中消息数量
     * @return 当前队列中的消息数量
     * @note 线程安全
     */
    size_t getMessageCount();

private:
    // 套接字文件描述符
    int sockfd;
    // 服务器地址结构
    struct sockaddr_in server_addr;
    // 运行状态标志
    bool running;
    // 服务器端口号
    int server_port;
    
    // 消息缓存队列
    std::queue<std::vector<uint8_t>> message_queue;
    // 队列访问互斥锁
    std::mutex queue_mutex;
    
    /**
     * @brief 接收循环（在独立线程中运行）
     * @note 持续接收UDP数据并存入队列
     */
    void receiveLoop();
};

#endif // UDP_H
