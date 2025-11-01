#include "buzzer.h"

//=========================================================

static uint16_t prd;
static uint16_t prsclr;


void Buzzer_Init(uint32_t period, uint16_t prescaler)
{
	// Struct
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	prd = period-1;
	prsclr = prescaler-1;
	
	// Clock Timer
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//====================== Motor ======================
	
	// GPIO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// TIMER
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = prd;
	TIM_InitStruct.TIM_Prescaler = prsclr;
	TIM_InitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM1, &TIM_InitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
//	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
//	TIM_OC2Init(TIM1, &TIM_OCInitStruct);
//	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
//	TIM_OC3Init(TIM1, &TIM_OCInitStruct);
//	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
//	TIM_OC4Init(TIM1, &TIM_OCInitStruct);
//	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Buzzer_SetFrequency(uint32_t frequency)
{
	uint32_t timer_clock = 72000000 / (prsclr + 1);
	uint32_t period = (timer_clock / frequency) - 1;
	
	prd = (uint16_t)period;
	TIM1->ARR = prd;
}
void Buzzer_SetDuty(float percent)
{
    TIM_SetCompare1(TIM1, (uint16_t)(prd * percent / 100.0f));
}

void Buzzer_Start(void)
{
	TIM_Cmd(TIM1, ENABLE);
}

void Buzzer_Stop(void)
{
	TIM_Cmd(TIM1, DISABLE);
}
// buzzer.c
uint16_t Buzzer_GetARR(void)
{
    return prd;
}
