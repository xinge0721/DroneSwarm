#ifndef SWARM_REGISTRY_H
#define SWARM_REGISTRY_H

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#define ERROR_ID 0xFF

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
    // 理论上最大只能到255，实际上无人机系统无法支撑50台以上的无人机
    // 所以哪怕无人机反复创建，也不会溢出的
    // 换句话说真来255个无人机，内存会先他一步崩溃
    int count_id = 0;
    // 数组长度
    int capacity = 1;
    // 目前无人机数量
    int count = 0;
    
    // ==================哈希表优化==================
    // ID到数组索引的映射（用于O(1)查找）
    std::unordered_map<uint8_t, int> id_to_index;
    // IP+端口到ID的映射（用于重复检查）
    std::unordered_map<std::string, uint8_t> endpoint_to_id;

    //  ==================扩容函数==================
    // 参数一：扩容倍数
    // 参数二：默认扩容倍数为2
    void recapacity(int new_capacity = 2);
    
    // ==================辅助函数==================
    // 生成endpoint键（IP:端口）
    std::string makeEndpointKey(const std::string& ip, int port) const;
    // 重建索引映射（在数组元素位置改变后调用）
    void rebuildIndexMaps();
public:
    // 无人机信息缓存
    DroneInfo* drone_info_cache;
    
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
    const DroneInfo& operator[](int index) const;

    // ===================迭代器===================
    DroneInfo* begin()
    {
        return drone_info_cache;
    }
    DroneInfo* end()
    {
        return drone_info_cache + count;
    }
    const DroneInfo* begin() const
    {
        return drone_info_cache;
    }
    const DroneInfo* end() const
    {
        return drone_info_cache + count;
    }
    // ================== 注册无人机 ==================
    uint8_t registerDrone(const std::string& ip, int port);

    // ================== 删除无人机信息 ==================
    // 参数一：删除数据的ID
    bool removeDroneInfo(uint8_t id);
    // ================== 获取无人机信息 ==================
    DroneInfo* getDroneInfo(uint8_t id);
    const DroneInfo* getDroneInfo(uint8_t id) const;

    // ================== 获取无人机数量 ==================
    int getDroneCount() const
    {return count;} 
    // ================== 获取无人机信息缓存长度 ==================
    int getCapacity() const
    {return capacity;} 
    //  ==================判断是否空 ==================
    bool isEmpty() const
    {return count == 0;} 



};


#endif