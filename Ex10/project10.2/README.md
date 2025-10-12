# MÔN HỆ THỐNG NHÚNG - BÀI 10 : Sử dụng RTOS thực hiện tạo Task nhấp nháy led 
# Giới thiệu: 
Ví dụ này sử dụng STM32F103C8T6 VÀ STM32CUMEMX để :
- Tạo task nhấp nháy led theo tần số.
- Sử dụng 3 task với 3 tần số khác nhau.

# Các bước thực hiện:
# 1. Hàm main 
``` 
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

	xTaskCreate(Task01Funct, "LED3Hz", 128, NULL, 1, &Task01Handle);
	xTaskCreate(Task02Funct, "LED10Hz", 128, NULL, 1, &Task02Handle);
	xTaskCreate(Task03Funct, "LED25Hz", 128, NULL, 1, &Task03Handle);

	vTaskStartScheduler();
  while (1)
  {
		
  }

}

```
- HAL_Init() khởi HAL (cấu hình NVIC priority grouping, tick timer nếu CubeMX cấu hình).
- SystemClock_Config() cấu hình clock hệ thống (ở code bạn: HSE + PLL×9 → 72 MHz).
- xTaskCreate() tạo task: tham số quan trọng là stack depth (ở đây 128), priority (1..3).
# 2. Các hàm task
``` 
void Task01Funct(void * argument)
{
  for(;;)
  {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
		vTaskDelay(167); // 3Hz
  }
}

void Task02Funct(void * argument)
{
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
		vTaskDelay(50); //10Hz
  }
}

void Task03Funct(void * argument)
{
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
		vTaskDelay(20); /// 25Hz
  }
}
```
- thực hiện các chức năng nháy led .
# 3. Hàm thiết lập xung điều khiển cho STM32
```
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

```
- Dùng thạch anh ngoài (HSE) 8 MHz.
- Nhân lên PLL ×9.
- → tạo ra 72 MHz làm xung chính cho CPU.
- cấu hình độ chia các bus với từng xung clock khác nhau.
  