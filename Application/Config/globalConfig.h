/**
*   @file globalConfig.h
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/
#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

/* 是否使用FreeRTOS */
#define USE_FREERTOS

/* CAN 总线启用 */
#define USER_CAN1                           // 使用 CAN1
#define USER_CAN2                           // 使用 CAN2
// #define USER_CAN3                        // 使用 CAN3

/* CAN FIFO 选择 */
#define USER_CAN1_FIFO_0                    // 使用 CAN1 FIFO0
// #define USER_CAN1_FIFO_1                 // 使用 CAN1 FIFO1
#define USER_CAN2_FIFO_0                 // 使用 CAN2 FIFO0
// #define USER_CAN2_FIFO_1                    // 使用 CAN2 FIFO1
#define USER_CAN3_FIFO_0                 // 使用 CAN3 FIFO0
// #define USER_CAN3_FIFO_1                 // 使用 CAN3 FIFO1

#endif //GLOBALCONFIG_H
