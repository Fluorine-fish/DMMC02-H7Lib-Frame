/**
*   @file Bsp_dwt.h
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/
#ifndef BSP_DWT_H
#define BSP_DWT_H

#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void DWT_Init(void);

uint32_t DWT_GetCycle(void);

/**
 * @brief 使用dwt定时器获取一段时间长度
 * @param start 使用DWT_GetCycle()得到的开始时间
 * @param end 使用DWT_GetCycle()得到的结束时间
 * @return 时间段长度，单位us
 */
float DWT_GetMicroseconds(uint32_t start, uint32_t end);

#ifdef __cplusplus
}
#endif

#endif //BSP_DWT_H
