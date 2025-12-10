/**
*   @file Motor_dm.h
*   @brief 
*   @author Wenxin HU
*   @date 25-12-9
*   @version 1.0
*   @note
*/
#ifndef MOTOR_DM_H
#define MOTOR_DM_H

#include "Bsp_Fdcan.h"
#include "Alg_Pid.h"

#define KP_MIN 0.0f
#define KP_MAX 500.0f
#define KD_MIN 0.0f
#define KD_MAX 5.0f

extern FDCANBus fdcan1;
extern FDCANBus fdcan2;
extern FDCANBus fdcan3;

void DMMotorDecode(FDCANMember* fdcan_member);

typedef enum alignas(4) {
    DM_4310 = 0,
    DM_4340,
    DM_8006,
    DM_6248,
} DMMotorType_e;

typedef enum alignas(4) {
    DM_MIT = 0,  // MIT模式
    DM_POSITION, // 位置速度模式
    DM_VELOCITY, // 速度模式
} DMCtrlMode_e;

typedef enum alignas(4) {
    DM_DISABLE = 0x00,
    DM_ENABLE = 0x01,
    DM_OVERVOLTAGE = 0x08,
    DM_LESSVOLTAGE = 0x09,
    DM_OVERCURRENT = 0x0A,
    DM_MOSOVERTEMP = 0x0B,
    DM_ROTOROVERTEMP = 0x0C,
    DM_COMMLOST = 0x0D,
    DM_OVERLOAD = 0x0E,
} DMMotorState_e;

typedef struct alignas(4) {
    float PMAX;
    float VMAX;
    float TMAX;
} DMMotorParams_s;

typedef struct alignas(4) {
    DMMotorType_e type;
    uint8_t can_id;    // 电机设置的CANID
    uint8_t master_id; //电机内设置的MasterID
    FDCAN_HandleTypeDef* hfdcan;
    DMMotorParams_s motor_params;
    PIDParam_s angle_pid_param;
    PIDParam_s velocity_pid_param;
} DMMotorConfig_s;

typedef struct alignas(4) {
    uint8_t ID;
    uint8_t ERR;
    uint16_t POS;
    uint16_t VEL;
    uint16_t T;
    uint8_t T_MOS;
    uint8_t T_Rotor;
} DMMotorFeedback_s;

class alignas(4) DMMotor {
private:
    explicit DMMotor(DMMotorConfig_s config)
        : type(config.type),
          can_id(config.can_id),
          master_id(config.master_id),
          motor_params(config.motor_params),
          angle_pid(new Pid(config.angle_pid_param)),
          velocity_pid(new Pid(config.velocity_pid_param)) {
    };

public:
    /**
     * @brief 不允许默认初始化，必须填入FDCAN参数
    */
    DMMotor() = delete;

    static DMMotor* Create(const DMMotorConfig_s& config) {
        auto motor = new DMMotor(config);

        if (motor->angle_pid == nullptr || motor->velocity_pid == nullptr) {
            delete motor;
            return nullptr;
        }

        FDCANBus* bus = nullptr;
        if (config.hfdcan == fdcan1.fdcan_handle) bus = &fdcan1;
        else if (config.hfdcan == fdcan2.fdcan_handle) bus = &fdcan2;
        else if (config.hfdcan == fdcan3.fdcan_handle) bus = &fdcan3;

        if (!bus) {
            delete motor;
            return nullptr;
        } // 句柄不匹配

        motor->fdcan_member =
            bus->MemberRegister(config.can_id, config.master_id, config.hfdcan,
                                motor, DMMotorDecode);

        if (motor->fdcan_member == nullptr) {
            delete motor;
            return nullptr;
        }

        return motor;
    }

    ~DMMotor() {
        if (fdcan_member != nullptr) {
            fdcan_member = nullptr;
        }
        delete angle_pid;
        delete velocity_pid;
    }

    DMMotorType_e type{DM_8006};
    uint8_t can_id{0x00};
    uint8_t master_id{0x00};
    DMMotorParams_s motor_params{};
    DMMotorFeedback_s feedback{};
    DMMotorState_e motor_state{DM_DISABLE};

    FDCANMember* fdcan_member{nullptr};
    Pid* angle_pid{nullptr};
    Pid* velocity_pid{nullptr};

    float out_position{0.0f}; //电机输出轴位置 （-PI~PI）
    float out_velocity{0.0f}; //电机输出速度（rad/s）
    float out_torque{0.0f};   //电机输出力矩
    float output{0.0f};       //电机目标力矩

    void Enable();
    void Disable();

    void MITCtrl(float target_torque);
    void MITCtrl(double target_velocity);
    void MITCtrl(float target_velocity, float target_position);
    void MITCtrl(float position, float velocity, float kp, float kd, float torque);

    void VelocityCtlr(float target_velocity);
    void PositionVelocityCtlr(float target_velocity, float target_position);
};

#endif //MOTOR_DM_H
