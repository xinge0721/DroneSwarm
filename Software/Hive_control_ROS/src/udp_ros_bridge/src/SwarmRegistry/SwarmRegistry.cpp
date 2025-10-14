#include "SwarmRegistry.h"


//  ==================扩容函数==================
// 参数一：扩容倍数
// 参数二：默认扩容倍数为2
void SwarmRegistry::recapacity(int new_capacity )
{
    // 扩容
    this->capacity *= new_capacity;

    DroneInfo* new_drone_info_cache = new DroneInfo[capacity];
    for (int i = 0; i < count; i++)
    {
        new_drone_info_cache[i] = this->drone_info_cache[i];
    }
    // 释放旧的无人机信息缓存
    delete[] this->drone_info_cache;
    // 更新无人机信息缓存
    this->drone_info_cache = new_drone_info_cache;
    // 更新数组长度
}


DroneInfo& SwarmRegistry::operator[](int index)
{
    return this->drone_info_cache[index];
}

DroneInfo& SwarmRegistry::operator[](int index) const
{
    return this->drone_info_cache[index];
}

// ================== 拷贝构造函数 ==================
SwarmRegistry::SwarmRegistry(const SwarmRegistry& other) : capacity(other.capacity), count(other.count), count_id(other.count_id)
{
    // 分配新的缓存数组并拷贝内容
    this->drone_info_cache = new DroneInfo[this->capacity];
    for (int i = 0; i < this->count; ++i)
    {
        this->drone_info_cache[i] = other.drone_info_cache[i];
    }
}

// ================== 赋值运算符 ==================
SwarmRegistry& SwarmRegistry::operator=(const SwarmRegistry& other)
{
    if (this == &other)
    {
        return *this;
    }
    if (this->drone_info_cache != nullptr)
    {
        delete[] this->drone_info_cache;
        this->drone_info_cache = nullptr;
    }
    this->capacity = other.capacity;
    this->count = other.count;
    this->count_id = other.count_id;
    this->drone_info_cache = new DroneInfo[this->capacity];
    for (int i = 0; i < this->count; ++i)
    {
        this->drone_info_cache[i] = other.drone_info_cache[i];
    }
    return *this;
}
// ================== 注册无人机 ==================
uint8_t SwarmRegistry::registerDrone(const std::string& ip, int port)
{
    // 输入验证：IP非空，端口号有效范围(1-65535)
    if (ip.empty() || port <= 0 || port > 65535) 
    {
        // 返回一个无效ID
        return 0xFF;
    }

    // 检查是否已经注册（防止重复注册）
    for (int i = 0; i < count; i++)
    {
        if (drone_info_cache[i].ip == ip && drone_info_cache[i].port == port)
        {
            return drone_info_cache[i].id;  // 返回现有ID
        }
    }

    if (count >= capacity)
    {
        recapacity();
    }
    drone_info_cache[count].ip = ip;
    drone_info_cache[count].port = port;
    drone_info_cache[count].id = count_id++;
    // 返回无人机新注册的ID
    return drone_info_cache[count++].id;
}

// ================== 删除无人机信息 ==================
// 参数一：删除数据的索引
void SwarmRegistry::removeDroneInfo(uint8_t id)
{

    int index = -1;  // 初始化为-1表示未找到
    for (int i = 0; i < count; i++)
    {
        if (drone_info_cache[i].id == id)
        {
            index = i;
            break;
        }
    }
    
    // 如果没找到，直接返回
    if (index == -1)
    {
        return;
    }
    
    // 挪动数据
    for (int i = index; i < count - 1; i++)
    {
        drone_info_cache[i] = drone_info_cache[i + 1];
    }
    // 更新数量
    count--;
}
// ================== 获取无人机信息 ==================
DroneInfo* SwarmRegistry::getDroneInfo(uint8_t id)
{

    for (int i = 0; i < count; i++)
    {
        if (drone_info_cache[i].id == id)
        {
            return &drone_info_cache[i];
        }
    }
    return nullptr;
}