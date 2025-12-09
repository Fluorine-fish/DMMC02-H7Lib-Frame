/**
*   @file App_TaksPubic.h
*   @brief FreeRTOS任务公共头文件
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note 任务函数必须在此文件定义,否则在app_freertos.c中找不到该函数符号
*/
#ifndef APP_TAKSPUBIC_H
#define APP_TAKSPUBIC_H

#ifdef __cplusplus
extern "C" {
#endif

/**=======================================C头文件内容======================================**/
//任务句柄和任务函数声明
//任务函数必须在此声明(在extern "C"块中),否则在app_freertos.c中找不到该函数符号
void App_DebugTask(void *argument);
void App_RobiticsAlgTask(void *argument);
void App_ChassisTask(void *argument);
/**======================================================================================**/

#ifdef __cplusplus
};
#endif

#endif //APP_TAKSPUBIC_H
