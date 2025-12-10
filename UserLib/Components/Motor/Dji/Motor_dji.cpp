/**
*   @file Motor_dji.cpp
*   @brief DJI电机驱动
*   @author Wenxin HU
*   @date 25-12-7
*   @version 1.0
*   @note
*   @TODO 提供接口在task中解码，而不是中断中
*/
#include "Motor_dji.h"
#include "Bsp_Fdcan.h"
#include "arm_math.h"
#include <cstring>

void DJIMotorDecode(FDCANMember* fdcan_member) {
    if (fdcan_member == nullptr) return;
    if (fdcan_member->parent_ptr == nullptr) return;
    auto motor = static_cast<DJIMotor*>(fdcan_member->parent_ptr);

    motor->feedback.rotor_position =
        static_cast<int16_t>((fdcan_member->rx_buffer[0] << 8) | fdcan_member->rx_buffer[1]);
    motor->feedback.rotor_velocity =
        static_cast<int16_t>((fdcan_member->rx_buffer[2] << 8) | fdcan_member->rx_buffer[3]);
    motor->feedback.torque_current =
        static_cast<int16_t>((fdcan_member->rx_buffer[4] << 8) | fdcan_member->rx_buffer[5]);
    motor->feedback.temperature = fdcan_member->rx_buffer[6];

    motor->out_velocity = motor->feedback.rotor_velocity / motor->reduction_ratio;
    int16_t raw_delta_position = motor->feedback.rotor_position - motor->feedback.rotor_last_position;
    if (raw_delta_position < -6000) raw_delta_position += 8192;
    else if (raw_delta_position > 6000) raw_delta_position -= 8192;
    motor->out_position += (static_cast<float>(raw_delta_position) / 8192.0f) * 2.0f * PI / motor->reduction_ratio;
    if (motor->out_position > PI) motor->out_position -= 2 * PI;
    else if (motor->out_position < -PI) motor->out_position += 2 * PI;
}

void DJIMotor::Ctrl(int16_t target_current) {
    this->output = target_current;
}

void DJIMotor::Ctrl(float target_velocity) {
    if (velocity_pid == nullptr) return;
    this->output = static_cast<int16_t>(velocity_pid->Calc(target_velocity, this->out_velocity));
}

void DJIMotor::Ctrl(float max_velocity, float target_position) {
    if (angle_pid == nullptr || velocity_pid == nullptr) return;
    float target_velocity = angle_pid->Calc(target_position, this->out_position);
    target_velocity = clamp(target_velocity, -max_velocity, max_velocity);
    this->output = static_cast<int16_t>(velocity_pid->Calc(target_velocity, this->out_velocity));
}


void DJIMotorGroup::Transmit() {
    if (!this->fdcan_member) return;
    memset(fdcan_member->tx_buffer, 0, 8);
    this->fdcan_member->tx_buffer[0] = this->motors[0]->output >> 8;
    this->fdcan_member->tx_buffer[1] = this->motors[0]->output;
    if (this->motors[1] != nullptr) {
        this->fdcan_member->tx_buffer[2] = this->motors[1]->output >> 8;
        this->fdcan_member->tx_buffer[3] = this->motors[1]->output;
    } else {
        this->fdcan_member->tx_buffer[2] = 0x00;
        this->fdcan_member->tx_buffer[3] = 0x00;
    }
    if (this->motors[2] != nullptr) {
        this->fdcan_member->tx_buffer[4] = this->motors[2]->output >> 8;
        this->fdcan_member->tx_buffer[5] = this->motors[2]->output;
    } else {
        this->fdcan_member->tx_buffer[4] = 0x00;
        this->fdcan_member->tx_buffer[5] = 0x00;
    }
    if (this->motors[3] != nullptr) {
        this->fdcan_member->tx_buffer[6] = this->motors[3]->output >> 8;
        this->fdcan_member->tx_buffer[7] = this->motors[3]->output;
    } else {
        this->fdcan_member->tx_buffer[6] = 0x00;
        this->fdcan_member->tx_buffer[7] = 0x00;
    }

    fdcan_member->Transmit();
}
