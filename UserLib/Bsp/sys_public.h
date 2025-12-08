/**
*   @file sys_public.h
*   @brief 用于支持简单平台移植的中间层
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note 用户需要根据提示定义相关宏函数,其中,MAX_DELAY应与delay()匹配
*/
#ifndef SYS_PUBLIC_H
#define SYS_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

/**====================================User Code Begin====================================**/
#include "cmsis_os2.h"
/**=====================================User Code End=====================================**/

/**====================================用户需要定义的部分====================================**/
#define SYS_LOG_ENABLE          0                   //开启系统日志,注意:关闭后也可在代码里使用log(),只是没有动作
#define MAX_DELAY               portMAX_DELAY       //最大延时时间,单位ms
#define TX_BUFFER_LENGTH        64                  //系统日志输出缓冲区大小

#define delay(ms)               osDelay(ms)         //延时函数
#define get_tick()              xTaskGetTickCount() //获取系统滴答计数
#define sys_log_function(...)   printf(__VA_ARGS__)
/**=======================================================================================**/

#ifdef DEBUG
#endif

#if SYS_LOG_ENABLE == 1
#define sys_log(...)            sys_log_function(__VA_ARGS__)
#define sys_log_info(...)       sys_log_function("INFO: " __VA_ARGS__)
#define sys_log_warning(...)    sys_log_function("WARNING: " __VA_ARGS__)
#define sys_log_error(...)      sys_log_function("ERROR: " __VA_ARGS__)
#else
#define sys_log(...)            ((void)0U)
#define sys_log_info(...)       ((void)0U)
#define sys_log_warning(...)    ((void)0U)
#define sys_log_error(...)      ((void)0U)
#endif

#ifdef __cplusplus
};

/**====================================User Code Begin====================================**/
#include <cstdint>
#include "FreeRTOS.h"
#include "globalConfig.h"
/**=====================================User Code End=====================================**/
inline void* operator new(const std::size_t size) {
    return pvPortMalloc(size);
}

inline void* operator new[](const std::size_t size) {
    return pvPortMalloc(size);
}

inline void operator delete(void *ptr) {
    vPortFree(ptr);
}

inline void operator delete(void *ptr, std::size_t) {
    vPortFree(ptr);
}

inline void operator delete[](void *ptr) {
    vPortFree(ptr);
}

inline void operator delete[](void *ptr, std::size_t) {
    vPortFree(ptr);
}
#endif

#endif //SYS_PUBLIC_H
