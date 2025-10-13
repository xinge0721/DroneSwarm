#ifndef SWARM_REGISTRY_H
#define SWARM_REGISTRY_H


// 开机管理类并管理状态
class SwarmRegistry {
public:

    // 无人机信息结构
    SwarmRegistry();
    ~SwarmRegistry();

    // 运算符重载
    SwarmRegistry& operator[](int index);
    SwarmRegistry& operator[](int index) const;
    // 大小
    int size() const;
    // 判断是否为空
    bool empty() const;
    // 遍历缓存
    void ParseData(std::queue<T> &message_cache);
    // ID生成器
    uint8_t generateID();
    // 注册无人机
    void registerDrone(const std::string& ip, int port);
    // 注销无人机
    void unregisterDrone(const std::string& ip, int port);
    // 获取无人机信息
    DroneInfo getDroneInfo(const std::string& ip, int port);
    // 获取无人机信息
    DroneInfo getDroneInfo(uint8_t id);
};


#endif