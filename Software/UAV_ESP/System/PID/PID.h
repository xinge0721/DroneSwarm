#ifndef PID_H
#define PID_H

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "freertos/portmacro.h"
// PID控制器
class PID
{
private:
    float _kp = 0;       // 比例系数
    float _ki = 0;       // 积分系数
    float _kd = 0;       // 微分系数
    float _last_err = 0; // 上次误差
    int _MAX = 0;       // 最大值
    int _MIN = 0;       // 最小值
    int _ControlVelocity = 0; // 控制速度

public:
    // 角度环(PID)
    int Turn_Pid(int now_position,float tar_position);
    // 速度环(增量式)
    int FeedbackControl(int TargetVelocity, int CurrentVelocity);
    // 位置环(PID)
    int PositionControl(int TargetPosition, int CurrentPosition);

};


#endif