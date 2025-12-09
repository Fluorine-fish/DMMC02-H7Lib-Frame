/**
*   @file globalInit.h
*   @brief 库组件初始化
*   @author Wenxin HU
*   @date 25-12-8
*   @version 1.0
*   @note
*/
#ifndef GLOBALINIT_H
#define GLOBALINIT_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 全局初始化函数
 * @note 此函数用于Lib内组件的初始化，请放置在main.c的FreeRTOS启动之前
 */
void Global_Init(void);

#ifdef __cplusplus
    }
#endif

#endif //GLOBALINIT_H
