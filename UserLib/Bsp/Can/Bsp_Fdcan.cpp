/**
*   @file Bsp_Fdcan.cpp
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/
#include "Bsp_Fdcan.h"
#include "fdcan.h"
#include "sys_public.h"

#include <cstring>

static bool is_FDCAN_Enabled = false;

FDCAN_FilterTypeDef FDCAN_FIFO0_Filter = {
    .IdType = FDCAN_STANDARD_ID,
    .FilterIndex = 0, // 过滤器编号
    .FilterType = FDCAN_FILTER_MASK, // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
    .FilterID1 = 0x00000000, // 过滤器 ID1，只要 ID2 配置为 0x00000000，就不会过滤任何 ID
    .FilterID2 = 0x00000000, // 过滤器 ID2
};
FDCAN_FilterTypeDef FDCAN_FIFO1_Filter = {
    .IdType = FDCAN_STANDARD_ID,
    .FilterIndex = 0, // 过滤器编号
    .FilterType = FDCAN_FILTER_MASK, // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    .FilterConfig = FDCAN_FILTER_TO_RXFIFO1,
    .FilterID1 = 0x00000000, // 过滤器 ID1，只要 ID2 配置为 0x00000000，就不会过滤任何 ID
    .FilterID2 = 0x00000000, // 过滤器 ID2
};

// 全局的FDCANBus 存储到DTCM区域
__attribute__((section(".dtcm_data"))) FDCANBus Fdcan1(&hfdcan1, 8);
__attribute__((section(".dtcm_data"))) FDCANBus Fdcan2(&hfdcan2, 8);
__attribute__((section(".dtcm_data"))) FDCANBus Fdcan3(&hfdcan3, 8);

/* 私有函数 ---------------------------------------------------------------------*/
/**
 * @brief 初始化 FDCAN 过滤器配置。
 *
 * 该函数根据用户定义的宏（USER_CAN1, USER_CAN2, USER_CAN3）初始化相应的 FDCAN 实例的过滤器。对于每个启用的 FDCAN 实例，它首先确定 ID 类型（标准或扩展），然后选择接收 FIFO（FIFO0 或 FIFO1）。如果两个 FIFO 都没有元素，则记录错误并返回 false。接着，配置过滤器参数，包括 ID 类型、过滤器索引、过滤器类型和过滤器配置。此外，还配置了全局过滤器以拒绝不匹配的标准 ID 和扩展 ID 以及远程帧，并设置了 FIFO 水印中断。如果在配置过程中出现任何错误，将记录错误日志并重试直到成功。
 * 注意：此函数依赖于 HAL 库提供的 FDCAN 相关 API。
 * @return 如果所有 FDCAN 实例的过滤器配置成功，返回 true；否则返回 false。
 */
static void Can_Filter_Init(void){
#ifdef USER_CAN1_FIFO_0

    while (HAL_FDCAN_ConfigFilter(&hfdcan1, &FDCAN_FIFO0_Filter) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 Fifo0 configs filter failed");
        #endif
    }
    // 水印中断，接受 1 条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 1) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 Fifo0 configs fifo watermark failed");
        #endif
    }
#endif
#ifdef USER_CAN1_FIFO_1
    while (HAL_FDCAN_ConfigFilter(&hfdcan1, &FDCAN_FIFO1_Filter) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 Fifo1 configs filter failed");
        #endif
    }
    // 水印中断，接受 1 条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO1, 1) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 Fifo1 configs fifo watermark failed");
        #endif
    }
#endif
#ifdef USER_CAN2_FIFO_0
    while (HAL_FDCAN_ConfigFilter(&hfdcan2, &FDCAN_FIFO0_Filter) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 Fifo0 configs filter failed");
        #endif
    }
    // 水印中断，接受 1 条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan2, FDCAN_CFG_RX_FIFO0, 1) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 Fifo0 configs fifo watermark failed");
        #endif
    }
#endif
#ifdef USER_CAN2_FIFO_1
    while (HAL_FDCAN_ConfigFilter(&hfdcan2, &FDCAN_FIFO1_Filter) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 Fifo1 configs filter failed");
        #endif
    }
    // 水印中断，接受 1 条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan2, FDCAN_CFG_RX_FIFO1, 1) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 Fifo1 configs fifo watermark failed");
        #endif
    }
#endif
#ifdef USER_CAN3_FIFO_0
    while (HAL_FDCAN_ConfigFilter(&hfdcan3, &FDCAN_FIFO0_Filter) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 Fifo0 configs filter failed");
        #endif
    }
    // 水印中断，接受 1 条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan3, FDCAN_CFG_RX_FIFO0, 1) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 Fifo0 configs fifo watermark failed");
        #endif
    }
