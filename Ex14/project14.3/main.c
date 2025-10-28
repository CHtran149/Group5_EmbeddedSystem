#include "stm32f10x.h"                  // Device header
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups

#define BIT_TASK_A (1 << 0)
#define BIT_TASK_B (1 << 1)
#define BIT_TASK_C (1 << 2)

EventGroupHandle_t xEventGroup;


void Config_UART(void);
void TaskCtrl_Funct(void * argument);
void TaskA_Funct(void * argument);
void TaskB_Funct(void * argument);
void TaskC_Funct(void * argument);



int main(){
	Config_UART();
	xEventGroup = xEventGroupCreate();
	xTaskCreate(TaskCtrl_Funct, "TaskCtrl", 128, NULL, 2, NULL);
	xTaskCreate(TaskA_Funct, "TaskA", 128, NULL, 1, NULL);
	xTaskCreate(TaskB_Funct, "TaskB", 128, NULL, 1, NULL);
	xTaskCreate(TaskC_Funct, "TaskC", 128, NULL, 1, NULL);

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

void TaskCtrl_Funct(void * argument){
	uint16_t cnt = 0;
	while(1){
		switch(cnt % 3){
			case 0:
				xEventGroupSetBits(xEventGroup, BIT_TASK_A);
				break;
			case 1:
				xEventGroupSetBits(xEventGroup, BIT_TASK_B);
				break;
			case 2:
				xEventGroupSetBits(xEventGroup, BIT_TASK_C);
				break;
		}
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void TaskA_Funct(void * argument){
	while(1){
		xEventGroupWaitBits(xEventGroup, BIT_TASK_A, pdTRUE, pdFALSE, portMAX_DELAY);
		uart_SendStr("TaskA is running\n");
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}


void TaskB_Funct(void * argument){
	while(1){
		xEventGroupWaitBits(xEventGroup, BIT_TASK_B, pdTRUE, pdFALSE, portMAX_DELAY);
		uart_SendStr("TaskB is running\n");
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}


void TaskC_Funct(void * argument){
	while(1){
		xEventGroupWaitBits(xEventGroup, BIT_TASK_C, pdTRUE, pdFALSE, portMAX_DELAY);
		uart_SendStr("TaskC is running\n");
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

