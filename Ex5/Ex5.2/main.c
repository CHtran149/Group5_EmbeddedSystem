#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "string.h"
#include "stdio.h"

char ONLED[] = "ON";
char OFFLED[] = "OFF";

char pcr;
uint16_t stt;
uint16_t cnt = 0;
char res[100];

void Config_GPIO(){
	GPIO_InitTypeDef gpio;
	GPIO_InitTypeDef uart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	gpio.GPIO_Mode			= GPIO_Mode_Out_PP;
	gpio.GPIO_Pin				= GPIO_Pin_0;
	gpio.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	uart.GPIO_Mode 			= GPIO_Mode_AF_PP;
	uart.GPIO_Pin				= GPIO_Pin_9;
	uart.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &uart);
	
	uart.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
	uart.GPIO_Pin				= GPIO_Pin_10;
	GPIO_Init(GPIOA, &uart);
}

void Config_Uart(){
	USART_InitTypeDef usart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	usart.USART_BaudRate				= 9600;
	usart.USART_HardwareFlowControl			= USART_HardwareFlowControl_None;
	usart.USART_Mode						= USART_Mode_Rx;
	usart.USART_Parity					= USART_Parity_No;
	usart.USART_StopBits				= USART_StopBits_1;
	usart.USART_WordLength			= USART_WordLength_8b;
	
	USART_Init(USART1, &usart);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);// Bat ngat thanh ghi nhan du lieu khong trong.
	NVIC_EnableIRQ(USART1_IRQn); // Bat ngat USART trong NVIC.
	USART_Cmd(USART1, ENABLE);//Bat thiet bi ngoai vi UART duoc chi dinh.
}


uint16_t UARTx_Getc(USART_TypeDef* USARTx){
	return USART_ReceiveData(USARTx);
}

void USART1_IRQHandler(){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){ //neu thanh ghi RXNE khong trong (1) thi thuc hien ngat.
		pcr = UARTx_Getc(USART1);
		if(pcr == '\r'){
			stt = 1;
			res[cnt] = '\0';
			cnt = 0;
		}
		else{
			res[cnt] = pcr;
			cnt++;
		}
	}
}

int main(){
	Config_GPIO();
	Config_Uart();
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
	while(1){
		if(stt){
			if(strstr(res, ONLED) != NULL){ //hàm strstr tìm con tro dau tien den chuoi con trong chuoi lon.
				GPIO_ResetBits(GPIOA, GPIO_Pin_0);
			}
			else if(strstr(res, OFFLED) != NULL){
				GPIO_SetBits(GPIOA, GPIO_Pin_0);
			}
			stt = 0;
		}
	}
}