#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

void Config(){
	GPIO_InitTypeDef led;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	led.GPIO_Mode		= GPIO_Mode_Out_PP;
	led.GPIO_Pin		= GPIO_Pin_1;
	led.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
}

void Config_Timer(){
	TIM_TimeBaseInitTypeDef timer;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	timer.TIM_ClockDivision 		= 0;
	timer.TIM_CounterMode				= TIM_CounterMode_Up;
	timer.TIM_Period						= 5000-1;
	timer.TIM_Prescaler					= 7200-1;
	timer.TIM_RepetitionCounter	=0;
	TIM_TimeBaseInit(TIM2, &timer);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM_Cmd(TIM2, ENABLE);
}

void Display(){
	static uint8_t state = 0;
	state = !state;
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, state);
}

void TIM2_IRQHandler(){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		Display();
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

int main(){
	Config();
	Config_Timer();
	while(1){
		
	}
}