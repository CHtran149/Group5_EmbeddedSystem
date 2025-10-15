#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "queue.h"                      // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core

TaskHandle_t Task_Led;
TaskHandle_t Task_Ctrl;
QueueHandle_t Queue01;

void Task_Led_Funct(void * argument);
void Task_Ctrl_Funct(void * argument);
void Config_Led(void);

int main(){
	Config_Led();
	
	Queue01 = xQueueCreate(1, sizeof(uint32_t));
	xTaskCreate(Task_Led_Funct, "LED", 128, NULL, 1, NULL);
	xTaskCreate(Task_Ctrl_Funct, "CTRL", 128, NULL, 2, NULL);
	vTaskStartScheduler();
	while(1){
	}
}

void Config_Led(void){
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio.GPIO_Mode		= GPIO_Mode_Out_PP;
	gpio.GPIO_Pin			= GPIO_Pin_0;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &gpio);
}

void Task_Led_Funct(void * argument){
	uint32_t delay_time = 500;
	uint32_t recv_time;
	while(1){
		if(xQueueReceive(Queue01, &recv_time, 0) == pdPASS){
			delay_time = recv_time;
		}
		GPIOA->ODR ^= GPIO_Pin_0;
		vTaskDelay(pdMS_TO_TICKS(delay_time));
	}
}
void Task_Ctrl_Funct(void * argument){
	uint32_t max = 100;
	uint32_t min = 500;
	uint8_t check = 0;
	while(1){
		vTaskDelay(pdMS_TO_TICKS(5000));
		if(check == 0){
			xQueueOverwrite(Queue01, &max);
			check = 1;
		}
		else{
			xQueueOverwrite(Queue01, &min);
			check = 0;
		}
	}
}
