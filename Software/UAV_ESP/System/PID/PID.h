#ifndef PID_H
#define PID_H

// PID控制器
class PID
{
private:

    float _last_err ; // 上次误差
    float _integral ; // 积分项累积
    int _MAX ;       // 最大值
    int _MIN ;       // 最小值
    int _ControlVelocity ; // 控制速度

public:
    float _kp ;       // 比例系数
    float _ki ;       // 积分系数
    float _kd ;       // 微分系数
    
    PID(float kp = 0, float ki = 0, float kd = 0, int max_val = 255, int min_val = -255);
    
    // 角度环(PID)
    int Turn_Pid(int now_position,float tar_position);
    // 速度环(增量式)
    int FeedbackControl(int TargetVelocity, int CurrentVelocity);
    // 位置环(PID)
    int PositionControl(int TargetPosition, int CurrentPosition);

};


#endif