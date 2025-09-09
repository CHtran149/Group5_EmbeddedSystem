# MÔN HỆ THỐNG NHÚNG - Bài 3: Điều khiển ngắt ngoài (EXTI)
Đề bài: Cấu hình ngắt ngoài cho một nút ấn, khi ấn nút trạng thái led đảo ngược. Trong khi đó, một led khác nhấp nháy với chu kì 1Hz.

## Các bước thực hiện:
### 1. Cấu hình chân GPIO.
```
void Config_GPIO(){
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	led.GPIO_Mode		= GPIO_Mode_Out_PP;
	led.GPIO_Pin		= GPIO_Pin_1 | GPIO_Pin_3;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
	
	button.GPIO_Mode 		= GPIO_Mode_IPU;
	button.GPIO_Pin			= GPIO_Pin_2;
	GPIO_Init(GPIOA, &button);
}
```
#### Giải thích:
- Cấu hình chân PA1 và PA3 điều khiển 2 led. Trong đó PA3 là led luôn nhấp nháy và PA1 là led đảo trạng thái.
- Cấu hình chân PA2 là nút nhấn ở chế độ Input Up.

### 2. 