#endif
#ifdef USER_CAN3_FIFO_1
    while (HAL_FDCAN_ConfigFilter(&hfdcan3, &FDCAN_FIFO1_Filter) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 Fifo1 configs filter failed");
        #endif
    }
    // 水印中断，接受 1 条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan3, FDCAN_CFG_RX_FIFO1, 1) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 Fifo1 configs fifo watermark failed");
        #endif
    }
#endif
}

/**
 * @brief 初始化CAN服务。
 *
 * 该函数根据用户配置的宏定义（USER_CAN1, USER_CAN2, USER_CAN3）来初始化相应的FDCAN硬件实例。对于每个启用的FDCAN实例，首先尝试启动它。如果启动失败，则记录错误日志并继续重试直到成功。随后，根据USER_CANx_FIFOx的值选择激活Rx FIFO 0或Rx FIFO 1的消息接收中断通知。如果激活中断通知失败，同样会记录错误日志，并且持续尝试直到成功为止。
 * 注意：此函数依赖于HAL库提供的FDCAN相关API。
 * @return 无返回值（void）。
 */
static void Can_Service_Init(void){
#ifdef USER_CAN1
    // 拒绝接收匹配不成功的标准 ID 和扩展 ID, 不接受远程帧
    while (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) !=HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 config global filter failed");
        #endif
    }
    while (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 Starts Failed");
        #endif
    }
#endif
#ifdef USER_CAN2
    // 拒绝接收匹配不成功的标准 ID 和扩展 ID, 不接受远程帧
    while (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) !=HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 config global filter failed");
        #endif
    }
    while (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 Starts Failed");
        #endif
    }
#endif
#ifdef USER_CAN3
    // 拒绝接收匹配不成功的标准 ID 和扩展 ID, 不接受远程帧
    while (HAL_FDCAN_ConfigGlobalFilter(&hfdcan3, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) !=HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 config global filter failed");
        #endif

    }
    while (HAL_FDCAN_Start(&hfdcan3) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 Starts Failed");
        #endif
    }
#endif
#ifdef USER_CAN1_FIFO_0
    while (HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 Fifo0 configs interruption failed");
        #endif
    }
#endif

#ifdef USER_CAN1_FIFO_1
    while (HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN1 Fifo1 configs interruption failed");
        #endif
    }
#endif


