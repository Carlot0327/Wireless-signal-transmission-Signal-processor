#include "Wave.h"

static uint16_t sineBuffer[SINE_SAMPLES];

void WAVE_GenerateSine(void)
{
    for (int i = 0; i < SINE_SAMPLES; i++) {
        float angle = 2.0f * PI * i / SINE_SAMPLES;
        sineBuffer[i] = (uint16_t)(2047.0f + 2047.0f * sinf(angle));
    }
}

void WAVE_Start(void)
{
    WAVE_GenerateSine();

    // 重算 TIM6 ARR 以得到精确频率
    uint32_t timerFreq = SINE_FREQ_HZ * SINE_SAMPLES;
    uint32_t arr = (TIM6_CLK_HZ / timerFreq) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim6, arr);
    __HAL_TIM_SET_COUNTER(&htim6, 0);

    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)sineBuffer, SINE_SAMPLES, DAC_ALIGN_12B_R);
    HAL_TIM_Base_Start(&htim6);
}