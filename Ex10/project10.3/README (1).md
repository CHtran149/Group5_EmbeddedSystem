# MÔN HỆ THỐNG NHÚNG - BÀI 10 : Sử dụng RTOS thực hiện tạo Task nhấp nháy led 
# Giới thiệu: 
Ví dụ này sử dụng STM32F103C8T6 VÀ STM32CUMEMX để :
- Tạo task nhấp nháy led theo tần số.
- Sử dụng 3 task với 3 tần số khác nhau.
# Các bước thực hiện :
# 1.hàm struct
``` 
typedef struct{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	uint32_t ms;
} LED_Config_t;
``` 
- Mục đích: gom thông tin cho 1 LED: cổng, chân, chu kỳ (ms).
- Lợi ích: tái sử dụng 1 hàm TaskBlink cho nhiều LED, code gọn, dễ mở rộng.
# 2. Hàm main 
```
HAL_Init();
SystemClock_Config();
MX_GPIO_Init();

xTaskCreate(StartDefaultTask, "DefaultTask", 128, NULL, 1, &defaultTaskHandle);
vTaskStartScheduler();
``` 
- HAL_Init() khởi HAL, SysTick mặc định (nếu CubeMX cấu hình).
- SystemClock_Config() đặt clock (ví dụ 72 MHz).
- MX_GPIO_Init() bật clock GPIO và cấu hình PA0..PA2 làm output.
- xTaskCreate(...) tạo một task khởi tạo (StartDefaultTask) với stack 128 (đơn vị stack words — xem chú ý phía dưới).
- vTaskStartScheduler() bật scheduler; sau lệnh này CPU do RTOS quản lý.
# 3. Hàm TaskBlink
``` 
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
```
- xLastWakeTime = xTaskGetTickCount() khởi tạo mốc thời gian để dùng với vTaskDelayUntil().
- Mỗi lần toggle → task block đúng led->ms tick, rồi lại toggle → chu kỳ nháy = 2 * led->ms.