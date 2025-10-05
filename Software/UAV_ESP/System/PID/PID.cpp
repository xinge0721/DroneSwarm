#include "PID.h"    

/************
通用转向环或者角度环：输入目标位置和当前位置
*************/
// 函数：角度环(PID)
// 参数1：当前位置
// 参数2：目标位置
// 返回值：PWM值
int pid::Turn_Pid(int now_position,float tar_position)//当前脉冲，目标脉冲 ,角速度
{
    float Err = tar_position - now_position;//目标脉冲-现在脉冲=误差脉冲
	// 限幅
    if(Err>_MAX) {Err -= _MAX * 2;}
    else if(Err<_MIN) {Err += _MIN *2;} 
    // 计算PWM
    float pwm = kp*Err + kd*(Err-_last_err);
    // 更新误差
    _last_err = Err;
	// 返回PWM
    return pwm;
}

/************
增量式速度环
先加i消除误差，再加p消除静态误差
*************/
// 函数：速度环(增量式)
// 参数1：目标速度
// 参数2：当前速度
// 返回值：速度控制值
int pid::FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
		int Bias = TargetVelocity - CurrentVelocity; //求速度偏差
		
		 _ControlVelocity += _kp * (Bias - _last_err) + _ki *Bias;  

		_last_err = Bias;	

		// 使用三目运算符直接限制并返回PWM值
		return (_ControlVelocity > _MAX) ? _MAX : ((_ControlVelocity < -_MIN) ? -_MIN : _ControlVelocity); //返回速度控制值
}

// 函数：位置环(PID)
// 参数1：目标位置
// 参数2：当前位置
// 返回值：速度控制值
int pid::PositionControl(int TargetPosition, int CurrentPosition)
{
		int Bias = TargetPosition - CurrentPosition; //求位置偏差
		_ControlVelocity += _kp * (Bias - _last_err) + _ki *Bias;  //PID控制器
		_last_err = Bias;	
		return (_ControlVelocity > _MAX) ? _MAX : ((_ControlVelocity < -_MIN) ? -_MIN : _ControlVelocity); //返回速度控制值
}