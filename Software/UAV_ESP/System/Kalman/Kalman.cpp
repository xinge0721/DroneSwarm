#include "Kalman.h"
#include <math.h>
// 构造函数
// 参数一 ：初始状态
// 参数二 ：过程噪声
// 参数三 ：测量噪声
KalmanFilter::KalmanFilter(const float initial_state[STATE_SIZE], 
                           const float process_noise[STATE_SIZE],
                           const float measurement_noise[MEAS_SIZE]) {
    // 默认初始状态
    float default_state[STATE_SIZE] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // 默认过程噪声 (适中的值)
    float default_process[STATE_SIZE] = {0.1f, 0.1f, 0.1f, 0.5f, 0.5f, 0.5f};
    
    // 默认测量噪声 (根据典型传感器精度)
    float default_measurement[MEAS_SIZE] = {0.3f, 0.3f, 0.2f, 2.0f, 1.0f, 1.0f};
    
    // 使用提供的参数或默认值
    const float* use_state = initial_state ? initial_state : default_state;
    const float* use_process = process_noise ? process_noise : default_process;
    const float* use_measurement = measurement_noise ? measurement_noise : default_measurement;
    
    // 执行初始化
    initializeFilter(use_state, use_process, use_measurement);
}

KalmanFilter::~KalmanFilter() {
    // 析构函数，ESP32不需要特别的清理
}

void KalmanFilter::reinit(const float initial_state[STATE_SIZE], 
                          const float process_noise[STATE_SIZE],
                          const float measurement_noise[MEAS_SIZE]) {
    initializeFilter(initial_state, process_noise, measurement_noise);
}

void KalmanFilter::initializeFilter(const float initial_state[STATE_SIZE], 
                                   const float process_noise[STATE_SIZE],
                                   const float measurement_noise[MEAS_SIZE]) {
    
    // 初始化状态向量
    for (int i = 0; i < STATE_SIZE; i++) {
        state[i] = initial_state[i];
    }
    
    // 初始化状态转移矩阵 F (单位矩阵，假设静态模型)
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            F[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // 初始化观测矩阵 H (单位矩阵，直接观测所有状态)
    for (int i = 0; i < MEAS_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            H[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // 初始化状态协方差矩阵 P (对角矩阵)
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            P[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // 设置噪声协方差矩阵
    setProcessNoise(process_noise);
    setMeasurementNoise(measurement_noise);
}

void KalmanFilter::predict(float dt) {
    // 状态预测: x_k = F * x_{k-1}
    float predicted_state[STATE_SIZE];
    for (int i = 0; i < STATE_SIZE; i++) {
        predicted_state[i] = 0.0f;
        for (int j = 0; j < STATE_SIZE; j++) {
            predicted_state[i] += F[i][j] * state[j];
        }
    }
    
    // 更新状态
    for (int i = 0; i < STATE_SIZE; i++) {
        state[i] = predicted_state[i];
    }
    
    // 协方差预测: P_k = F * P_{k-1} * F^T + Q
    
    // 计算 F * P
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            temp1[i][j] = 0.0f;
            for (int k = 0; k < STATE_SIZE; k++) {
                temp1[i][j] += F[i][k] * P[k][j];
            }
        }
    }
    
    // 计算 F * P * F^T
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            temp2[i][j] = 0.0f;
            for (int k = 0; k < STATE_SIZE; k++) {
                temp2[i][j] += temp1[i][k] * F[j][k]; // F^T[k][j] = F[j][k]
            }
        }
    }
    
    // P = F * P * F^T + Q
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            P[i][j] = temp2[i][j] + Q[i][j];
        }
    }
}

