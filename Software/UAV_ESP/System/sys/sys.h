#ifndef SYS_H
#define SYS_H
// ============ 头文件 ============
#include "driver/gpio.h"
#include "../../Hardware/IIC/IIC.h"
#include "../../Hardware/TIME/TIME.h"
#include "../../Hardware/MPU6050/MPU6050.h"
#include "sys_err.h"
// ============ 公共参数 ============
extern IIC iic;

extern MPU6050* mpu6050;  // 使用指针，避免全局对象初始化顺序问题
extern int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
extern float Temperature;
extern volatile uint32_t interrupt_count;  // 中断计数器
extern volatile bool mpu6050_data_ready;   // 数据就绪标志（ISR设置，任务读取）
// ============ 系统初始化 ============
int system_init(void);
void mpu6050_Interrupt_function(void* arg);

// ============ 宏定义 ============
// ============ 输出引脚 ============
#define Pout1(x)      gpio_set_level(1, x);
#define Pout2(x)      gpio_set_level(2, x);
#define Pout3(x)      gpio_set_level(3, x);
#define Pout4(x)      gpio_set_level(4, x);
#define Pout5(x)      gpio_set_level(5, x);
#define Pout6(x)      gpio_set_level(6, x);
#define Pout7(x)      gpio_set_level(7, x);
#define Pout8(x)      gpio_set_level(8, x);
#define Pout9(x)      gpio_set_level(9, x);
#define Pout10(x)     gpio_set_level(10, x);
#define Pout11(x)     gpio_set_level(11, x);
#define Pout12(x)     gpio_set_level(12, x);
#define Pout13(x)     gpio_set_level(13, x);
#define Pout14(x)     gpio_set_level(14, x);
#define Pout15(x)     gpio_set_level(15, x);
#define Pout16(x)     gpio_set_level(16, x);

// ============ 输入引脚 ============
#define Pin1()      gpio_get_level(1);
#define Pin2()      gpio_get_level(2);
#define Pin3()      gpio_get_level(3);
#define Pin4()      gpio_get_level(4);
#define Pin5()      gpio_get_level(5);
#define Pin6()      gpio_get_level(6);
#define Pin7()      gpio_get_level(7);
#define Pin8()      gpio_get_level(8);
#define Pin9()      gpio_get_level(9);
#define Pin10()     gpio_get_level(10);
#define Pin11()     gpio_get_level(11);
#define Pin12()     gpio_get_level(12);
#define Pin13()     gpio_get_level(13);
#define Pin14()     gpio_get_level(14);
#define Pin15()     gpio_get_level(15);
#define Pin16()     gpio_get_level(16);

#endif