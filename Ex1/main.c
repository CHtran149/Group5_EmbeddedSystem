#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC


void Config_Led(){
	
	GPIO_InitTypeDef led;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	led.GPIO_Mode 	= GPIO_Mode_Out_PP;
	led.GPIO_Pin 		= GPIO_Pin_1 | GPIO_Pin_2;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &led);
}

void Delay_ms(unsigned int t){
	int i, j;
	for(i=0; i<t; i++){
		for(j = 0; j<0x2aff; j++);
	}
}


int main(){

	Config_Led();
	
	while(1){
		GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
		Delay_ms(1000);
		GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
		Delay_ms(1000);
	}
}