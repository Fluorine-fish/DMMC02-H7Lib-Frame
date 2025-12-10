/**
*   @file Motor_dji.h
*   @brief 
*   @author Wenxin HU
*   @date 25-12-7
*   @version 1.0
*   @note
*/
#ifndef MOTOR_DJI_H
#define MOTOR_DJI_H

#include "Bsp_Fdcan.h"
#include "Alg_Pid.h"

extern FDCANBus fdcan1;
extern FDCANBus fdcan2;
extern FDCANBus fdcan3;

typedef enum {
    DJI_GM6020 = 0,
    DJI_M3508,
    DJI_M2006,
} DJIMotorType_e;

typedef struct alignas(4) {
    DJIMotorType_e type;
    uint8_t id;
    FDCAN_HandleTypeDef* hfdcan;
    PIDParam_s angle_pid_param;
    PIDParam_s velocity_pid_param;
    float reduction_ratio; // 电机减速比
} DJIMotorConfig_s;

// 电机CAN反馈的数据
typedef struct alignas(4) {
    int16_t rotor_position;      // 转子角度编码器值
    int16_t rotor_last_position; // 转子上次位置
    int16_t rotor_velocity;      // 转子速度
    int16_t torque_current;      // 电机电流
    int16_t temperature;         // 电机温度
} DJIMotorFeedback_s;

void DJIMotorDecode(FDCANMember* fdcan_member);

class alignas(4) DJIMotor {
public:
    /**
     * @brief 不允许默认初始化，必须填入电机参数
     */
    DJIMotor() = delete;

    static DJIMotor* Create(const DJIMotorConfig_s& config) {
        auto motor = new DJIMotor(config);

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

        switch (config.type) {
            case DJI_GM6020:
                if (motor->motor_id > 0 && motor->motor_id < 5)
                    motor->fdcan_member = bus->MemberRegister(0x1FE, motor->motor_id + 0x204, config.hfdcan,
                                                                motor, DJIMotorDecode);
                else if (motor->motor_id > 4 && motor->motor_id < 8) {
                    motor->fdcan_member = bus->MemberRegister(0x2FE, motor->motor_id + 0x204, config.hfdcan,
                                                                motor, DJIMotorDecode);
                }
                break;
            case DJI_M3508:
            case DJI_M2006:
                if (motor->motor_id > 0 && motor->motor_id < 5)
                    motor->fdcan_member = bus->MemberRegister(0x200, motor->motor_id + 0x200, config.hfdcan,
                                                                motor, DJIMotorDecode);
                else if (motor->motor_id > 4 && motor->motor_id < 8) {
                    motor->fdcan_member = bus->MemberRegister(0x1FF, motor->motor_id + 0x200, config.hfdcan,
                                                                motor, DJIMotorDecode);
                }
                break;
            default:
                delete motor;
                return nullptr;
        }
        motor->reduction_ratio = config.reduction_ratio;
        if (motor->fdcan_member == nullptr) {
            delete motor;
            return nullptr;
        }

        return motor;
    };

    ~DJIMotor() {
        if (fdcan_member) {
            fdcan_member = nullptr;
        }
        delete angle_pid;
        delete velocity_pid;
    };

    DJIMotorType_e type{DJI_GM6020};
    uint8_t motor_id{0};
    DJIMotorFeedback_s feedback{};

    FDCANMember* fdcan_member{nullptr};
    Pid* angle_pid{nullptr};
    Pid* velocity_pid{nullptr};

    float reduction_ratio{1}; //电机减速比
    float out_position{0.0f}; //电机输出轴角度（-PI~PI）
    float out_velocity{0.0f}; //电机输出轴速度（rpm）
    int16_t output{0};        //电机控制输出值

    void Ctrl(int16_t target_current);
    void Ctrl(float target_velocity);
    void Ctrl(float max_velocity, float target_position);

private:
    explicit DJIMotor(DJIMotorConfig_s config)
        : type(config.type),
          motor_id(config.id),
          angle_pid(new Pid(config.angle_pid_param)),
          velocity_pid(new Pid(config.velocity_pid_param)) {
    };
};

// DJI电机需要四个电机一组控制
class alignas(4) DJIMotorGroup {
private:
    DJIMotorGroup() = default;
public:
    /**
     * @brief 根据传入的电机配置参数创建DJI电机组
     * @warning 所有电机的种类和hfdcan都会以motor1为准！
     * @param motor1_config
     * @param motor2_config
     * @param motor3_config
     * @param motor4_config
     */
    static DJIMotorGroup* Create(const DJIMotorConfig_s* motor1_config,
                                 const DJIMotorConfig_s* motor2_config = nullptr,
                                 const DJIMotorConfig_s* motor3_config = nullptr,
                                 const DJIMotorConfig_s* motor4_config = nullptr) {
        auto group = new DJIMotorGroup;

        group->type = motor1_config->type;
        group->motors[group->motor_cnt] = DJIMotor::Create(*motor1_config);
        if (group->motors[group->motor_cnt] == nullptr) {
            delete group;
            return nullptr;
        }
        group->motor_cnt++;
        group->fdcan_member = group->motors[0]->fdcan_member;

        if (motor2_config != nullptr && motor2_config->type == group->type) {
            group->motors[group->motor_cnt] = DJIMotor::Create(*motor2_config);
            if (group->motors[group->motor_cnt] == nullptr) {
                for (int i = 0; i < group->motor_cnt; i++) {
                    delete group->motors[i];
                }
                delete group;
                return nullptr;
            }
            group->motor_cnt++;
        }
        if (motor3_config != nullptr && motor3_config->type == group->type) {
            group->motors[group->motor_cnt] = DJIMotor::Create(*motor3_config);
            if (group->motors[group->motor_cnt] == nullptr) {
                for (int i = 0; i < group->motor_cnt; i++) {
                    delete group->motors[i];
                }
                delete group;
                return nullptr;
            }
            group->motor_cnt++;
        }
        if (motor4_config != nullptr && motor4_config->type == group->type) {
            group->motors[group->motor_cnt] = DJIMotor::Create(*motor4_config);
            if (group->motors[group->motor_cnt] == nullptr) {
                for (int i = 0; i < group->motor_cnt; i++) {
                    delete group->motors[i];
                }
                delete group;
                return nullptr;
            }
            group->motor_cnt++;
        }

        return group;
    };

    DJIMotorType_e type{DJI_GM6020}; //电机类型
    FDCANMember* fdcan_member{nullptr};       // fdcan句柄

    uint8_t motor_cnt{0};
    DJIMotor* motors[4]{nullptr, nullptr, nullptr, nullptr};

    ~DJIMotorGroup() {
        for (int i = 0; i < motor_cnt; i++) {
            delete motors[i];
        }
        fdcan_member = nullptr; // 仅断开引用，FDCAN由成员自身注销
        motor_cnt = 0;
    }

    /**
     * @brief 发送该组所有注册电机的指令
     */
    void Transmit();
};

#endif //MOTOR_DJI_H
