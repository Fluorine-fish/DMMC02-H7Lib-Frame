/**
*   @file globalInit.cpp
*   @brief 库组件初始化
*   @author Wenxin HU
*   @date 25-12-8
*   @version 1.0
*   @note
*/
#include "globalInit.h"
#include "Bsp_dwt.h"
#include "Bsp_Fdcan.h"
#include "usb_device.h"

void Global_Init(void) {
    DWT_Init();
    FDCAN_Init();
    MX_USB_DEVICE_Init();
}