#ifdef USER_CAN2_FIFO_0
    while (HAL_FDCAN_ActivateNotification(&hfdcan2,FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 Fifo0 configs interruption failed");
        #endif
    }
#endif

#ifdef USER_CAN2_FIFO_1
    while (HAL_FDCAN_ActivateNotification(&hfdcan2,FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN2 Fifo1 configs interruption failed");
        #endif
    }
#endif

#ifdef USER_CAN3_FIFO_0
    while (HAL_FDCAN_ActivateNotification(&hfdcan3,FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 Fifo0 configs interruption failed");
        #endif
    }
#endif
#ifdef USER_CAN3_FIFO_1
    while (HAL_FDCAN_ActivateNotification(&hfdcan3,FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) {
        #ifdef USER_LOG
        Log_Error("FDCAN3 Fifo1 configs interruption failed");
        #endif
    }
#endif
}

void FDCAN_Init() {
    if (!is_FDCAN_Enabled) {
        Can_Filter_Init();
        Can_Service_Init();
        is_FDCAN_Enabled = true;
    }
}

FDCANMember* FDCANBus::MemberRegister(uint16_t _txid, uint16_t _rxid, FDCAN_HandleTypeDef* fdcan_handle,
        void* parent_ptr, void (*fdcan_callback)(struct FDCANMember*)) {
    if (fdcan_handle == nullptr) return nullptr;
    if (fdcan_handle  != this->fdcan_handle) return nullptr;

    if (this->member_cnt > this->max_member_cnt) return nullptr;
    auto *member = new FDCANMember(_txid, _rxid, fdcan_handle, parent_ptr, fdcan_callback, this);
    if (member == nullptr) return nullptr;

    std::memset(member->tx_buffer, 0, 8);
    std::memset(member->rx_buffer, 0, 8);

    // 存入member池
    this->members[this->member_cnt] = member;
    this->member_cnt++;

    return member;
}

bool FDCANBus::Transmit(FDCANMember *member) {
    if (member == nullptr) return false;
    if (member->fdcan_handle != this->fdcan_handle) return false;

    uint8_t can_tx_cnt = 0;
    while (HAL_FDCAN_GetTxFifoFreeLevel(member->fdcan_handle) == 0) {
        // 检查发送缓冲区直到有空闲或超时退出
        can_tx_cnt++;
        if (can_tx_cnt >= 100) {
            return false;
        }
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(member->fdcan_handle,
        &static_cast<FDCANBus*>(member->bus_ptr)->tx_header,member->tx_buffer) == HAL_OK) {
        return true;
    }
    return false;
}



/**
 * @brief FDCAN接收FIFO0中断的回调函数。
 *
 * 该函数处理接收FIFO0中的消息。当检测到新的消息时，它会根据配置调用相应的用户定义的回调函数。
 *
 * @param hfdcan 指向包含指定FDCAN配置信息的FDCAN_HandleTypeDef结构的指针。
 * @param RxFifo0ITs 触发此回调的中断源。
 */
// ReSharper disable once CppParameterMayBeConst
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
#ifdef USER_CAN1_FIFO_0
        if (hfdcan == &hfdcan1){
            uint8_t msg[8];
            FDCAN_RxHeaderTypeDef header;
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, msg);

            for (const auto &member : Fdcan1.members) {
                if (member != nullptr && member->rx_id == header.Identifier) {
                    std::memcpy(member->rx_buffer, msg, 8);
                    if (member->fdcan_callback != nullptr) {
                        member->fdcan_callback(member);
                    }
                }
            }
        }
#endif
#ifdef USER_CAN2_FIFO_0
        if (hfdcan == &hfdcan2){
            uint8_t msg[8];
            FDCAN_RxHeaderTypeDef header;
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, msg);

            for (const auto &member : Fdcan2.members) {
                if (member != nullptr && member->rx_id == header.Identifier) {
                    std::memcpy(member->rx_buffer, msg, 8);
                    if (member->fdcan_callback != nullptr) {
                        member->fdcan_callback(member);
                    }
                }
            }
        }
#endif
#ifdef USER_CAN3_FIFO_0
        if (hfdcan == &hfdcan3){
            uint8_t msg[8];
            FDCAN_RxHeaderTypeDef header;
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, msg);

            for (const auto &member : Fdcan3.members) {
                if (member != nullptr && member->rx_id == header.Identifier) {
                    std::memcpy(member->rx_buffer, msg, 8);
                    if (member->fdcan_callback != nullptr) {
                        member->fdcan_callback(member);
                    }
                }
            }
        }
#endif
    }
}

/**
 * @brief FDCAN接收FIFO1中断的回调函数。
 *
 * 该函数处理来自FDCAN接收FIFO1中的消息。当接收到新消息时，它会根据配置调用相应的用户定义回调函数。
 *
 * @param hfdcan 指向包含指定FDCAN配置信息的FDCAN_HandleTypeDef结构的指针。
 * @param RxFifo1ITs 触发此回调的中断源。
 */
// ReSharper disable once CppParameterMayBeConst
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
    if (RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) {
#ifdef USER_CAN1_FIFO_1
        if (hfdcan == &hfdcan1){
            uint8_t msg[8];
            FDCAN_RxHeaderTypeDef header;
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &header, msg);

            for (const auto &member : Fdcan1.members) {
                if (member != nullptr && member->rx_id == header.Identifier) {
                    std::memcpy(member->rx_buffer, msg, 8);
                    if (member->fdcan_callback != nullptr) {
                        member->fdcan_callback(member);
                    }
                }
            }
        }
#endif
#ifdef USER_CAN2_FIFO_1
        if (hfdcan == &hfdcan2){
            uint8_t msg[8];
            FDCAN_RxHeaderTypeDef header;
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &header, msg);

            for (const auto &member : Fdcan2.members) {
                if (member != nullptr && member->rx_id == header.Identifier) {
                    std::memcpy(member->rx_buffer, msg, 8);
                    if (member->fdcan_callback != nullptr) {
                        member->fdcan_callback(member);
                    }
                }
            }
        }
#endif
#ifdef USER_CAN3_FIFO_1
        if (hfdcan == &hfdcan3){
            uint8_t msg[8];
            FDCAN_RxHeaderTypeDef header;
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &header, msg);

            for (const auto &member : Fdcan3.members) {
                if (member != nullptr && member->rx_id == header.Identifier) {
                    std::memcpy(member->rx_buffer, msg, 8);
                    if (member->fdcan_callback != nullptr) {
                        member->fdcan_callback(member);
                    }
                }
            }
        }
#endif
    }
}
