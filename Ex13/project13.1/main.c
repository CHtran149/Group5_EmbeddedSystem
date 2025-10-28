#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "queue.h"                      // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

//TaskHandle_t Task_Led;
//TaskHandle_t Task_Ctrl;
SemaphoreHandle_t Sema_Btn;
void Task_Led_Funct(void * argument);
void Task_Ctrl_Funct(void * argument);
void Config_EXTI(void);
void Config_Led(void);

int main(){
	Config_Led();
	Config_EXTI();
	
	Sema_Btn			= xSemaphoreCreateBinary();
	
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
	led.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_2;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
	GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_2);
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
	nvic.NVIC_IRQChannelPreemptionPriority = 3;
	nvic.NVIC_IRQChannelSubPriority	= 0;
	nvic.NVIC_IRQChannelCmd	= ENABLE;
	NVIC_Init(&nvic);
}

void EXTI1_IRQHandler(void){
	BaseType_t flag_check =pdFALSE;
	if(EXTI_GetITStatus(EXTI_Line1) != RESET){
		xSemaphoreGiveFromISR(Sema_Btn, &flag_check);
		EXTI_ClearITPendingBit(EXTI_Line1);
		portYIELD_FROM_ISR(flag_check); // gui schedule neu task co uu tien cao hon
	}
}

void Task_Led_Funct(void * argument){
	while(1){
		GPIOA->ODR ^= GPIO_Pin_0;
		vTaskDelay(500);
	}
}

void Task_Ctrl_Funct(void * argument){
	while(1){
		if(xSemaphoreTake(Sema_Btn, portMAX_DELAY) == pdTRUE){
			GPIO_ResetBits(GPIOA, GPIO_Pin_2);
			vTaskDelay(1000);
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
		}
	}
}