void KalmanFilter::update(const float measurement[MEAS_SIZE]) {
    // 计算创新(innovation): y = z - H * x
    float innovation[MEAS_SIZE];
    for (int i = 0; i < MEAS_SIZE; i++) {
        innovation[i] = measurement[i];
        for (int j = 0; j < STATE_SIZE; j++) {
            innovation[i] -= H[i][j] * state[j];
        }
        
        // 角度归一化 (对于yaw, pitch, roll)
        if (i >= 3) {
            while (innovation[i] > 180.0f) innovation[i] -= 360.0f;
            while (innovation[i] < -180.0f) innovation[i] += 360.0f;
        }
    }
    
    // 计算创新协方差: S = H * P * H^T + R
    
    // 计算 H * P
    for (int i = 0; i < MEAS_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            temp4[j][i] = 0.0f; // 复用temp4存储P*H^T
            for (int k = 0; k < STATE_SIZE; k++) {
                temp4[j][i] += P[j][k] * H[i][k]; // H^T[k][i] = H[i][k]
            }
        }
    }
    
    // 计算 S = H * P * H^T + R
    for (int i = 0; i < MEAS_SIZE; i++) {
        for (int j = 0; j < MEAS_SIZE; j++) {
            temp3[i][j] = R[i][j];
            for (int k = 0; k < STATE_SIZE; k++) {
                temp3[i][j] += H[i][k] * temp4[k][j];
            }
        }
    }
    
    // 计算卡尔曼增益: K = P * H^T * S^{-1}
    float S_inv[MEAS_SIZE][MEAS_SIZE];
    if (!matrixInverse(temp3, S_inv, MEAS_SIZE)) {
        // 矩阵求逆失败，跳过更新
        return;
    }
    
    // K = P * H^T * S^{-1}
    float K[STATE_SIZE][MEAS_SIZE];
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < MEAS_SIZE; j++) {
            K[i][j] = 0.0f;
            for (int k = 0; k < MEAS_SIZE; k++) {
                K[i][j] += temp4[i][k] * S_inv[k][j];
            }
        }
    }
    
    // 状态更新: x = x + K * y
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < MEAS_SIZE; j++) {
            state[i] += K[i][j] * innovation[j];
        }
        
        // 角度归一化 (对于yaw, pitch, roll)
        if (i >= 3) {
            while (state[i] > 180.0f) state[i] -= 360.0f;
            while (state[i] < -180.0f) state[i] += 360.0f;
        }
    }
    
    // 协方差更新: P = (I - K * H) * P
    
    // 计算 K * H
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            temp1[i][j] = (i == j) ? 1.0f : 0.0f; // 单位矩阵
            for (int k = 0; k < MEAS_SIZE; k++) {
                temp1[i][j] -= K[i][k] * H[k][j];
            }
        }
    }
    
    // P = (I - K * H) * P
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            temp2[i][j] = 0.0f;
            for (int k = 0; k < STATE_SIZE; k++) {
                temp2[i][j] += temp1[i][k] * P[k][j];
            }
        }
    }
    
    // 复制结果回P
    matrixCopy((const float(*)[STATE_SIZE])temp2, P, STATE_SIZE, STATE_SIZE);
}

void KalmanFilter::getState(float output_state[STATE_SIZE]) {
    for (int i = 0; i < STATE_SIZE; i++) {
        output_state[i] = state[i];
    }
}

void KalmanFilter::getPosition(float &x, float &y, float &z) {
    x = state[0];
    y = state[1];
    z = state[2];
}

void KalmanFilter::getAttitude(float &yaw, float &pitch, float &roll) {
    yaw = state[3];
    pitch = state[4];
    roll = state[5];
}

void KalmanFilter::setProcessNoise(const float noise_std[STATE_SIZE]) {
    // 设置过程噪声协方差矩阵 Q (对角矩阵)
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            Q[i][j] = (i == j) ? (noise_std[i] * noise_std[i]) : 0.0f;
        }
    }
}

void KalmanFilter::setMeasurementNoise(const float noise_std[MEAS_SIZE]) {
    // 设置测量噪声协方差矩阵 R (对角矩阵)
    for (int i = 0; i < MEAS_SIZE; i++) {
        for (int j = 0; j < MEAS_SIZE; j++) {
            R[i][j] = (i == j) ? (noise_std[i] * noise_std[i]) : 0.0f;
        }
    }
}

void KalmanFilter::reset(const float new_state[STATE_SIZE]) {
    for (int i = 0; i < STATE_SIZE; i++) {
        state[i] = new_state[i];
    }
    
    // 重置协方差矩阵为单位矩阵
    for (int i = 0; i < STATE_SIZE; i++) {
        for (int j = 0; j < STATE_SIZE; j++) {
            P[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

float KalmanFilter::getCovarianceTrace() {
    float trace = 0.0f;
    for (int i = 0; i < STATE_SIZE; i++) {
        trace += P[i][i];
    }
    return trace;
}

// 辅助矩阵运算函数

void KalmanFilter::matrixCopy(const float src[][STATE_SIZE], float dst[][STATE_SIZE], 
                             int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

// 简化的矩阵求逆函数 (仅适用于小矩阵)
bool KalmanFilter::matrixInverse(const float A[][MEAS_SIZE], float result[][MEAS_SIZE], int size) {
    // 使用高斯-约旦消元法求逆
    float temp[MEAS_SIZE][MEAS_SIZE * 2];
    
    // 创建增广矩阵 [A|I]
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            temp[i][j] = A[i][j];
            temp[i][j + size] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // 高斯消元
    for (int i = 0; i < size; i++) {
        // 寻找主元
        int maxRow = i;
        for (int k = i + 1; k < size; k++) {
            if (fabs(temp[k][i]) > fabs(temp[maxRow][i])) {
                maxRow = k;
            }
        }
        
        // 交换行
        if (maxRow != i) {
            for (int j = 0; j < size * 2; j++) {
                float swap = temp[i][j];
                temp[i][j] = temp[maxRow][j];
                temp[maxRow][j] = swap;
            }
        }
        
        // 检查是否可逆
        if (fabs(temp[i][i]) < 1e-10f) {
            return false;
        }
        
        // 归一化当前行
        float pivot = temp[i][i];
        for (int j = 0; j < size * 2; j++) {
            temp[i][j] /= pivot;
        }
        
        // 消元
        for (int k = 0; k < size; k++) {
            if (k != i) {
                float factor = temp[k][i];
                for (int j = 0; j < size * 2; j++) {
                    temp[k][j] -= factor * temp[i][j];
                }
            }
        }
    }
    
    // 提取逆矩阵
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result[i][j] = temp[i][j + size];
        }
    }
    
    return true;
}
