/**
*   @file App_ChassisTask.cpp
*   @brief 底盘task
*   @author Wenxin HU
*   @date 25-12-9
*   @version 1.0
*   @note
*/
#include "App_TaksPubic.h"
#include "cmsis_os.h"
#include "Motor_dji.h"

DJIMotorGroup *M3508Group1 = nullptr;

DJIMotorConfig_s M3508config_1{
    .type = DJI_M3508,
    .id = 1,
    .hfdcan = &hfdcan1,
    .velocity_pid_param = {
        .kp = 120.0f,
        .ki = 60.0f,
        .kd = 0.0f,
        .i_out_max = 3000,
        .out_max = 10000,
    },
    .reduction_ratio = 19,
};

float target_velocity = 10.0f;
extern osMutexId_t RoboticAlgMutexHandle;
extern float FeedBack_Position[6];

void App_ChassisTask(void *argument) {
    while (M3508Group1 == nullptr) M3508Group1 = DJIMotorGroup::Create(&M3508config_1);

    while (1) {
        M3508Group1->motors[0]->Ctrl(target_velocity);
        M3508Group1->Transmit();

        if (osMutexAcquire(RoboticAlgMutexHandle, pdMS_TO_TICKS(1)) == osOK) {
            FeedBack_Position[1] = M3508Group1->motors[0]->out_position;
            osMutexRelease(RoboticAlgMutexHandle);
        }

        osDelay(1);
    }
}
