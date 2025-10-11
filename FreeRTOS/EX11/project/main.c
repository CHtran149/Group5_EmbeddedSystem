#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSconfig.h"
#include "timers.h"

void Delay(int t){
	int i, j;
	for(i=0; i<t; i++){
		for(j=0; j<0x2aff;j++);
	}
}

void LED_Init(void){
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_Out_PP;
	gpio.GPIO_Pin			= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
}

void vTaskLED1(void *pvParameters){
	for(;;){
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		vTaskDelay(pdMS_TO_TICKS(1000));
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void vTaskLED2(void *pvParameters){
	for(;;){
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		vTaskDelay(pdMS_TO_TICKS(3000));
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		vTaskDelay(pdMS_TO_TICKS(3000));
	}
}

int main(){
	BaseType_t status1, status2;
	LED_Init();
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	Delay(500);
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	Delay(500);
	SysTick_Config(SystemCoreClock / configTICK_RATE_HZ);
	status1 = xTaskCreate(vTaskLED1, "LED1", 256, NULL, 2, NULL);
	status2 = xTaskCreate(vTaskLED2, "LED2", 256, NULL, 1, NULL);

	if(status1 != pdPASS || status2 != pdPASS){
    while(1);
	}
	
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	Delay(500);
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	Delay(500);
	
	vTaskStartScheduler();
	
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	Delay(500);
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	Delay(500);
	while(1){
		
	}
}
