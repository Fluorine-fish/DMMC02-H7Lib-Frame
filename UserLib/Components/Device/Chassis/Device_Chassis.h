/**
*   @file Device_Chassis.h
*   @brief 底盘类
*   @author Wenxin HU
*   @date 25-12-9
*   @version 1.0
*   @note 目前使用底盘电机均为DJI电机
*/
#ifndef DEVICE_CHASSIS_H
#define DEVICE_CHASSIS_H

#include "Motor_dji.h"

typedef struct alignas(4) {
    float Vx{0.0f}; // 底盘X轴速度（m/s）
    float Vy{0.0f}; // 底盘Y轴速度（m/s）
    float Vw{0.0f}; // 底盘旋转角速度（m/s）
} ChassisVelocity_s;

typedef struct alignas(4) {
    float wheel_radius{0.0f}; // 底盘轮半径(m)
    float length_a{0.0f};     // 底盘前后半长度(m）
    float length_b{0.0f};     // 底盘左右半长度（m）
} MecanumChassisParam_s;

typedef struct alignas(4) {
    MecanumChassisParam_s chassis_param;
    DJIMotorConfig_s motor1_config;
    DJIMotorConfig_s motor2_config;
    DJIMotorConfig_s motor3_config;
    DJIMotorConfig_s motor4_config;
} MecanumChassisConfig_s;

class alignas(4) Chassis_Interface {
public:
    virtual ~Chassis_Interface() = default;
    virtual void Ik_Calc() = 0;
    virtual void Ctrl() = 0;
};

class alignas(4) MecanumChassis : Chassis_Interface {
private:
    MecanumChassis();

public:
    static MecanumChassis* Create(MecanumChassisConfig_s* config) {
        auto chassis = new MecanumChassis();
        chassis->chassis_param = config->chassis_param;

        chassis->motor_group = DJIMotorGroup::Create(&config->motor1_config,
                                                     &config->motor2_config,
                                                     &config->motor3_config,
                                                     &config->motor4_config);
        if (chassis->motor_group == nullptr) return nullptr;

        return chassis;
    };

    float wheel_velocity[4]{0.0f, 0.0f, 0.0f, 0.0f}; // 四轮速度

    MecanumChassisParam_s chassis_param{};
    ChassisVelocity_s chassis_velocity{};
    DJIMotorGroup* motor_group{nullptr};

    void Ik_Calc() override;

    void Ctrl() override;

    ~MecanumChassis() override {
        delete motor_group;
    }
};

typedef struct alignas(4) {
    float mecanum_radius{0.0f}; // 麦轮半径(m)
    float omni_radius{0.0f};    //全向轮半径(m)
    float length_a{0.0f};       // 底盘前后半长度(m）
    float length_b{0.0f};       // 底盘左右半长度（m）
} MoonChassisParam_s;

typedef struct alignas(4) {
    MoonChassisParam_s chassis_param;
    DJIMotorConfig_s mecanum_motor1_config;
    DJIMotorConfig_s mecanum_motor2_config;
    DJIMotorConfig_s mecanum_motor3_config;
    DJIMotorConfig_s mecanum_motor4_config;
    DJIMotorConfig_s omni_motor1_config;
    DJIMotorConfig_s omni_motor2_config;
} MoonChassisConfig_s;

typedef enum alignas(4) {
    MOONCHASSIS_FAST = 0,
    MOONCHASSIS_SLOW = 1,
    MOONCHASIIS_CLIMB = 2,
} MoonChassisState_e;

class alignas(4) MoonChassis : Chassis_Interface {
private:
    MoonChassis();

public:
    static MoonChassis* Create(MoonChassisConfig_s* config) {
        auto chassis = new MoonChassis();
        chassis->chassis_param = config->chassis_param;
        chassis->mecanum_motor = DJIMotorGroup::Create(&config->mecanum_motor1_config,
                                                       &config->mecanum_motor2_config,
                                                       &config->mecanum_motor3_config,
                                                       &config->mecanum_motor4_config);
        chassis->omni_motor = DJIMotorGroup::Create(&config->omni_motor1_config,
                                                    &config->omni_motor2_config);
        if (chassis->mecanum_motor == nullptr || chassis->omni_motor == nullptr) {
            delete chassis;
        };

        return chassis;
    };

    float mecanum_velocity[4]{0.0f, 0.0f, 0.0f, 0.0f}; // 四麦克纳姆轮速度
    float omni_velocity[2]{0.0f, 0.0f};                //悬挂全向轮速度

    MoonChassisParam_s chassis_param{};
    ChassisVelocity_s chassis_velocity{};
    DJIMotorGroup* mecanum_motor{nullptr};
    DJIMotorGroup* omni_motor{nullptr};

    ~MoonChassis() override {
        delete mecanum_motor;
        delete omni_motor;
    }

    void Ik_Calc() override;

    void Ctrl() override;
};

#endif //DEVICE_CHASSIS_H
