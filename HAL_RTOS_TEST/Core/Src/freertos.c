/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};
/* Definitions for myEvent01 */
osEventFlagsId_t myEvent01Handle;
const osEventFlagsAttr_t myEvent01_attributes = {
  .name = "myEvent01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
	#include "user_can_init.h"
	#include "chassis.h"
	#include "pid.h"
	#include "queue.h"
	#include "event_groups.h"

	extern ROBOT_CHASSIS ROBOT_CHASSI;


	#define KEY1_EVENT  (0x01 << 0)//设置事件掩码的位0
	#define KEY2_EVENT  (0x01 << 1)//设置事件掩码的位1
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of myEvent01 */
  myEvent01Handle = osEventFlagsNew(&myEvent01_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	User_Open_Can();
  
	PID_Parameter_Speed_Init(&M3508_PID[0],10.0,1.0,0.1 , 15000 ,5000 , 0 , 500);	//1号电机   
	PID_Parameter_Speed_Init(&M3508_PID[1],10.0,1.0,0.1 , 15000 ,5000 , 0 , 500);	//2号电机   
	PID_Parameter_Speed_Init(&M3508_PID[2],10.0,1.0,0.1 , 15000 ,5000 , 0 , 500);	//3号电机   
	PID_Parameter_Speed_Init(&M3508_PID[3],10.0,1.0,0.1 , 15000 ,5000 , 0 , 500);	//4号电机   
	
	ROBOT_CHASSI.SPEED.Robot_VX = 0;
	ROBOT_CHASSI.SPEED.Robot_VY = 0;
	ROBOT_CHASSI.SPEED.World_W = 0;

	
	EventBits_t r_event;
	uint16_t flag=0;
  /* Infinite loop */
  for(;;)
  {
	  r_event = xEventGroupWaitBits(myEvent01Handle,  /* 事件对象句柄 */
									  KEY1_EVENT|KEY2_EVENT,/* 接收线程感兴趣的事件 */
									  pdTRUE,   /* 退出时清除事件位 */
									  pdTRUE,   /* 满足感兴趣的所有事件 */
									  0);/* 指定超时事件,一直等 */
	  
	  if((r_event & (KEY1_EVENT|KEY2_EVENT)) == (KEY1_EVENT|KEY2_EVENT))
	  {
		flag=!flag;
	  }
	  
	  if(flag)
	  {
		ROBOT_CHASSI.SPEED.Robot_VX = 0;
		ROBOT_CHASSI.SPEED.Robot_VY = 0;
		ROBOT_CHASSI.SPEED.World_W = 1;
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	  }
	  else
	  {
		ROBOT_CHASSI.SPEED.Robot_VX = 0;
		ROBOT_CHASSI.SPEED.Robot_VY = 0;
		ROBOT_CHASSI.SPEED.World_W = 0;
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	  }
	  
	  Robot_Wheels_Adjust();
	  osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
	if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0)
	{
		osDelay(20);
		while(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0);
		xEventGroupSetBits(myEvent01Handle,KEY1_EVENT);
	}
	
	if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0)
	{
		osDelay(20); 
		while(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0);
		xEventGroupSetBits(myEvent01Handle,KEY2_EVENT);
	}
    osDelay(20);
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

