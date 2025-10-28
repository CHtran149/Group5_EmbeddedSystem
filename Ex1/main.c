#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC

void Config_Led(){
	GPIO_InitTypeDef led;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	led.GPIO_Mode 	= GPIO_Mode_Out_PP;
	led.GPIO_Pin 		= GPIO_Pin_13;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &led);
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
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		Delay_ms(100);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		Delay_ms(100);
	}
}