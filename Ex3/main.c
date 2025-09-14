#include "stm32f10x.h"                  // Device header
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC

void Config_GPIO(){
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	led.GPIO_Mode		= GPIO_Mode_Out_PP;
	led.GPIO_Pin		= GPIO_Pin_1 | GPIO_Pin_3;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
	
	button.GPIO_Mode 		= GPIO_Mode_IPU;
	button.GPIO_Pin			= GPIO_Pin_2;
	GPIO_Init(GPIOA, &button);
}

void Config_Nvic(){
	NVIC_InitTypeDef nvic;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	nvic.NVIC_IRQChannel = EXTI2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
}
void Config_Exti(){
	EXTI_InitTypeDef exti;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
	exti.EXTI_Line		= EXTI_Line2;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode		= EXTI_Mode_Interrupt;
	exti.EXTI_Trigger	= EXTI_Trigger_Falling;
	
	EXTI_Init(&exti);
}

void EXTI2_IRQHandler(){
	uint8_t led1_state = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0){
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, !led1_state);
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void Delay(unsigned int t){
	unsigned int i, j;
	for(i=0; i<t;i++){
		for(j=0;j<0x2aff;j++);
	}
}

int main(){
	Config_GPIO();
	Config_Nvic();
	Config_Exti();
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, 1);
	while(1){
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		Delay(500);
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
		Delay(500);
	}
}
