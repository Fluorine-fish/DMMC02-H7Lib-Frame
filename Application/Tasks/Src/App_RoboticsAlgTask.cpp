/**
*   @file App_RoboticsAlgTask.cpp
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/

#include "App_TaksPubic.h"
#include "cmsis_os.h"
#include "matrix.h"
#include "robotics.h"
#include "Bsp_dwt.h"

// 自定义的栈空间 分配在dtcm_datae区
__attribute__((section(".dtcm_data"))) uint32_t user_stack[ 2048 ];

__attribute__((section(".dtcm_data"))) float mess[6] = {1.32f, 0.649f, 0.642f, 0.493f, 0.488f, 0.009};
// link质心位置原始数据
__attribute__((section(".dtcm_data"))) float RC[18] = {
    -0.011f, -0.027f, -0.012f, 0.0f, 0.0f, 0.0f,
    0.038f, 0.0f, 0.008f, -0.02f, -0.08, 0.0f,
    0.008f, -0.069f, -0.015f, -0.005f, 0.05f, 0.012f
};
__attribute__((section(".dtcm_data"))) Matrixf<3, 6> centroid(RC);
__attribute__((section(".dtcm_data"))) Matrixf<3, 3> I[6] = {
    matrixf::eye<3, 3>() * 1.0f,
    matrixf::eye<3, 3>() * 1.0f,
    matrixf::eye<3, 3>() * 1.0f,
    matrixf::eye<3, 3>() * 1.0f,
    matrixf::eye<3, 3>() * 1.0f,
    matrixf::eye<3, 3>() * 1.0f
};
__attribute__((section(".dtcm_data"))) robotics::Link links[6] = {
    robotics::Link(0, 0.09955, 0.0, -PI / 2, robotics::R, 0.0,
                   -PI, PI, mess[0], centroid.col(0), I[0]),
    robotics::Link(0, 0.0, 0.14, PI, robotics::R, -PI / 2,
                   -PI, PI, mess[1], centroid.col(1), I[1]),
    robotics::Link(0, 0.0, 0.0, -PI / 2, robotics::R, -PI / 2,
                   -2.7925, 0.0, mess[2], centroid.col(2), I[2]),
    robotics::Link(0, 0.153, 0.0, PI / 2, robotics::R, 0.0,
                   -PI / 2, PI / 2, mess[3], centroid.col(3), I[3]),
    robotics::Link(0, 0.0, 0.0, -PI / 2, robotics::R, 0.0,
                   -2.3561, 2.3561, mess[4], centroid.col(4), I[4]),
    robotics::Link(0, 0.0875, 0.0, 0.0, robotics::R, 0.0,
                   -PI, PI, mess[5], centroid.col(5), I[5]),
};
__attribute__((section(".dtcm_data"))) robotics::Serial_Link<6> miniArm(links);

//前馈增益矩阵
__attribute__((section(".dtcm_data")))float gain[6] = {0.5f, 1.15f, 1.2f, 0.3f, 1.0f, 0.5f};
float us;

//算法输入值
float FeedBack_Position[6] = {};
__attribute__((section(".dtcm_data")))float JointTorque[6];
float FeedForward_Torque[6];

void Target_Remap(float *target) {
    if (!target) return;

    for (uint8_t i = 0; i < 6; i++) {
        if (target[i] < links[i].qmin() ) target[i] = links[i].qmin();
        else if (target[i] > links[i].qmax() ) target[i] = links[i].qmax();
    }

}

void App_RobiticsAlgTask(void *argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2);  // 2ms任务周期

    while (1) {
        uint32_t start = DWT_GetCycle();

        Matrixf<6,1>q = FeedBack_Position;
        Matrixf<6, 1>torque = miniArm.rne(q);

        for (uint8_t i = 0; i < 6; i++) {
            if (abs(torque[i][0]) > 0.01f) FeedForward_Torque[i] = torque[i][0];
            else FeedForward_Torque[i] = 0;
        }

        uint32_t end = DWT_GetCycle();

        us = DWT_GetMicroseconds(start, end);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
