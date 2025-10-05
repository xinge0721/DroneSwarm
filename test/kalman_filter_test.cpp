/**
 * @file kalman_filter_test.cpp
 * @brief 卡尔曼滤波器测试和使用示例
 * 
 * 此文件展示如何在无人机系统中使用卡尔曼滤波器
 * 用于融合传感器数据，估计位置和姿态
 */

#include "../Software/UAV_ESP/System/Kalman/Kalman.h"
#include <Arduino.h>

/**
 * @brief 测试基本的卡尔曼滤波器功能
 */
void testBasicKalmanFilter() {
    Serial.println("=== 基本卡尔曼滤波器测试 ===");
    
    // 使用默认参数创建滤波器 - 现在初始化在构造函数中完成！
    KalmanFilter kalman;
    
    // 模拟传感器数据测试
    float test_measurements[6] = {1.0f, 2.0f, 5.0f, 45.0f, 10.0f, -5.0f};
    
    Serial.println("测试数据输入: [1.0, 2.0, 5.0, 45.0, 10.0, -5.0]");
    
    // 进行10次预测和更新循环
    for (int i = 0; i < 10; i++) {
        kalman.predict(0.02f); // 50Hz
        kalman.update(test_measurements);
        
        float x, y, z, yaw, pitch, roll;
        kalman.getPosition(x, y, z);
        kalman.getAttitude(yaw, pitch, roll);
        
        Serial.printf("第%d次: 位置(%.2f,%.2f,%.2f) 姿态(%.1f,%.1f,%.1f)\n", 
                     i+1, x, y, z, yaw, pitch, roll);
    }
    
    Serial.println("基本测试完成\n");
}

/**
 * @brief 测试自定义参数的卡尔曼滤波器
 */
void testCustomKalmanFilter() {
    Serial.println("=== 自定义参数卡尔曼滤波器测试 ===");
    
    // 自定义初始状态
    float initial_state[6] = {10.0f, 20.0f, 30.0f, 90.0f, 15.0f, -10.0f};
    
    // 低噪声参数 (高精度传感器)
    float process_noise[6] = {0.01f, 0.01f, 0.01f, 0.1f, 0.1f, 0.1f};
    float measurement_noise[6] = {0.05f, 0.05f, 0.02f, 0.5f, 0.2f, 0.2f};
    
    // 使用自定义参数创建滤波器
    KalmanFilter kalman(initial_state, process_noise, measurement_noise);
    
    Serial.println("初始状态: [10.0, 20.0, 30.0, 90.0, 15.0, -10.0]");
    
    // 模拟有噪声的测量数据
    float measurements[6];
    for (int i = 0; i < 5; i++) {
        // 在真实值附近添加随机噪声
        measurements[0] = initial_state[0] + random(-10, 10) / 100.0f;
        measurements[1] = initial_state[1] + random(-10, 10) / 100.0f;
        measurements[2] = initial_state[2] + random(-5, 5) / 100.0f;
        measurements[3] = initial_state[3] + random(-50, 50) / 100.0f;
        measurements[4] = initial_state[4] + random(-20, 20) / 100.0f;
        measurements[5] = initial_state[5] + random(-20, 20) / 100.0f;
        
        kalman.predict(0.02f);
        kalman.update(measurements);
        
        float x, y, z, yaw, pitch, roll;
        kalman.getPosition(x, y, z);
        kalman.getAttitude(yaw, pitch, roll);
        
        Serial.printf("测量值: [%.2f,%.2f,%.2f,%.1f,%.1f,%.1f]\n", 
                     measurements[0], measurements[1], measurements[2], 
                     measurements[3], measurements[4], measurements[5]);
        Serial.printf("估计值: [%.2f,%.2f,%.2f,%.1f,%.1f,%.1f]\n", 
                     x, y, z, yaw, pitch, roll);
        Serial.printf("不确定性: %.3f\n\n", kalman.getCovarianceTrace());
    }
    
    Serial.println("自定义参数测试完成\n");
}

/**
 * @brief 测试滤波器重置功能
 */
