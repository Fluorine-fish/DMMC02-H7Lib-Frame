/**
*   @file App_DebugTask.cpp
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/
#include "App_TaksPubic.h"
#include "cmsis_os2.h"

void App_DebugTask(void *argument) {
    while (1) {
        osDelay(1);
    }
}
