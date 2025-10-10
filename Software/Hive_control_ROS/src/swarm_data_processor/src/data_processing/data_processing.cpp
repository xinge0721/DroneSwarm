#include "data_processing.h"

void DataProcessing::ParseData(const std::string& data)
{
    // 字符串解析
    // 将字符串按逗号分割
    std::stringstream ss(data);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        // 将字符串按等号分割
    }


}
// JSON
void DataProcessing::ParseData(const nlohmann::json& data)
{
    // JSON解析
}
// 二进制
void DataProcessing::ParseData(const uint8_t* data, size_t size)
{
    
}