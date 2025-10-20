#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "queue.h"                      // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

TaskHandle_t Task_Led;
TaskHandle_t Task_Ctrl;
SemaphoreHandle_t Sema_Btn;
QueueHandle_t Queue_Led;
void Task_Led_Funct(void * argument);
void Task_Ctrl_Funct(void * argument);
void Config_EXTI(void);
void Config_Led(void);

int main(){
	Config_Led();
	Config_EXTI();
	
	Sema_Btn			= xSemaphoreCreateBinary();
	Queue_Led			= xQueueCreate(1, sizeof(uint32_t));
	
	xTaskCreate(Task_Led_Funct, "Task_Led", 128, NULL, 1, NULL);
	xTaskCreate(Task_Ctrl_Funct, "Task_Ctrl", 128, NULL,2, NULL);
	
	vTaskStartScheduler();
	while(1){
		
	}
	
}

void Config_Led(void){
	GPIO_InitTypeDef led;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	led.GPIO_Mode		= GPIO_Mode_Out_PP;
	led.GPIO_Pin		= GPIO_Pin_0;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
}

void Config_EXTI(void){
	GPIO_InitTypeDef button;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	button.GPIO_Mode		= GPIO_Mode_IPU;
	button.GPIO_Pin			= GPIO_Pin_1;
	GPIO_Init(GPIOA, &button);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	exti.EXTI_Line		= EXTI_Line1;
	exti.EXTI_LineCmd	= ENABLE;
	exti.EXTI_Mode		= EXTI_Mode_Interrupt;
	exti.EXTI_Trigger	= EXTI_Trigger_Falling;
	EXTI_Init(&exti);
	
	nvic.NVIC_IRQChannel		= EXTI1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 5;
	nvic.NVIC_IRQChannelSubPriority	= 0;
	nvic.NVIC_IRQChannelCmd	= ENABLE;
	NVIC_Init(&nvic);
}

void EXTI1_IRQHandler(void){
	BaseType_t flag_check =pdFALSE;
	if(EXTI_GetITStatus(EXTI_Line1) != RESET){
		xSemaphoreGiveFromISR(Sema_Btn, &flag_check);
		EXTI_ClearITPendingBit(EXTI_Line1);
		portYIELD_FROM_ISR(flag_check); // g?i schedul? neu task co uu tien cao hon
	}
}

void Task_Led_Funct(void * argument){
	uint32_t delay_time = 500;
	uint32_t recv_time;
	while(1){
		if(xQueueReceive(Queue_Led, &recv_time, 0) == pdPASS){
			delay_time = recv_time;
		}
		GPIOA->ODR ^= GPIO_Pin_0;
		vTaskDelay(delay_time);
	}
}

void Task_Ctrl_Funct(void * argument){
	uint32_t fast = 100;
	uint32_t low  = 500;
	uint8_t state = 0;
	xQueueSend(Queue_Led, &low, portMAX_DELAY);
	while(1){
		if(xSemaphoreTake(Sema_Btn, portMAX_DELAY) == pdTRUE){
			if(state == 0){
				xQueueSend(Queue_Led, &fast, portMAX_DELAY);
				state = 1;
			}
			else{
				xQueueSend(Queue_Led, &low, portMAX_DELAY);
				state = 0;
			}
			vTaskDelay(200); //chong doi nut
		}
	}
}
