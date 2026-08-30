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
#include "stdio.h"
#include "arm_math.h"  
#include "TJC.h"
#include "FFT.h"
#include "warnning.h"
#include "Wave.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUFFER_SIZE 256                     // ADC采样点数                   
#define VREF 3.3f                               // 基准电压
#define DataREF 4095.0f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t   usart5_flag;
uint8_t   usart5_len = 0;
uint8_t   usart5_data;
uint8_t   usart5_databuf[4];		

uint16_t  adcBuf0[ADC_BUFFER_SIZE];
uint16_t  adcBuf1[ADC_BUFFER_SIZE];

FFT_ParameterTypeDef fftParam;

float    voltage = 0.0;
volatile uint8_t FlagDma = 0;                               // DMA传输完成标志位
volatile uint8_t FlagTrs = 0;                               // 开始检测标志位
volatile uint8_t FlagHalf = 0;                              // 半缓冲完成标志位
volatile uint8_t processingBuf = 0;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Warnning */
osThreadId_t WarnningHandle;
const osThreadAttr_t Warnning_attributes = {
  .name = "Warnning",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myUartQueue */
osMessageQueueId_t myUartQueueHandle;
const osMessageQueueAttr_t myUartQueue_attributes = {
  .name = "myUartQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartWarnningTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
//		WS2812B_Init();

//		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuf0, ADC_BUFFER_SIZE);        // 开启ADC-DMA    
//		HAL_TIM_Base_Start(&htim3);                                           // 开启时钟
//		HAL_UART_Receive_IT(&huart5, &usart5_data, 1);                     // 开启uart中断
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

  /* Create the queue(s) */
  /* creation of myUartQueue */
  myUartQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &myUartQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Warnning */
  WarnningHandle = osThreadNew(StartWarnningTask, NULL, &Warnning_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

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
	uint8_t   data = 1;
	
	HAL_UART_Receive_IT(&huart5, &usart5_data, 1);
  /* Infinite loop */
  for(;;)
  {
		if(usart5_flag == 1)
		{
			if(usart5_databuf[0] == 0x01)
			{
//				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
				HAL_UART_Transmit(&huart5, usart5_databuf, 4, HAL_MAX_DELAY);
				usart5_flag = 0;
			}
		}
		osDelay(5);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartWarnningTask */
/**
* @brief Function implementing the Warnning thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWarnningTask */
void StartWarnningTask(void *argument)
{
  /* USER CODE BEGIN StartWarnningTask */
	
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    osDelay(1000);
  }
  /* USER CODE END StartWarnningTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART5)
    {
			if (huart->Instance == UART5)
			{
				usart5_databuf[usart5_len] = usart5_data;
				usart5_len++;
				if(usart5_len == 4)
				{
					usart5_flag = 1;
					usart5_len = 0;
				}
				HAL_UART_Receive_IT(&huart5, &usart5_data, 1);
			}
    }
		
}









/* USER CODE END Application */

