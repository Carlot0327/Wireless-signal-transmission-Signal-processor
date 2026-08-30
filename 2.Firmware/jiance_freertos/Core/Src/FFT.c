#include "FFT.h"

float32_t fftInput[FFT_LEN * 2];                                         // FFT输入（复数形式，需补齐至512点）
//float32_t fftOutput[FFT_LEN * 2];                                        // FFT输出

float32_t signalPower = 0.0f;
float32_t noisePower = 0.0f;
float32_t harmonicPower = 0.0f;
float32_t fundamentalPower = 0.0f;
float32_t fundamentalAmplitude = 0.0f;
uint32_t  fundBin = 0;                                            // 基波位置

static float32_t hanningWindow[WINDOW_LEN];                        //汉宁窗系数表
static uint8_t   FlagWindow = 0;

arm_cfft_radix4_instance_f32 fftInst;                            // FFT实例
//生成256点汉宁创
static void HanningWindow()
{
    if (FlagWindow) 
		{
			return;
		}
		else
		{
			for (uint16_t n = 0; n < WINDOW_LEN; n++)
			{
					hanningWindow[n] = 0.5f - 0.5f * cosf(2.0f * PI * n / (float32_t)WINDOW_LEN);
			}
			FlagWindow = 1;		
		}

}

//FFT变换，并计算参数
void FFT_Process(FFT_ParameterTypeDef* fftParam)
{
	HanningWindow();
	arm_cfft_radix4_init_f32(&fftInst, FFT_LEN, 0, 1);                   // 256点FFT、正向变换、使用位反转
	
	// 实时求直流偏置
	float32_t sum = 0.0f;
	for (uint16_t i = 0; i < FFT_LEN; i++) 
	{
			sum += (float32_t)fftParam->adcBuf[i];
	}
	float32_t mean = sum / FFT_LEN;  
	
	// 准备FFT输入数据
	for (uint16_t i = 0; i < FFT_LEN; i++)                               
	{
		float32_t sample = (float32_t)(fftParam->adcBuf[i] - mean) / 2048.0f;    
		fftInput[2*i] = sample * hanningWindow[i];                     // 加窗
		fftInput[2*i + 1] = 0.0f;                                      
	}
	
	// 执行FFT
	arm_cfft_radix4_f32(&fftInst, fftInput);                         
	arm_cmplx_mag_f32(fftInput, fftParam->fftOutput, FFT_LEN);                     
	                                                  
	arm_max_f32(fftParam->fftOutput, FFT_LEN / 2 - 1, &fundamentalAmplitude, &fundBin);    // 找基波
	
	// 计算基波功率
	fundamentalPower = 0;
	int startBin = (fundBin > 2) ? (fundBin - 2) : 1;
  int endBin   = (fundBin < (FFT_LEN/2 - 3)) ? (fundBin + 2) : (FFT_LEN/2 - 1);   //防止越界
	for (int i = startBin; i <= endBin; i++) 
	{
        fundamentalPower += fftParam->fftOutput[i] * fftParam->fftOutput[i];
  }
	
	// 计算总功率
	arm_power_f32(&fftParam->fftOutput[1], FFT_LEN / 2 - 1, &signalPower);                 
	
	// 计算谐波功率
	harmonicPower = 0;
	if(fundBin == 0)
	{
		harmonicPower = 0;                    //防止基波为0频时，谐波重复计算
	}
	else
	{
		for (uint8_t h = 2; h <= 5; h++)                 
		{
			uint16_t harBin = fundBin * h;
			if (harBin < FFT_LEN / 2) 
			{
				harmonicPower += fftParam->fftOutput[harBin] * fftParam->fftOutput[harBin];    
			}
		}
	}
																														 
	noisePower = signalPower - fundamentalPower - harmonicPower;         //求噪音
	if (noisePower < 0) noisePower = 0;
	
	fftParam->snr = 10.0f * log10f(fundamentalPower / noisePower);
	fftParam->thd = (sqrtf(harmonicPower) / sqrtf(fundamentalPower)) * 100.0f;   
	
}


