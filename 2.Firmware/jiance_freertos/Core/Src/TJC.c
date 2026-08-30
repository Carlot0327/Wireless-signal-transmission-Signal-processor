#include "TJC.h"

char TJC_Buf[32];
const uint8_t tail[3] = {0xFF, 0xFF, 0xFF};
uint16_t spectrum[256];

void TJC_printf(uint8_t x, float32_t value)
{
    int integer_part = (int)value;
    float decimal_f = (value - integer_part) * 100.0f;
    int decimal_part = (int)decimal_f;
    if (decimal_part < 0)
		{			
			decimal_part = -decimal_part;
		}
    int32_t result = integer_part * 100 + decimal_part;
    
		int len = snprintf(TJC_Buf, sizeof(TJC_Buf), "x%d.val=%d", x, (int)result);
    HAL_UART_Transmit(&huart5, (uint8_t *)TJC_Buf, len, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart5, tail, 3, HAL_MAX_DELAY);
		
}
//FFT_ParameterTypeDef* fftParam
void TJC_Screen(void)
{
	int len = snprintf(TJC_Buf, sizeof(TJC_Buf), "addt s0.id,0,256");
	HAL_UART_Transmit(&huart5, (uint8_t *)TJC_Buf, len, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart5, tail, 3, HAL_MAX_DELAY);

}

void TJC_Spectrum(FFT_ParameterTypeDef* fftParam)
{
	float32_t maxVal = 0.0f;
	float32_t minVal = 4096.0f;
	float32_t sample = 0.0f;
	
	static float s_minValDamped = 0.0f;
  static float s_maxValDamped = 230.0f;
  const float  DAMPING_FACTOR = 0.3f;  // 0-1之间，越大越平滑	
	
	for(uint8_t i = 0;i < FFT_LEN / 2;i++)
	{
		if(fftParam->fftOutput[i] > maxVal)
		{
			maxVal = fftParam->fftOutput[i];
		}
		if(fftParam->fftOutput[i] < minVal)
		{
			minVal = fftParam->fftOutput[i];
		}
	}
	
  s_minValDamped = s_minValDamped * (1 - DAMPING_FACTOR) + minVal * DAMPING_FACTOR;
  s_maxValDamped = s_maxValDamped * (1 - DAMPING_FACTOR) + maxVal * DAMPING_FACTOR;
	
	for(uint8_t i = 0;i < FFT_LEN / 2;i++)
	{
		sample = (fftParam->fftOutput[i] - s_minValDamped) / (s_maxValDamped - s_minValDamped);
		if(sample > 1){sample = 1;}
		if(sample < 0){sample = 0;}
		spectrum[2 * i] = (uint8_t)(sample * 230.0f);
		spectrum[2 * i + 1] = (uint8_t)(sample * 230.0f);
	}
	
	HAL_UART_Transmit(&huart5, (uint8_t *)spectrum, 256, HAL_MAX_DELAY);
}



