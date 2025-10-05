#ifndef KALMAN_H
#define KALMAN_H

#include <Arduino.h>

/**
 * @brief 6维卡尔曼滤波器类
 * 状态向量: [x, y, z, yaw, pitch, roll]
 * 用于无人机位置和姿态估计
 */
class KalmanFilter {
private:
    static const int STATE_SIZE = 6;  // 状态向量维度
    static const int MEAS_SIZE = 6;   // 测量向量维度
    
    // 状态向量: [x, y, z, yaw, pitch, roll]
    float state[STATE_SIZE];
    
    // 状态协方差矩阵 P (6x6)
    float P[STATE_SIZE][STATE_SIZE];
    
    // 状态转移矩阵 F (6x6)
    float F[STATE_SIZE][STATE_SIZE];
    
    // 过程噪声协方差矩阵 Q (6x6)
    float Q[STATE_SIZE][STATE_SIZE];
    
    // 观测矩阵 H (6x6)
    float H[MEAS_SIZE][STATE_SIZE];
    
    // 测量噪声协方差矩阵 R (6x6)
    float R[MEAS_SIZE][MEAS_SIZE];
    
    // 临时矩阵用于计算
    float temp1[STATE_SIZE][STATE_SIZE];
    float temp2[STATE_SIZE][STATE_SIZE];
    float temp3[MEAS_SIZE][MEAS_SIZE];
    float temp4[STATE_SIZE][MEAS_SIZE];
    
    // 矩阵运算函数
    void matrixMultiply(const float A[][STATE_SIZE], const float B[][STATE_SIZE], 
                       float result[][STATE_SIZE], int rows, int cols, int inner);
    void matrixAdd(const float A[][STATE_SIZE], const float B[][STATE_SIZE], 
                   float result[][STATE_SIZE], int rows, int cols);
    void matrixSubtract(const float A[][STATE_SIZE], const float B[][STATE_SIZE], 
                       float result[][STATE_SIZE], int rows, int cols);
    void matrixTranspose(const float A[][STATE_SIZE], float result[][STATE_SIZE], 
                        int rows, int cols);
    bool matrixInverse(const float A[][MEAS_SIZE], float result[][MEAS_SIZE], int size);
    void matrixCopy(const float src[][STATE_SIZE], float dst[][STATE_SIZE], 
                   int rows, int cols);
    
    // 内部初始化函数
    void initializeFilter(const float initial_state[STATE_SIZE], 
                         const float process_noise[STATE_SIZE],
                         const float measurement_noise[MEAS_SIZE]);

public:
    /**
     * @brief 构造函数，初始化卡尔曼滤波器
     * @param initial_state 初始状态向量 [x, y, z, yaw, pitch, roll] (可选)
     * @param process_noise 过程噪声标准差数组 (可选)
     * @param measurement_noise 测量噪声标准差数组 (可选)
     */
    KalmanFilter(const float initial_state[STATE_SIZE] = nullptr, 
                 const float process_noise[STATE_SIZE] = nullptr,
                 const float measurement_noise[MEAS_SIZE] = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~KalmanFilter();
    
    /**
     * @brief 重新初始化滤波器参数（如果需要运行时修改参数）
     * @param initial_state 初始状态向量 [x, y, z, yaw, pitch, roll]
     * @param process_noise 过程噪声标准差数组
     * @param measurement_noise 测量噪声标准差数组
     */
    void reinit(const float initial_state[STATE_SIZE], 
                const float process_noise[STATE_SIZE],
                const float measurement_noise[MEAS_SIZE]);
    
    /**
     * @brief 预测步骤
     * @param dt 时间步长 (秒)
     */
    void predict(float dt);
    
    /**
     * @brief 更新步骤
     * @param measurement 测量值数组 [x, y, z, yaw, pitch, roll]
     */
    void update(const float measurement[MEAS_SIZE]);
    
    /**
     * @brief 获取当前状态估计
     * @param output_state 输出状态数组
     */
    void getState(float output_state[STATE_SIZE]);
    
    /**
     * @brief 获取位置估计 (x, y, z)
     * @param x 输出x坐标
     * @param y 输出y坐标  
     * @param z 输出z坐标
     */
    void getPosition(float &x, float &y, float &z);
    
    /**
     * @brief 获取姿态估计 (yaw, pitch, roll)
     * @param yaw 输出偏航角 (度)
     * @param pitch 输出俯仰角 (度)
     * @param roll 输出翻滚角 (度)
     */
    void getAttitude(float &yaw, float &pitch, float &roll);
    
    /**
     * @brief 设置过程噪声协方差
     * @param noise_std 噪声标准差数组
     */
    void setProcessNoise(const float noise_std[STATE_SIZE]);
    
    /**
     * @brief 设置测量噪声协方差
     * @param noise_std 噪声标准差数组
     */
    void setMeasurementNoise(const float noise_std[MEAS_SIZE]);
    
    /**
     * @brief 重置滤波器状态
     * @param new_state 新的状态向量
     */
    void reset(const float new_state[STATE_SIZE]);
    
    /**
     * @brief 获取状态协方差矩阵的迹（对角线元素之和）
     * @return 协方差矩阵的迹
     */
    float getCovarianceTrace();
};

#endif // KALMAN_H
