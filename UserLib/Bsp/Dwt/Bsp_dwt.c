/**
*   @file Bsp_dwt.c
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/
#include "Bsp_dwt.h"

void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 允许 DWT
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 启用 cycle counter
}

uint32_t DWT_GetCycle(void) {
    return DWT->CYCCNT;
}

float DWT_GetMicroseconds(uint32_t start, uint32_t end) {
    uint32_t cycles = end - start;
    return (float)cycles / (HAL_RCC_GetHCLKFreq() / 1e6f);
}
