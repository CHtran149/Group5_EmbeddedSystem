#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM


void Config_Led(){
	GPIO_InitTypeDef led;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	led.GPIO_Mode			= GPIO_Mode_Out_PP;
	led.GPIO_Pin			= GPIO_Pin_0;
	led.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
}

void Config_Timer(){
	TIM_TimeBaseInitTypeDef timer;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	timer.TIM_ClockDivision			= 0;
	timer.TIM_CounterMode				= TIM_CounterMode_Up;
	timer.TIM_Period						= 65535;
	timer.TIM_Prescaler					= 72 - 1;
	timer.TIM_RepetitionCounter		= 0;
	TIM_TimeBaseInit(TIM2, &timer);
}

void Delay_1ms(){
	TIM_Cmd(TIM2, ENABLE);
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < 1000);
	TIM_Cmd(TIM2, DISABLE);
}

void Delay_ms(unsigned int t){
	while(t--){
		Delay_1ms();
	}
}

int main(){
	Config_Led();
	Config_Timer();
	while(1){
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, 0);
		Delay_ms(500);
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, 1);
		Delay_ms(500);
	}
}
