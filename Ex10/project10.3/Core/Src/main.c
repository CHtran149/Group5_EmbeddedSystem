#include "main.h"
#include "cmsis_os.h"

typedef struct{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	uint32_t ms;
}LED_Config_t;

	TaskHandle_t defaultTaskHandle;
	LED_Config_t led1 = {GPIOA, GPIO_PIN_0, 167};
	LED_Config_t led2 = {GPIOA, GPIO_PIN_1, 50};
	LED_Config_t led3 = {GPIOA, GPIO_PIN_2, 20};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void * argument);

void TaskBlink(void *argument);

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
	
	xTaskCreate(StartDefaultTask, "DefaultTask", 128, NULL, 1,&defaultTaskHandle);
  vTaskStartScheduler();

  while (1)
  {
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_SET);

  /*Configure GPIO pins : PA0 PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void TaskBlink(void *argument)
{
    LED_Config_t *led = (LED_Config_t *)argument;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        HAL_GPIO_TogglePin(led->GPIOx, led->GPIO_Pin);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(led->ms));
    }
}

void StartDefaultTask(void * argument)
{
	xTaskCreate(TaskBlink, "LED1", 128, &led1, 1, NULL);
	xTaskCreate(TaskBlink, "LED2", 128, &led2, 1, NULL);
	xTaskCreate(TaskBlink, "LED3", 128, &led3, 1, NULL);
	vTaskDelete(NULL);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
