#ifndef __TJC_H
#define __TJC_H

#ifdef __cplusplus
extern "C" {
#endif

/* 1. 包含 HAL 库公共头文件 */
#include "stm32f4xx_hal.h"  

/* 2. 可选：包含需要用到的 HAL 外设头文件 */
#include "usart.h"
#include "stdio.h"
#include "FFT.h"
#include "arm_math.h" 

/* 3. 对外暴露的句柄/结构体（如果需要在 main.c 或其他文件引用） */
//extern UART_HandleTypeDef huart2;

/* 4. 用户 API 声明 -------------------------------------------------------*/
void TJC_printf(uint8_t x, float32_t value);
void TJC_Screen(void);
void TJC_Spectrum(FFT_ParameterTypeDef* fftParam);

#ifdef __cplusplus
}
#endif
#endif /* __MY_PERIPHERAL_H */




