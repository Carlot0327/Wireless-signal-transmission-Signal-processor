#ifndef __FFT_H
#define __FFT_H

#ifdef __cplusplus
extern "C" {
#endif

/* 1. 包含 HAL 库公共头文件 */
#include "stm32f4xx_hal.h"  

/* 2. 可选：包含需要用到的 HAL 外设头文件 */
#include "stdio.h"
#include "arm_math.h"  

/* 3. 对外暴露的句柄/结构体（如果需要在 main.c 或其他文件引用） */
#define FFT_LEN     256                                            // FFT 点数
#define WINDOW_LEN  FFT_LEN                                        // 汉宁窗长度	

typedef struct
{
	uint16_t*  adcBuf;
	
	float32_t  snr;
	float32_t  thd;
	float32_t  fftOutput[FFT_LEN];
	
	float32_t  fundamentalAmplitude;
	uint32_t   fundBin; 
	
}FFT_ParameterTypeDef;


/* 4. 用户 API 声明 -------------------------------------------------------*/
static void HanningWindow(void);
void FFT_Process(FFT_ParameterTypeDef* fftParam);

#ifdef __cplusplus
}
#endif
#endif /* __MY_PERIPHERAL_H */