void testKalmanReset() {
    Serial.println("=== 卡尔曼滤波器重置测试 ===");
    
    KalmanFilter kalman;
    
    // 进行一些更新让状态偏离初始值
    float measurements[6] = {5.0f, 5.0f, 10.0f, 180.0f, 45.0f, 30.0f};
    for (int i = 0; i < 5; i++) {
        kalman.predict(0.02f);
        kalman.update(measurements);
    }
    
    float x, y, z, yaw, pitch, roll;
    kalman.getPosition(x, y, z);
    kalman.getAttitude(yaw, pitch, roll);
    Serial.printf("重置前: [%.2f,%.2f,%.2f,%.1f,%.1f,%.1f]\n", x, y, z, yaw, pitch, roll);
    
    // 重置滤波器
    float reset_state[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    kalman.reset(reset_state);
    
    kalman.getPosition(x, y, z);
    kalman.getAttitude(yaw, pitch, roll);
    Serial.printf("重置后: [%.2f,%.2f,%.2f,%.1f,%.1f,%.1f]\n", x, y, z, yaw, pitch, roll);
    
    Serial.println("重置测试完成\n");
}

/**
 * @brief 测试动态参数调整
 */
void testDynamicNoiseAdjustment() {
    Serial.println("=== 动态噪声参数调整测试 ===");
    
    KalmanFilter kalman;
    
    // 测试不同的噪声设置
    float high_noise[6] = {1.0f, 1.0f, 1.0f, 5.0f, 5.0f, 5.0f};
    float low_noise[6] = {0.01f, 0.01f, 0.01f, 0.1f, 0.1f, 0.1f};
    
    Serial.println("设置高噪声参数...");
    kalman.setProcessNoise(high_noise);
    kalman.setMeasurementNoise(high_noise);
    
    float measurements[6] = {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    
    // 高噪声下的几次更新
    for (int i = 0; i < 3; i++) {
        kalman.predict(0.02f);
        kalman.update(measurements);
        Serial.printf("高噪声第%d次，不确定性: %.3f\n", i+1, kalman.getCovarianceTrace());
    }
    
    Serial.println("设置低噪声参数...");
    kalman.setProcessNoise(low_noise);
    kalman.setMeasurementNoise(low_noise);
    
    // 低噪声下的几次更新
    for (int i = 0; i < 3; i++) {
        kalman.predict(0.02f);
        kalman.update(measurements);
        Serial.printf("低噪声第%d次，不确定性: %.3f\n", i+1, kalman.getCovarianceTrace());
    }
    
    Serial.println("动态调整测试完成\n");
}

/**
 * @brief 测试角度归一化功能
 */
void testAngleNormalization() {
    Serial.println("=== 角度归一化测试 ===");
    
    KalmanFilter kalman;
    
    // 测试边界角度值
    float test_angles[][6] = {
        {0.0f, 0.0f, 0.0f, 350.0f, 0.0f, 0.0f},   // 350° 应该归一化为 -10°
        {0.0f, 0.0f, 0.0f, -350.0f, 0.0f, 0.0f},  // -350° 应该归一化为 10°
        {0.0f, 0.0f, 0.0f, 0.0f, 190.0f, 0.0f},   // 190° 应该归一化为 -170°
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -190.0f}   // -190° 应该归一化为 170°
    };
    
    for (int i = 0; i < 4; i++) {
        kalman.reset(test_angles[i]);  // 重置到测试角度
        kalman.predict(0.02f);
        kalman.update(test_angles[i]);
        
        float x, y, z, yaw, pitch, roll;
        kalman.getPosition(x, y, z);
        kalman.getAttitude(yaw, pitch, roll);
        
        Serial.printf("输入角度: [%.1f, %.1f, %.1f]\n", 
                     test_angles[i][3], test_angles[i][4], test_angles[i][5]);
        Serial.printf("归一化后: [%.1f, %.1f, %.1f]\n\n", yaw, pitch, roll);
    }
    
    Serial.println("角度归一化测试完成\n");
}

/**
 * @brief 运行所有测试
 */
void runAllKalmanTests() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("开始卡尔曼滤波器综合测试...\n");
    
    testBasicKalmanFilter();
    testCustomKalmanFilter();
    testKalmanReset();
    testDynamicNoiseAdjustment();  
    testAngleNormalization();
    
    Serial.println("=== 所有测试完成！===");
    Serial.println("卡尔曼滤波器工作正常，可以集成到你的无人机系统中。");
}

/**
 * @brief 实际使用示例 - 简化版
 */
void simpleUsageExample() {
    Serial.println("=== 简化使用示例 ===");
    
    // 1. 创建滤波器 (使用默认参数，构造函数自动初始化)
    KalmanFilter kalman;
    
    // 2. 或者使用自定义参数
    // float initial_state[6] = {0,0,0,0,0,0};
    // float process_noise[6] = {0.1f,0.1f,0.1f,0.5f,0.5f,0.5f};
    // float meas_noise[6] = {0.3f,0.3f,0.2f,2.0f,1.0f,1.0f};
    // KalmanFilter kalman(initial_state, process_noise, meas_noise);
    
    // 3. 在主循环中使用
    for (int i = 0; i < 5; i++) {
        // 从传感器获取数据 (这里是模拟数据)
        float sensor_data[6] = {
            random(-100, 100) / 100.0f,  // GPS X
            random(-100, 100) / 100.0f,  // GPS Y  
            random(400, 600) / 100.0f,   // 高度
            random(-180, 180),           // 偏航角
            random(-30, 30),             // 俯仰角
            random(-30, 30)              // 翻滚角
        };
        
        // 预测和更新
        kalman.predict(0.02f);  // 50Hz
        kalman.update(sensor_data);
        
        // 获取滤波结果
        float x, y, z, yaw, pitch, roll;
        kalman.getPosition(x, y, z);
        kalman.getAttitude(yaw, pitch, roll);
        
        Serial.printf("滤波结果: 位置[%.2f,%.2f,%.2f] 姿态[%.1f,%.1f,%.1f]\n",
                     x, y, z, yaw, pitch, roll);
    }
    
    Serial.println("简化示例完成\n");
}

/* 
使用说明:
1. 现在创建卡尔曼滤波器非常简单：
   KalmanFilter kalman;  // 使用默认参数，自动初始化
   
2. 或者使用自定义参数：
   KalmanFilter kalman(initial_state, process_noise, measurement_noise);
   
3. 在主循环中调用:
   kalman.predict(dt);
   kalman.update(measurements);
   
4. 运行此测试: runAllKalmanTests()
*/
