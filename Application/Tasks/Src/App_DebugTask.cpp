/**
*   @file App_DebugTask.cpp
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/
#include "App_TaksPubic.h"
#include "cmsis_os.h"
#include "Motor_dm.h"

DMMotor *J6248_1 = nullptr;
DMMotor *J8006_1 = nullptr;

DMMotorConfig_s J6248_Config_1 = {
    .type = DM_6248,
    .can_id = 0x01,
    .master_id = 0x00,
    .hfdcan = &hfdcan2,
    .motor_params = {
        .PMAX = 12.566f,
        .VMAX = 20.0f,
        .TMAX = 120.0f,
    }
};

DMMotorConfig_s J8006_Config_1 = {
    .type = DM_8006,
    .can_id = 0x01,
    .master_id = 0x00,
    .hfdcan = &hfdcan2,
    .motor_params = {
        .PMAX = 12.5f,
        .VMAX = 45.0f,
        .TMAX = 20.0f,
    }
};

float target_torque = 0.0f;

void App_DebugTask(void *argument) {
    while (J6248_1 == nullptr) J6248_1 = DMMotor::Create(J6248_Config_1);
    while (J8006_1 == nullptr) J8006_1 = DMMotor::Create(J8006_Config_1);
    // while (J6248_1->motor_state == DM_DISABLE) {
    //     J6248_1->Enable();
    //     osDelay(1);
    // }
    while (J8006_1->motor_state == DM_DISABLE) {
        J8006_1->Enable();
        osDelay(1);
    }

    while (1) {
        J8006_1->MITCtrl(target_torque);
        osDelay(1);
    }
}
