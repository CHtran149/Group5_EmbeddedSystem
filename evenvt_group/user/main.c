#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config

EventGroupHandle_t xEventGroup;

#define BIT_LED1   (1 << 0)
#define BIT_LED2   (1 << 1)
#define BIT_LED3   (1 << 2)
#define configUSE_EVENT_GROUPS     1


void Main_Task(void *argument);
void LED1_Task(void *argument);
void LED2_Task(void *argument);
void LED3_Task(void *argument);
void GPIO_Config(void);

int main(void)
{
    GPIO_Config();
    xEventGroup = xEventGroupCreate();
    if (xEventGroup == NULL) while(1); // kiểm tra lỗi

    xTaskCreate(Main_Task, "Main", 256, NULL, 3, NULL);
    xTaskCreate(LED1_Task, "LED1", 256, NULL, 2, NULL);
    xTaskCreate(LED2_Task, "LED2", 256, NULL, 2, NULL);
    xTaskCreate(LED3_Task, "LED3", 256, NULL, 2, NULL);

    vTaskStartScheduler();
    while(1);
}

void GPIO_Config(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);
}

void Main_Task(void *argument)
{
    while(1)
    {
        xEventGroupSetBits(xEventGroup, BIT_LED1);
        vTaskDelay(pdMS_TO_TICKS(1000));

        xEventGroupSetBits(xEventGroup, BIT_LED2);
        vTaskDelay(pdMS_TO_TICKS(1000));

        xEventGroupSetBits(xEventGroup, BIT_LED3);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void LED1_Task(void *argument)
{
    EventBits_t uxBits;
    while(1)
    {
        uxBits = xEventGroupWaitBits(xEventGroup, BIT_LED1, pdTRUE, pdFALSE, portMAX_DELAY);
        if(uxBits & BIT_LED1)
        {
            GPIOA->ODR ^= GPIO_Pin_0;
            vTaskDelay(pdMS_TO_TICKS(300));
        }
    }
}

void LED2_Task(void *argument)
{
    EventBits_t uxBits;
    while(1)
    {
        uxBits = xEventGroupWaitBits(xEventGroup, BIT_LED2, pdTRUE, pdFALSE, portMAX_DELAY);
        if(uxBits & BIT_LED2)
        {
            GPIOA->ODR ^= GPIO_Pin_1;
            vTaskDelay(pdMS_TO_TICKS(300));
        }
    }
}

void LED3_Task(void *argument)
{
    EventBits_t uxBits;
    while(1)
    {
        uxBits = xEventGroupWaitBits(xEventGroup, BIT_LED3, pdTRUE, pdFALSE, portMAX_DELAY);
        if(uxBits & BIT_LED3)
        {
            GPIOA->ODR ^= GPIO_Pin_2;
            vTaskDelay(pdMS_TO_TICKS(300));
        }
    }
}
