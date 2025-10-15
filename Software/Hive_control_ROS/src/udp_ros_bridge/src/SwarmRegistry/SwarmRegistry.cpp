#include "SwarmRegistry.h"
#include <stdexcept>

// ==================辅助函数==================
// 生成endpoint键（IP:端口）
std::string SwarmRegistry::makeEndpointKey(const std::string& ip, int port) const
{
    return ip + ":" + std::to_string(port);
}

// 重建索引映射（在数组元素位置改变后调用）
void SwarmRegistry::rebuildIndexMaps()
{
    // 清空现有映射
    id_to_index.clear();
    endpoint_to_id.clear();
    
    // 重新建立映射
    for (int i = 0; i < count; i++)
    {
        const DroneInfo& info = drone_info_cache[i];
        id_to_index[info.id] = i;
        endpoint_to_id[makeEndpointKey(info.ip, info.port)] = info.id;
    }
}

//  ==================扩容函数==================
// 参数一：扩容倍数
// 参数二：默认扩容倍数为2
void SwarmRegistry::recapacity(int new_capacity )
{
    // 扩容
    this->capacity = (this->capacity == 0 ? 4 : this->capacity) * new_capacity;

    DroneInfo* new_drone_info_cache = new DroneInfo[capacity];
    for (int i = 0; i < count; i++)
    {
        new_drone_info_cache[i] = this->drone_info_cache[i];
    }
    // 释放旧的无人机信息缓存
    delete[] this->drone_info_cache;
    // 更新无人机信息缓存
    this->drone_info_cache = new_drone_info_cache;
    
    // 扩容后重建索引映射（数组地址已经改变）
    rebuildIndexMaps();
}


DroneInfo& SwarmRegistry::operator[](int index)
{
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("SwarmRegistry下标越界");
    }
    return this->drone_info_cache[index];
}

const DroneInfo& SwarmRegistry::operator[](int index) const
{
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("SwarmRegistry下标越界");
    }
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
    
    // 重建哈希表映射
    rebuildIndexMaps();
}

// ================== 赋值运算符 ==================
SwarmRegistry& SwarmRegistry::operator=(const SwarmRegistry& other)
{
    if (this == &other)
    {
        return *this;
    }
    
    // 清理现有资源
    if (this->drone_info_cache != nullptr)
    {
        delete[] this->drone_info_cache;
        this->drone_info_cache = nullptr;
    }
    id_to_index.clear();
    endpoint_to_id.clear();
    
    // 复制基本成员
    this->capacity = other.capacity;
    this->count = other.count;
    this->count_id = other.count_id;
    
    // 分配新数组并复制数据
    this->drone_info_cache = new DroneInfo[this->capacity];
    for (int i = 0; i < this->count; ++i)
    {
        this->drone_info_cache[i] = other.drone_info_cache[i];
    }
    
    // 重建哈希表映射
    rebuildIndexMaps();
    
    return *this;
}
// ================== 注册无人机 ==================
uint8_t SwarmRegistry::registerDrone(const std::string& ip, int port)
{
    // 输入验证：IP非空，端口号有效范围(1-65535)
    if (ip.empty() || port <= 0 || port > 65535) 
    {
        // 返回一个无效ID
        return ERROR_ID;
    }

    // O(1)检查是否已经注册（防止重复注册）
    std::string endpoint = makeEndpointKey(ip, port);
    auto it = endpoint_to_id.find(endpoint);
    if (it != endpoint_to_id.end()) 
    {
        return it->second;  // 返回现有ID
    }

    // 检查是否需要扩容
    if (count >= capacity)
    {
        recapacity();  // recapacity函数内部会重建索引映射
    }
    
    // 创建新的无人机信息
    uint8_t new_id = count_id++;
    drone_info_cache[count] = DroneInfo(ip, port, new_id);
    
    // 更新哈希表映射
    id_to_index[new_id] = count;
    endpoint_to_id[endpoint] = new_id;
    
    // 增加计数并返回新ID
    count++;
    return new_id;
}

// ================== 删除无人机信息 ==================
// 参数一：删除数据的ID
bool SwarmRegistry::removeDroneInfo(uint8_t id)
{
    // O(1)哈希表查找
    auto it = id_to_index.find(id);
    if (it == id_to_index.end()) 
    {
        return false;  // 没找到该ID
    }
    
    int index = it->second;
    DroneInfo& info = drone_info_cache[index];
    
    // 从endpoint映射中删除
    std::string endpoint = makeEndpointKey(info.ip, info.port);
    endpoint_to_id.erase(endpoint);
    
    // 挪动数据（从指定位置删除元素）
    for (int i = index; i < count - 1; i++)
    {
        drone_info_cache[i] = drone_info_cache[i + 1];
    }
    
    // 更新数量
    count--;
    
    // 重建索引映射（因为数组元素位置改变了）
    rebuildIndexMaps();
    
    return true;
}
// ================== 获取无人机信息 ==================
DroneInfo* SwarmRegistry::getDroneInfo(uint8_t id)
{
    // O(1)哈希表查找
    auto it = id_to_index.find(id);
    if (it != id_to_index.end()) 
    {
        return &drone_info_cache[it->second];
    }
    return nullptr;
}

const DroneInfo* SwarmRegistry::getDroneInfo(uint8_t id) const
{
    // O(1)哈希表查找（const版本）
    auto it = id_to_index.find(id);
    if (it != id_to_index.end()) 
    {
        return &drone_info_cache[it->second];
    }
    return nullptr;
}