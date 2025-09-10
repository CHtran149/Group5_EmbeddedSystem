# MÔN HỆ THỐNG NHÚNG - Bài 4.1: Cấu hình Timer để tạo ngắt 500ms. Trong ISR của timer, đảo trạng thái của led LD2.


## Giới thiệu:

Đây là ví dụ sử dụng **STM32F103C8T6 (Blue Pill)** để điều khiển LED nhấp nháy bằng **ngắt Timer 2 (TIM2)**.  
Không cần dùng vòng lặp `Delay`, thay vào đó **TIM2 tạo ngắt định kỳ** và trong hàm xử lý ngắt sẽ đảo trạng thái LED.

---

## Các bước thực hiện:
### 1. Cấu hình chân GPIO.
```c
void Config_Led(){
	GPIO_InitTypeDef led;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	led.GPIO_Mode			= GPIO_Mode_Out_PP;
	led.GPIO_Pin			= GPIO_Pin_1;
	led.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
}
```
- Bật clock cho GPIOA.
- Cấu hình **PA1** là Output Push-Pull, tốc độ 50 MHz.
- LED được nối vào PA1.
  
### 2. Cấu hình Timer.
```c
void Config_Timer(){
	TIM_TimeBaseInitTypeDef timer;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	timer.TIM_ClockDivision 		= 0;
	timer.TIM_CounterMode				= TIM_CounterMode_Up;
	timer.TIM_Period						= 5000-1;
	timer.TIM_Prescaler					= 7200-1;
	timer.TIM_RepetitionCounter	=0;
	TIM_TimeBaseInit(TIM2, &timer);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM_Cmd(TIM2, ENABLE);
}
```
  - Bật clock cho TIM2.
  - Thiết lập bộ chia tần số:
    - `Prescaler = 7200 - 1`
    - `Period = 5000 - 1`
    - Với HCLK = 72 MHz ⇒ tần số Timer = 72 MHz / 7200 = 10 kHz ⇒ 1 tick = 0.1 ms.
    - Đếm từ 0 → 4999 = 5000 tick = **500 ms**.
  - Kích hoạt ngắt **Update Event (tràn bộ đếm)**.
  - Cho phép NVIC xử lý ngắt TIM2.
  - Bật TIM2.


 ### 3. Hàm Display().
 ```c
void Display(){
	static uint8_t state = 0;
	state = !state;
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, state);
}
```
- Biến `state` dạng static (0/1).
- Mỗi lần gọi sẽ **đảo trạng thái LED** (ON ↔ OFF). 

### 4. Hàm TIM2_IRQHandler().
```c
void TIM2_IRQHandler(){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		Display();
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}
```
- Kiểm tra cờ ngắt Update.
- Nếu có, gọi `Display()` để đổi trạng thái LED.
- Xóa cờ ngắt để sẵn sàng cho lần tiếp theo.
### 5. Hàm main().
```c
int main(){
	Config();
	Config_Timer();
	while(1){
		
	}
}
```
- Gọi `Config()` và `Config_Timer()`.
- Vòng lặp `while(1)` trống, vì LED được điều khiển hoàn toàn bằng **ngắt TIM2**.

## Chu kỳ LED
- Timer tràn mỗi 500 ms.
- Mỗi lần tràn ⇒ LED đổi trạng thái.
- Kết quả: **LED nhấp nháy với chu kỳ 1 giây (ON 0.5s, OFF 0.5s).**

---
## Video Mô Phỏng:
>



