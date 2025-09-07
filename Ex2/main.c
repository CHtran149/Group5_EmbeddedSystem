#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC


void Config_GPIO(){
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	led.GPIO_Mode 	= GPIO_Mode_Out_PP;
	led.GPIO_Pin 		= GPIO_Pin_1;
	led.GPIO_Speed	= GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOA, &led);
	
	button.GPIO_Mode	= GPIO_Mode_IPU;
	button.GPIO_Pin		= GPIO_Pin_2;
	button.GPIO_Speed	= GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOA, &button);
	
}

int main(){
	Config_GPIO();
	while(1){
		uint8_t status = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0){
			GPIO_WriteBit(GPIOA, GPIO_Pin_1, !status);
		}
	}
}