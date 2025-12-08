# FreeRTOS Note
1. 使用所有task入口函数必须注册在`App_TaskPublic.h`中，使用`extern  "C"`保护，防止无法编译
2. 除了DefaultTask之外，用户所有task必须使用外部定义，并且stack使用外部固定，同时修改freeRTOS.c文件让stack落在指定的内存区域，享受DTCM内存加速