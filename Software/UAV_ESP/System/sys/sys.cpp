#include "sys.h"

// ============ 全局对象定义 ============
IIC iic = IIC(I2C_MODE_MASTER, GPIO_NUM_21, GPIO_NUM_22, GPIO_PULLUP_ENABLE, GPIO_PULLUP_ENABLE, 100000);
TIME time = TIME(arithmetic_time);

// ============ 系统初始化 ============
void sys_init(void)
{
    time.start();
}


// ============ 算法定时器 ============
void arithmetic_time(void)
{
    
}