## 针对H7适配的Lib

### 1. 概述
1. 本库使用使用CPP和C编写，如无使用CPP的必要，则使用C编写；
2. 设计思路同在C板F4上使用的库基本一致
3. 数学部分常用宏定义使用`arm_math.h`
4. BSP层的外设的Init需要在App层的Init函数在`main.c`调用
5. 所有的类/结构体均需要显式四字节对齐`alignas(4)`,保证DCache不出现问题（目前怀疑是在编译时压缩了）

### 2. RAM&Flash使用
1. ITCM/DTCM: 将算法任务中特别常用的全局变量放入此区域，对应的变量使用`__attribute__((section(".dtcm_data")))`进行修饰; 同时可以将自定义的Stack空间放在DTCM中，让临时变量得到更快的访问速度；
2. DOMAIN1_RAM：ld文件已将默认的所有变量和Task的堆栈空间放在此区域；

### 3. 开发状态

#### BSP
- [x] DWT
- [ ] CAN
- [ ] UART
- [ ]
#### Motor
- [ ] DJIMotor
- [ ] DMMotor
### Alg
- [x] 基本PID
- [ ] SMC
### Devices
- [ ] Dr16
- [ ] Et08a
- [ ] BMI088

## 4. 待测试项
- [ ] GM6020，M2006电机