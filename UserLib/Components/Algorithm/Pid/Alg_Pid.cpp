/**
*   @file Alg_Pid.cpp
*   @brief 参考中科大的PID写法
*   @author Wenxin HU
*   @date 25-12-7
*   @version 1.0
*   @note 1. 增加变速积分
*/
#include "Alg_Pid.h"

float Pid::Calc(float target, float now) {
    Target = target;
    Now = now;
    // 误差
    Error = Target - Now;
    // 绝对值误差
    float abs_error = abs(Error);
    // 判断死区
    if (abs_error < Dead_Zone) {
        Target = Now;
        Error = 0.0f;
        abs_error = 0.0f;
    }else if (Error > 0.0f && abs_error > Dead_Zone){
        Error = Error - Dead_Zone;
    }else if (Error < 0.0f && abs_error > Dead_Zone) {
        Error = Error + Dead_Zone;
    }

    P_Out = Kp * Error;

    if (I_Out_Max != 0.0f) {
        clamp(Integral_Error, -I_Out_Max / Ki, I_Out_Max / Ki);
    }
    if (I_Separate_Threshold == 0.0f) {
        // 不进行积分分离
        Integral_Error += Error * T;
        I_Out = Ki * Integral_Error;
    }else {
        if (abs_error < I_Separate_Threshold) {
            // 不在分离区间内
            Integral_Error += Error * T;
            I_Out = Ki * Integral_Error;
        }else {
            Integral_Error = 0.0f;
            I_Out = 0.0f;
        }
    }

    if (DFirst == PID_DFIRST_ON) {
        D_Out = -Kd * (Now - Pre_Now) / T;
    }else {
        D_Out = Kd * (Now - Pre_Now) / T;
    }

    Output = P_Out + D_Out + I_Out;

    // 迭代
    Pre_Now = Now;
    Pre_Target = Target;
    Pre_Error = Error;
    Pre_Output = Output;

    return Output;
}

