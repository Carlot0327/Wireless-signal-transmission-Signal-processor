#include "warnning.h"

static uint16_t ws2812b_buffer[WS2812B_LED_NUM * 24 + WS2812B_RST_PULSE] = {0};

void buzzer_start(void)
{
		HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);
	
}

void buzzer_stop(void)
{
		HAL_TIM_PWM_Stop(&htim13,TIM_CHANNEL_1);
	
}

void WS2812B_SetPixel(uint16_t num, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t index = num * 24;  // 第几个LED占的位置
    
    for (uint8_t i = 0; i < 8; i++) {
			if(g & (0x80 >> i))
			{
					ws2812b_buffer[index + i] = WS2812B_PULSE_1;
			}else{
					ws2812b_buffer[index + i] = WS2812B_PULSE_0;
			}
			
			if(r & (0x80 >> i))
			{
					ws2812b_buffer[index + 8 + i] = WS2812B_PULSE_1;
			}else{
					ws2812b_buffer[index + 8 + i] = WS2812B_PULSE_0;
			}
			
			if(b & (0x80 >> i))
			{
					ws2812b_buffer[index + 16 + i] = WS2812B_PULSE_1;
			}else{
					ws2812b_buffer[index + 16 + i] = WS2812B_PULSE_0;
			}
			
    }
}

void WS2812B_Update(void)
{
    // 停止DMA传输
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_2);
	
//	  memset(ws2812b_buffer, 0, sizeof(ws2812b_buffer));
    
    // 重新开始DMA传输
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)ws2812b_buffer, sizeof(ws2812b_buffer)/sizeof(uint16_t));
}

void WS2812B_SetPixelRGB(uint16_t num, uint32_t rgb)
{
    WS2812B_SetPixel(num, 
                     (rgb >> 16) & 0xFF,  // R
                     (rgb >> 8) & 0xFF,   // G
                     (rgb >> 0) & 0xFF);  // B
}

void WS2812B_Init(void)
{
    memset(ws2812b_buffer, 0, sizeof(ws2812b_buffer));
    WS2812B_Update();
}

void WS2812B_Debug_Test(void)
{
	
//		static uint16_t data[] = {
//				WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,
//				WS2812B_PULSE_1,WS2812B_PULSE_1,WS2812B_PULSE_1,WS2812B_PULSE_1,WS2812B_PULSE_1,WS2812B_PULSE_1,WS2812B_PULSE_1,WS2812B_PULSE_1,
//				WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,WS2812B_PULSE_0,
//				WS2812B_RST_PULSE
//		};
		
		HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_2);
		memset(ws2812b_buffer, 0, sizeof(ws2812b_buffer));
		ws2812b_buffer[0] = 120;
		ws2812b_buffer[1] = 100;
		ws2812b_buffer[2] = 120;
		ws2812b_buffer[3] = 40;		
		
    // 修正传输数量计算
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)ws2812b_buffer, sizeof(ws2812b_buffer)/sizeof(uint16_t));
	
	
	
}


