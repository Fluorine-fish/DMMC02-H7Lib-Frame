/**
*   @file Device_Chassis.cpp
*   @brief 底盘类，提供麦克纳姆轮底盘，月球车接口
*   @author Wenxin HU
*   @date 25-12-9
*   @version 1.0
*   @note
*/
#include "Device_Chassis.h"

void MecanumChassis::Ik_Calc() {
    wheel_velocity[0] = (-chassis_velocity.Vx + chassis_velocity.Vy + chassis_velocity.Vw * (chassis_param.length_a + chassis_param.length_b)) * 60.0f / (3.14f * chassis_param.wheel_radius);
    wheel_velocity[1] = (-chassis_velocity.Vx - chassis_velocity.Vy + chassis_velocity.Vw * (chassis_param.length_a + chassis_param.length_b)) * 60.0f / (3.14f * chassis_param.wheel_radius) ;
    wheel_velocity[2] = (chassis_velocity.Vx - chassis_velocity.Vy + chassis_velocity.Vw * (chassis_param.length_a + chassis_param.length_b)) * 60.0f / (3.14f * chassis_param.wheel_radius);
    wheel_velocity[3] = ( chassis_velocity.Vx + chassis_velocity.Vy + chassis_velocity.Vw * (chassis_param.length_a + chassis_param.length_b)) * 60.0f / (3.14f * chassis_param.wheel_radius);
}

void MecanumChassis::Ctrl() {
    Ik_Calc();

    if (motor_group != nullptr) {
        motor_group->motors[0]->Ctrl(wheel_velocity[0]);
        motor_group->motors[1]->Ctrl(wheel_velocity[1]);
        motor_group->motors[2]->Ctrl(wheel_velocity[2]);
        motor_group->motors[3]->Ctrl(wheel_velocity[3]);
    }
}


