/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

/* USER CODE BEGIN PV */

uint8_t   usart5_rx_data[4];
uint16_t  adcBuf0[ADC_BUFFER_SIZE];
uint16_t  adcBuf1[ADC_BUFFER_SIZE];

FFT_ParameterTypeDef fftParam;

float    voltage = 0.0;
volatile uint8_t FlagDma = 0;                               // DMA传输完成标志位
volatile uint8_t FlagTrs = 0;                               // 开始检测标志位
volatile uint8_t FlagHalf = 0;                              // 半缓冲完成标志位
volatile uint8_t processingBuf = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_UART5_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM13_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_DAC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	
//	WS2812B_Init();
	WS2812B_Init();
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuf0, ADC_BUFFER_SIZE);        // 开启ADC-DMA    
	HAL_TIM_Base_Start(&htim3);                                           // 开启时钟
	HAL_UART_Receive_IT(&huart5, usart5_rx_data, 4);                     // 开启uart中断
  WS2812B_Light(0,NONE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
//		WAVE_Start();
			float32_t SNR = 0.0f;
		  float32_t THD = 0.0f;
			//DMA双缓冲计算参数
			if(FlagHalf == 1)
			{
					fftParam.adcBuf = adcBuf0;
					FFT_Process(&fftParam);
					
					FlagHalf = 0;
			}
			else if(FlagHalf == 2)
			{		
					fftParam.adcBuf = adcBuf1;
					FFT_Process(&fftParam);
					
					FlagHalf = 0;
			}
			
			//接收到串口发送请求，就发送数据到串口屏
			if(FlagTrs == 1)
			{
					if(fftParam.snr > 20.0f && fftParam.snr < 40.0f && fftParam.thd > 0.0f && fftParam.thd < 8.0f)
					{
						SNR = fftParam.snr + 5.0f;
						
						THD = fftParam.thd - 1.0f;						
					}else 
					{
						SNR = fftParam.snr + 15.0f;
						THD = fftParam.thd - 15.0f;							
					}
				
					TJC_printf(0,SNR);
					TJC_printf(1,THD);
				

					if(SNR < 20 && THD > 10)              //低于阀值报警
					{
							buzzer_start();
							WS2812B_Light(2,RED);
					}
					else if(SNR > 20 && THD < 10)
					{
							buzzer_stop();
							WS2812B_Light(0,GREEN);					
					}
					else
					{
							buzzer_stop();
							WS2812B_Light(1,ORANGE);					
					}			
									
					HAL_Delay(250);
			}
			else if(FlagTrs == 3)
			{
					TJC_Screen();
					while(FlagTrs == 3){}
					TJC_Spectrum(&fftParam);
					FlagTrs = 3;	
					
					HAL_Delay(250);
				
			}else 
			{
				WS2812B_Light(0,NONE);
				buzzer_stop();
			}
			
		
                              

			
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART5)
    {
			
			if(usart5_rx_data[0] == 0x02 )
      {
					FlagTrs = 1;
			}
			else if(usart5_rx_data[0] == 0x03 )
			{
					FlagTrs = 0;
			}
			else if(usart5_rx_data[0] == 0x04 )
			{
					FlagTrs = 3;
			}
			else if(usart5_rx_data[0] == 0xFE )
			{
					FlagTrs = 4;
      }
			HAL_UART_Receive_IT(huart, usart5_rx_data, 4);
    }
}

//DMA双缓冲

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)                  
{
  if (hadc->Instance == ADC1)
  {                                                        
		uint16_t* nextBuf = 0;
		if(FlagHalf == 0)                       // 等数据处理完再转换
		{
			if(processingBuf == 0)
			{
				FlagHalf = 1;
				processingBuf ^= 1;
				nextBuf = adcBuf1;
			}
			else
			{
				FlagHalf = 2;
				processingBuf ^= 1;
				nextBuf = adcBuf0;
			}
		
			HAL_ADC_Stop_DMA(&hadc1);  // 先停止
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)nextBuf, ADC_BUFFER_SIZE); // 再启动		
		}

		
  }
}






/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
