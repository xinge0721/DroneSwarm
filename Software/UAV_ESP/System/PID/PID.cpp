#include "PID.h"    

// 带参数构造函数
PID::PID(float kp , float ki , float kd , int max_val , int min_val)
{
    _kp = kp;
    _ki = ki;
    _kd = kd;
    _last_err = 0;
    _integral = 0;
    _MAX = max_val;
    _MIN = min_val;
    _ControlVelocity = 0;
}

/************
通用转向环或者角度环：输入目标位置和当前位置
*************/
// 函数：角度环(PID)
// 参数1：当前位置
// 参数2：目标位置
// 返回值：PWM值
int PID::Turn_Pid(int now_position,float tar_position)//当前脉冲，目标脉冲 ,角速度
{
    float Err = tar_position - now_position;//目标脉冲-现在脉冲=误差脉冲
    
    // 角度环绕处理（如果是角度控制，处理0-360度环绕）
    if(Err > 180) {Err -= 360;}
    else if(Err < -180) {Err += 360;} 
    
    // 积分项累积（防止积分饱和）
    _integral += Err;
    if(_integral > _MAX) _integral = _MAX;
    else if(_integral < _MIN) _integral = _MIN;
    
    // 微分项
    float derivative = Err - _last_err;
    
    // 计算PID输出
    float output = _kp * Err + _ki * _integral + _kd * derivative;
    
    // 更新误差
    _last_err = Err;
    
    // 输出限幅
    if(output > _MAX) output = _MAX;
    else if(output < _MIN) output = _MIN;
    
	// 返回PWM
    return (int)output;
}

/************
增量式速度环
先加i消除误差，再加p消除静态误差
*************/
// 函数：速度环(增量式)
// 参数1：目标速度
// 参数2：当前速度
// 返回值：速度控制值
int PID::FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
    
    int Bias = TargetVelocity - CurrentVelocity; //求速度偏差
    
    // 增量式PID计算: Δu(k) = Kp[e(k)-e(k-1)] + Ki*e(k) + Kd[e(k)-2e(k-1)+e(k-2)]
    float increment = _kp * (Bias - _last_err) + _ki * Bias + _kd * (Bias - 2*_last_err + _last_err);
    
    _ControlVelocity += increment;  
    
    // 更新误差历史
    _last_err = Bias;	

    // 输出限制
    if(_ControlVelocity > _MAX) _ControlVelocity = _MAX;
    else if(_ControlVelocity < _MIN) _ControlVelocity = _MIN;
    
    return _ControlVelocity; //返回速度控制值
}

// 函数：位置环(PID)
// 参数1：目标位置
// 参数2：当前位置
// 返回值：速度控制值
int PID::PositionControl(int TargetPosition, int CurrentPosition)
{
    float Bias = TargetPosition - CurrentPosition; //求位置偏差
    
    // 积分项累积（防止积分饱和）
    _integral += Bias;
    if(_integral > _MAX) _integral = _MAX;
    else if(_integral < _MIN) _integral = _MIN;
    
    // 微分项
    float derivative = Bias - _last_err;
    
    // 完整PID控制器：u(k) = Kp*e(k) + Ki*Σe(k) + Kd*[e(k)-e(k-1)]
    float output = _kp * Bias + _ki * _integral + _kd * derivative;
    
    // 更新误差
    _last_err = Bias;
    
    // 输出限制
    if(output > _MAX) output = _MAX;
    else if(output < _MIN) output = _MIN;
    
    return (int)output; //返回位置控制值
}