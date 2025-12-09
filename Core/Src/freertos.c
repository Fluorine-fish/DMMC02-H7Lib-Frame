/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint32_t user_stack[ 2048 ];
/* USER CODE END Variables */
/* Definitions for DebugTask */
osThreadId_t DebugTaskHandle;
const osThreadAttr_t DebugTask_attributes = {
  .name = "DebugTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for RobiticsAlgTask */
osThreadId_t RobiticsAlgTaskHandle;
uint32_t RobiticsAlgTaskBuffer[ 2048 ];
osStaticThreadDef_t RobiticsAlgTaskControlBlock;
const osThreadAttr_t RobiticsAlgTask_attributes = {
  .name = "RobiticsAlgTask",
  .cb_mem = &RobiticsAlgTaskControlBlock,
  .cb_size = sizeof(RobiticsAlgTaskControlBlock),
  .stack_mem = &RobiticsAlgTaskBuffer[0],
  .stack_size = sizeof(RobiticsAlgTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal7,
};
/* Definitions for Chassis_Task */
osThreadId_t Chassis_TaskHandle;
const osThreadAttr_t Chassis_Task_attributes = {
  .name = "Chassis_Task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void App_DebugTask(void *argument);
extern void App_RobiticsAlgTask(void *argument);
extern void App_ChassisTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  osThreadAttr_t RobiticsAlgTask_attributes = {
    .name = "RobiticsAlgTask",
    .cb_mem = &RobiticsAlgTaskControlBlock,
    .cb_size = sizeof(RobiticsAlgTaskControlBlock),
    .stack_mem = &user_stack[0],
    .stack_size = sizeof(RobiticsAlgTaskBuffer),
    .priority = (osPriority_t) osPriorityAboveNormal1,
  };
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DebugTask */
  DebugTaskHandle = osThreadNew(App_DebugTask, NULL, &DebugTask_attributes);

  /* creation of RobiticsAlgTask */
  RobiticsAlgTaskHandle = osThreadNew(App_RobiticsAlgTask, NULL, &RobiticsAlgTask_attributes);

  /* creation of Chassis_Task */
  Chassis_TaskHandle = osThreadNew(App_ChassisTask, NULL, &Chassis_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_App_DebugTask */
/**
  * @brief  Function implementing the DebugTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_App_DebugTask */
__weak void App_DebugTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN App_DebugTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END App_DebugTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

