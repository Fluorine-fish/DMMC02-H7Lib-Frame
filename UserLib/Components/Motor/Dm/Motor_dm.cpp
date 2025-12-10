/**
*   @file Motor_dm.cpp
*   @brief 
*   @author Wenxin HU
*   @date 25-12-9
*   @version 1.0
*   @note
*/
#include "Motor_dm.h"
#include "arm_math.h"

// DM的解码转换函数
static int float_to_uint(const float x_float, const float x_min, const float x_max, const int bits){
    const float span = x_max - x_min;
    const float offset = x_min;
    return (int)((x_float - offset) * ((float)((1 << bits) - 1)) / span);
}
static float uint_to_float(const int x_int, const float x_min, const float x_max, const int bits){
    const float span = x_max - x_min;
    const float offset = x_min;
    return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}

void DMMotorDecode(FDCANMember* fdcan_member) {
    auto motor = static_cast<DMMotor*>(fdcan_member->parent_ptr);

    motor->feedback.ERR = fdcan_member->rx_buffer[0] >> 4;
    motor->feedback.ID = fdcan_member->rx_buffer[0] & 0x0F;
    motor->feedback.POS = (fdcan_member->rx_buffer[1] << 8) | fdcan_member->rx_buffer[2];
    motor->feedback.VEL = (fdcan_member->rx_buffer[3] << 4) | (fdcan_member->rx_buffer[4] >> 4);
    motor->feedback.T = ((fdcan_member->rx_buffer[4] & 0x0F) << 8) | fdcan_member->rx_buffer[5];
    motor->feedback.T_MOS = fdcan_member->rx_buffer[6];
    motor->feedback.T_Rotor = fdcan_member->rx_buffer[7];

    float tmp_position = uint_to_float(motor->feedback.POS, -motor->motor_params.PMAX, motor->motor_params.PMAX, 16);
    motor->out_velocity = uint_to_float(motor->feedback.VEL, -motor->motor_params.VMAX, motor->motor_params.VMAX, 12);
    motor->out_torque = uint_to_float(motor->feedback.T, -motor->motor_params.TMAX, motor->motor_params.TMAX, 12);

    // remap
    if (tmp_position <= PI && tmp_position >= -PI) motor->out_position = tmp_position;
    else if (tmp_position > PI && tmp_position <= 3*PI) motor->out_position = tmp_position - 2*PI;
    else if (tmp_position < -PI && tmp_position >= -3*PI) motor->out_position = tmp_position + 2*PI;
    else if (tmp_position > 3*PI) motor->out_position= tmp_position - 4*PI;
    else if (tmp_position < -3*PI) motor->out_position = tmp_position + 4*PI;
    else motor->out_position = tmp_position;

    switch (motor->feedback.ERR) {
        case DM_ENABLE:
            motor->motor_state = DM_ENABLE;
            break;
        case DM_OVERVOLTAGE:
            motor->motor_state = DM_OVERVOLTAGE;
            break;
        case DM_LESSVOLTAGE:
            motor->motor_state = DM_LESSVOLTAGE;
            break;
        case DM_OVERCURRENT:
            motor->motor_state = DM_OVERCURRENT;
            break;
        case DM_MOSOVERTEMP:
            motor->motor_state = DM_MOSOVERTEMP;
            break;
        case DM_ROTOROVERTEMP:
            motor->motor_state = DM_ROTOROVERTEMP;
            break;
        case DM_COMMLOST:
            motor->motor_state = DM_COMMLOST;
            break;
        case DM_OVERLOAD:
            motor->motor_state = DM_OVERLOAD;
            break;
        case DM_DISABLE:
        default:
            motor->motor_state = DM_DISABLE;
            break;
    }
}

void DMMotor::Enable() {
    fdcan_member->tx_buffer[0] = 0xFF;
    fdcan_member->tx_buffer[1] = 0xFF;
    fdcan_member->tx_buffer[2] = 0xFF;
    fdcan_member->tx_buffer[3] = 0xFF;
    fdcan_member->tx_buffer[4] = 0xFF;
    fdcan_member->tx_buffer[5] = 0xFF;
    fdcan_member->tx_buffer[6] = 0xFF;
    fdcan_member->tx_buffer[7] = 0xFC;

    fdcan_member->Transmit();
}

