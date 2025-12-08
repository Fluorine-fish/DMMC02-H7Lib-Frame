/**
*   @file Motor_dji.h
*   @brief 
*   @author Wenxin HU
*   @date 25-12-7
*   @version 1.0
*   @note
*/
#ifndef MOTOR_DJI_H
#define MOTOR_DJI_H

#include "Bsp_Fdcan.h"

typedef enum {
    DJI_POSITION = 0, // 速度位置双环控制
    DJI_VELOCITY,     // 速度环控制
} DJIMotorCtrlMode_e;

typedef enum {
    GM6020 = 0,
    M3508,
    M2006,
} DJIMotorType_e;

// 电机CAN反馈的数据
typedef struct {
    int16_t rotor_position;      // 转子角度编码器值
    int16_t rotor_last_position; // 转子上次位置
    int16_t rotor_velocity;      // 转子速度
    int16_t torque_current;      // 电机电流
    int16_t temperature;         // 电机温度
} DJIMotorFeedback_s;

class DJIMotor {
public:
    /**
     * @brief 不允许默认初始化，必须填入电机参数
     */
    DJIMotor() = delete;

    // DJIMotor();

    DJIMotorType_e _type{GM6020};
    DJIMotorCtrlMode_e _ctrl_mode{DJI_POSITION};
    uint8_t _motor_id{0};

    FDCANMember fdcan_member;
};

// DJI电机需要四个电机一组控制
class DJIMotorGroup {
};

#endif //MOTOR_DJI_H
