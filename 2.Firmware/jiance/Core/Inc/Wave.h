#ifndef __WAVE_H
#define __WAVE_H

#ifdef __cplusplus
extern "C" {
#endif

/* 1. 包含 HAL 库公共头文件 */
#include "stm32f4xx_hal.h"  

/* 2. 可选：包含需要用到的 HAL 外设头文件 */
#include "dac.h"
#include "math.h"
#include "tim.h"
#include "string.h"
#include "stdint.h"

/* 3. 对外暴露的句柄/结构体（如果需要在 main.c 或其他文件引用） */
#define SINE_FREQ_HZ  1090     // 目标频率
#define SINE_SAMPLES  100      // 每周期点数
#define TIM6_CLK_HZ   84000000
#define PI            3.1415926535f


/* 4. 用户 API 声明 -------------------------------------------------------*/
void WAVE_GenerateSine(void);
void WAVE_Start(void);


#ifdef __cplusplus
}
#endif
#endif /* __MY_PERIPHERAL_H */