void DMMotor::Disable() {
    fdcan_member->tx_buffer[0] = 0xFF;
    fdcan_member->tx_buffer[1] = 0xFF;
    fdcan_member->tx_buffer[2] = 0xFF;
    fdcan_member->tx_buffer[3] = 0xFF;
    fdcan_member->tx_buffer[4] = 0xFF;
    fdcan_member->tx_buffer[5] = 0xFF;
    fdcan_member->tx_buffer[6] = 0xFF;
    fdcan_member->tx_buffer[7] = 0xFD;

    fdcan_member->Transmit();
}

void DMMotor::MITCtrl(float position, float velocity, float kp, float kd, float torque) {
    uint16_t p_des = float_to_uint(position, -motor_params.PMAX, motor_params.PMAX, 16);
    uint16_t v_des = float_to_uint(velocity, -motor_params.VMAX, motor_params.VMAX, 12);
    uint16_t t_des = float_to_uint(torque, -motor_params.TMAX, motor_params.TMAX, 12);
    uint16_t Kp = float_to_uint(kp, KP_MIN, KP_MAX, 12);
    uint16_t Kd = float_to_uint(kd, KD_MIN, KD_MAX, 12);

    fdcan_member->tx_buffer[0] = p_des >> 8;
    fdcan_member->tx_buffer[1] = p_des;
    fdcan_member->tx_buffer[2] = v_des >> 4;
    fdcan_member->tx_buffer[3] = ((v_des & 0x0F) << 4) | (Kp >> 8);
    fdcan_member->tx_buffer[4] = Kp;
    fdcan_member->tx_buffer[5] = Kd >> 4;
    fdcan_member->tx_buffer[6] = ((Kd & 0x0F) << 4) | (t_des >> 8);
    fdcan_member->tx_buffer[7] = t_des;

    fdcan_member->Transmit();
}

void DMMotor::MITCtrl(float target_torque) {
    MITCtrl(0.0f, 0.0f, 0.0f, 0.0f, target_torque);
}

void DMMotor::MITCtrl(double target_velocity) {
    if (velocity_pid == nullptr) return;
    float target_torque = velocity_pid->Calc(static_cast<float>(target_velocity), this->out_velocity);
    MITCtrl(target_torque);
}

void DMMotor::MITCtrl(float target_velocity, float target_position) {
    if (velocity_pid == nullptr || angle_pid == nullptr) return;
    float tmp_target_velocity = angle_pid->Calc(target_position, this->out_position);
    tmp_target_velocity = clamp(tmp_target_velocity, -target_velocity, target_velocity);
    MITCtrl(static_cast<double>(tmp_target_velocity));
}

void DMMotor::VelocityCtlr(float target_velocity) {
    uint8_t* vel = (uint8_t*)&target_velocity;

    fdcan_member->tx_buffer[0] = *vel;
    fdcan_member->tx_buffer[1] = *(vel+1);
    fdcan_member->tx_buffer[2] = *(vel+2);
    fdcan_member->tx_buffer[3] = *(vel+3);
    fdcan_member->tx_buffer[4] = 0x00;
    fdcan_member->tx_buffer[5] = 0x00;
    fdcan_member->tx_buffer[6] = 0x00;
    fdcan_member->tx_buffer[7] = 0x00;

    fdcan_member->tx_id += 0x200;
    fdcan_member->Transmit();
    fdcan_member->tx_id -= 0x200;
}

void DMMotor::PositionVelocityCtlr(float target_velocity, float target_position) {
    uint8_t* vel = (uint8_t*)&target_velocity;
    uint8_t* pos = (uint8_t*)&target_position;

    fdcan_member->tx_buffer[0] = *vel;
    fdcan_member->tx_buffer[1] = *(vel+1);
    fdcan_member->tx_buffer[2] = *(vel+2);
    fdcan_member->tx_buffer[3] = *(vel+3);
    fdcan_member->tx_buffer[0] = *pos;
    fdcan_member->tx_buffer[1] = *(pos+1);
    fdcan_member->tx_buffer[2] = *(pos+2);
    fdcan_member->tx_buffer[3] = *(pos+3);

    fdcan_member->tx_id += 0x100;
    fdcan_member->Transmit();
    fdcan_member->tx_id -= 0x100;
}
