/**
*   @file Bsp_Fdcan.h
*   @brief 
*   @author Wenxin HU
*   @date 25-12-6
*   @version 1.0
*   @note
*/
#ifndef BSP_FDCAN_H
#define BSP_FDCAN_H

#include "fdcan.h"

void FDCAN_Init();

// 给每一个使用FDCAN的模块分配一个成员
struct FDCANMember {
    uint16_t rx_id{0x00};                                 //发送ID
    uint16_t tx_id{0x00};                                 //接收ID
    FDCAN_HandleTypeDef* fdcan_handle{nullptr};           //fdcan句柄，用于校验
    uint8_t tx_buffer[8]{};                               //发送缓存
    uint8_t rx_buffer[8]{};                               //接收缓存
    void (*fdcan_callback)(struct FDCANMember*){nullptr}; //接收回调函数
    void* parent_ptr{nullptr};                            //指向使用FDCAN模块
    void* bus_ptr{nullptr};                               //指向所在的FDCANBus

    /**
     * @brief 不允许默认初始化，必须填入FDCAN参数
    */
    FDCANMember() = delete;

    FDCANMember(uint16_t _rx_id, uint16_t _tx_id, FDCAN_HandleTypeDef* _fdcan_handle, void* _parent_ptr,
                void (*_fdcan_callback)(struct FDCANMember*), void* bus_ptr)
        : rx_id(_rx_id), tx_id(_tx_id), fdcan_handle(_fdcan_handle),
          parent_ptr(_parent_ptr), fdcan_callback(_fdcan_callback) {
    };
};

// 每一路FDCAN总线有Bus一个对象，用于管理总线上的成员
class FDCANBus {
public:
    /**
     * @brief 不允许默认初始化，必须填入FDCAN参数
     */
    FDCANBus() = delete;

    FDCANBus(FDCAN_HandleTypeDef* fdcan_handle, uint8_t max_members)
        : fdcan_handle(fdcan_handle), max_member_cnt(max_members) {

        // FDCAN_Init();

        this->tx_header.IdType = FDCAN_STANDARD_ID;
        this->tx_header.TxFrameType = FDCAN_DATA_FRAME;
        this->tx_header.DataLength = FDCAN_DLC_BYTES_8;
        this->tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  // 传输节点error activate
        this->tx_header.BitRateSwitch = FDCAN_BRS_OFF;           //经典CAN，不使用可变波特率
        this->tx_header.FDFormat = FDCAN_CLASSIC_CAN;            //配置成经典CAN帧格式
        this->tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS; //不存储Tx Event事件
        this->tx_header.MessageMarker = 0;
    };

    FDCAN_HandleTypeDef* fdcan_handle; //fdcan句柄
    FDCAN_TxHeaderTypeDef tx_header;   //fdcan发送报文配置

    uint8_t member_cnt{0};     //包含的成员数目
    uint8_t max_member_cnt{8}; //最大成员数目
    FDCANMember* members[8]    //成员指针数组
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

    FDCANMember* MemberRegister(uint16_t _txid, uint16_t _rxid, FDCAN_HandleTypeDef* fdcan_handle,
                                void* parent_ptr, void (*fdcan_callback)(struct FDCANMember*));
    bool Transmit(FDCANMember* member);

    ~FDCANBus() = default;
};


#endif //BSP_FDCAN_H
