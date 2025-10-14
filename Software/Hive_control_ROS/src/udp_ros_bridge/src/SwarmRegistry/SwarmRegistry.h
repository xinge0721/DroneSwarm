#ifndef SWARM_REGISTRY_H
#define SWARM_REGISTRY_H

#include <string>
#include <vector>
#include <cstdint>




// 开机管理类并管理状态
class SwarmRegistry {
private:
    struct DroneInfo
    {
        // 无人机IP地址
        std::string ip;
        // 无人机端口号
        int port;
        // 无人机ID
        uint8_t id;
        DroneInfo(const std::string& ip = "", int port = 0, uint8_t id = 0) : ip(ip), port(port), id(id) {}
    };
    // 无人机生成id
    int count_id = 0;
    // 数组长度
    int capacity = 1;
    // 目前无人机数量
    int count = 0;
    // 无人机信息缓存
    DroneInfo* drone_info_cache;
    //  ==================扩容函数==================
    // 参数一：扩容倍数
    // 参数二：默认扩容倍数为2
    void recapacity(int new_capacity = 2);
public:
    // ================== 构造函数 ==================
    SwarmRegistry()
    {
        drone_info_cache = new DroneInfo[capacity];
    }
    // ================== 析构函数 ==================
    ~SwarmRegistry()
    {
        // 清空无人机信息缓存
        delete[] drone_info_cache;
        drone_info_cache = nullptr;
        capacity = 0;
        count = 0;
        count_id = 0;
    }
    // ================== 拷贝构造函数 ==================
    SwarmRegistry(const SwarmRegistry& other);
    // ================== 赋值运算符 ==================
    SwarmRegistry& operator=(const SwarmRegistry& other);
    //  ================== 运算符重载 ==================
    DroneInfo& operator[](int index);
    DroneInfo& operator[](int index) const;

    // ================== 注册无人机 ==================
    uint8_t registerDrone(const std::string& ip, int port);

    // ================== 删除无人机信息 ==================
    // 参数一：删除数据的IDs
    void removeDroneInfo(uint8_t id);
    // ================== 获取无人机信息 ==================
    DroneInfo* getDroneInfo(uint8_t id);


    // ================== 获取无人机数量 ==================
    int getDroneCount(){return count;}
    // ================== 获取无人机信息缓存 ==================
    DroneInfo* getDroneInfoCache(){return drone_info_cache;}
    // ================== 获取无人机信息缓存长度 ==================
    int getDroneInfoCacheLength(){return capacity;}
    //  ==================判断是否空 ==================
    bool isEmpty(){return count == 0;}

};


#endif