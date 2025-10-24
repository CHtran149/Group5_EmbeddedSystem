#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "string.h"
#include "stdio.h"

void Task01_Funct(void * argument);
void Task02_Funct(void * argument);
void Config_UART(void);


int main(){
	Config_UART();
	xTaskCreate(Task01_Funct, "Task01", 128, NULL, 1, NULL);
	xTaskCreate(Task02_Funct, "Task02", 128, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1){
		
	}
}

void Config_UART(void){
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef uart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_AF_PP;
	gpio.GPIO_Pin			= GPIO_Pin_9;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Mode		= GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin			= GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	uart.USART_BaudRate		= 9600;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode				= USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity			= USART_Parity_No;
	uart.USART_StopBits		= USART_StopBits_1;
	uart.USART_WordLength	= USART_WordLength_8b;
	USART_Init(USART1, &uart);
	USART_Cmd(USART1, ENABLE);
}

void USARTx_SendChar(char c){
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET); // thanh ghi du lieu truyen khong trong thi truyen.
	USART_SendData(USART1, c);
}
void uart_SendStr(char *str){
	while(*str != NULL){
		USARTx_SendChar(*str++);
	}
}


void Task01_Funct(void * argument){
	while(1){
		uart_SendStr("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
		vTaskDelay(100);
	}
}

void Task02_Funct(void * argument){
	while(1){
		uart_SendStr("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n");
		vTaskDelay(100);
	}
}

