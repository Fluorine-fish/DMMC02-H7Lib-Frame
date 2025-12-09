/**
*   @file Alg_Pid.h
*   @brief 参考中科大的PID写法
*   @author Wenxin HU
*   @date 25-12-7
*   @version 1.0
*   @note
*/
#ifndef ALG_PID_H
#define ALG_PID_H

typedef enum {
    PID_DFIRST_ON = 1,
    PID_DFIRST_OFF = 0,
} DFirst_Enable_e;

typedef struct alignas(4){
    float kp;
    float ki;
    float kd;
    float i_out_max;
    float out_max;

    float i_separate_threshold{0.0f};
    float dead_zone{0.0f};
    float T{0.001f};
    DFirst_Enable_e dfirst_enable{PID_DFIRST_ON};
}PIDParam_s;

class alignas(4) Pid {
public:
    // 不允许空构造，必须填入PID基本参数
    Pid() = delete;

    // 使用结构体初始化
    explicit Pid(const PIDParam_s& config)
        : T(config.T),
          Dead_Zone(config.dead_zone),
          DFirst(config.dfirst_enable),
          Kp(config.kp),
          Ki(config.ki),
          Kd(config.kd),
          I_Out_Max(config.i_out_max),
          Out_Max(config.out_max),
          I_Separate_Threshold(config.i_separate_threshold) {};

    Pid(float _kp, float _ki, float _kd, float _i_Out_Max, float _out_Max,
        float _i_Separate_Threshold = 0.0f, float _dead_zone = 0.0f, float _t = 0.001f,
        DFirst_Enable_e _dFirst_Enable = PID_DFIRST_ON)
        : T(_t),
          Dead_Zone(_dead_zone),
          DFirst(_dFirst_Enable),
          Kp(_kp),
          Ki(_ki),
          Kd(_kd),
          I_Out_Max(_i_Out_Max),
          Out_Max(_out_Max),
          I_Separate_Threshold(_i_Separate_Threshold) {}
    float Get_Output() {return this->Output;}

    /**
     * @brief 清除PID积分和误差
     */
    void Clear() {
        this->Integral_Error = 0.0f;
        this->Error = 0.0f;
    }

    // 设置采样周期
    void Set_T(float _T) {this->T = _T;};

    float Calc(float target, float now);

protected:
    float T{0.001f};                       // 采样周期
    float Dead_Zone{0.0f};                 // 死区,Error绝对值在此区间内不输出
    DFirst_Enable_e DFirst{PID_DFIRST_ON}; //是否启用微分先行

    float Pre_Now{0.0f};
    float Pre_Target{0.0f};
    float Pre_Output{0.0f};
    float Pre_Error{0.0f};

    float P_Out{0.0f};
    float I_Out{0.0f};
    float D_Out{0.0f};
    float Output{0.0f};
    float Kp{0.0f};
    float Ki{0.0f};
    float Kd{0.0f};

    float I_Out_Max{0.0f};            //积分输出限幅，0为不限制
    float Out_Max{0.0f};              //总输出限幅，0为不限制
    float I_Separate_Threshold{0.0f}; //积分分离阈值，0为不启用积分分离

    float Target{0.0f};
    float Error{0.0f};
    float Now{0.0f};
    float Integral_Error{0.0f};
};

inline float clamp(float u, float min, float max) {
    if (u > max) return max;
    if (u < min) return min;
    return u;
}

#endif //ALG_PID_H
