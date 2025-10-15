#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core

TaskHandle_t Task01;
TaskHandle_t Task02;

void Task01Funct(void * argument);
void Task02Funct(void * argument);
void Config_GPIO();

int main(){
	Config_GPIO();
	xTaskCreate(Task01Funct, "Task01", 128, NULL, 2, &Task01);
	xTaskCreate(Task02Funct, "Task02", 128, NULL, 1, &Task02);
	vTaskStartScheduler();
	while(1){
		
	}
}

void Config_GPIO(void){
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio.GPIO_Mode		= GPIO_Mode_Out_PP;
	gpio.GPIO_Pin			= GPIO_Pin_0;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
}

void Task01Funct(void * argument){
	uint32_t check = 0;
	while(1){
		GPIOA->ODR ^= GPIO_Pin_0;
		vTaskDelay(250);
		check += 250;
		if(check >= 5000){
			check = 0;
			vTaskPrioritySet(Task02, 2);
			vTaskPrioritySet(Task01, 1);
		}
	}
}

void Task02Funct(void * argument){
	uint32_t check = 0;
	while(1){
		GPIOA->ODR ^= GPIO_Pin_0;
		vTaskDelay(100);
		check += 100;
		if(check >= 10000){
			check = 0;
			vTaskPrioritySet(Task02, 1);
			vTaskPrioritySet(Task01, 2);
		}
	}
}
