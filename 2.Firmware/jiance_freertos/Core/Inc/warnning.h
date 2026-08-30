#ifndef __WARNNING_H
#define __WARNNING_H

#ifdef __cplusplus
extern "C" {
#endif

/* 1. 包含 HAL 库公共头文件 */
#include "stm32f4xx_hal.h"  

/* 2. 可选：包含需要用到的 HAL 外设头文件 */
#include "tim.h"
#include "string.h"

/* 3. 对外暴露的句柄/结构体（如果需要在 main.c 或其他文件引用） */
#define WS2812B_LED_NUM     1      // 几个灯珠
#define WS2812B_PULSE_1     120      // 逻辑1高电平时间 ≈ 0.97μs
#define WS2812B_PULSE_0     40      // 逻辑0高电平时间 ≈ 0.49μs
#define WS2812B_RST_PULSE   300     // 复位脉冲数量（>300 = >50μs）
	
#define RED                 0xFF0000
#define ORANGE              0xFFA500
#define GREEN               0x008000
#define NONE                0x000000
	
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	
}RGB_ColorTypeDef;

/* 4. 用户 API 声明 -------------------------------------------------------*/

//蜂鸣器函数
void buzzer_start(void);
void buzzer_stop(void);

//rgb灯珠函数
void WS2812B_SetPixel(uint16_t num, uint8_t r, uint8_t g, uint8_t b);
void WS2812B_Update(void);
void WS2812B_SetPixelRGB(uint16_t num, uint32_t rgb);
void WS2812B_Init(void);
void WS2812B_Debug_Test(void);

#ifdef __cplusplus
}
#endif
#endif /* __MY_PERIPHERAL_H */




