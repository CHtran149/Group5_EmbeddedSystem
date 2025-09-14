# MÔN HỆ THỐNG NHÚNG - Bài 3: Cấu hình ngắt ngoài cho một nút ấn, khi ấn nút trạng thái led đảo ngược. Trong khi đó, một led khác nhấp nháy với chu kì 1Hz.

## Giới thiệu  
Chương trình này được viết cho **STM32F103C8T6** (dòng STM32F10x), minh họa cách:  
- Cấu hình **GPIO** để điều khiển LED và đọc nút nhấn.  
- Sử dụng **EXTI (External Interrupt)** để phát hiện sự kiện nhấn nút.  
- Cấu hình **NVIC (Nested Vectored Interrupt Controller)** để xử lý ngắt.  
- Điều khiển LED theo trạng thái nút nhấn và đồng thời nháy một LED khác trong vòng lặp chính.  

---

## Các bước thực hiện:
### 1. Cấu hình chân GPIO.
```c
void Config_GPIO(){
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	led.GPIO_Mode		= GPIO_Mode_Out_PP;
	led.GPIO_Pin		= GPIO_Pin_1 | GPIO_Pin_3;
	led.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
	
	button.GPIO_Mode 		= GPIO_Mode_IPU;
	button.GPIO_Pin			= GPIO_Pin_2;
	GPIO_Init(GPIOA, &button);
}
```

- **PA1, PA3**: cấu hình làm ngõ ra để điều khiển LED.
	- **PA1**: LED thay đổi trạng thái khi nhấn nút.
	- **PA3**: LED nhấp nháy trong vòng lặp main.
- **PA2**: cấu hình làm ngõ vào (Input Pull-up) để đọc trạng thái nút nhấn.

### 2. Cấu hình NVIC và EXTI 
```c
void Config_Nvic(){
	NVIC_InitTypeDef nvic;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	nvic.NVIC_IRQChannel = EXTI2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
}
void Config_Exti(){
	EXTI_InitTypeDef exti;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
	exti.EXTI_Line		= EXTI_Line2;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode		= EXTI_Mode_Interrupt;
	exti.EXTI_Trigger	= EXTI_Trigger_Falling;
	
	EXTI_Init(&exti);
}
``` 
- **NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0)**: chỉ dùng SubPriority, không có PreemptionPriority.
- **EXTI2_IRQn**: chọn ngắt ngoài trên line 2 (ứng với chân PA2).
- **EXTI_Trigger_Falling**: ngắt được kích hoạt khi nút nhấn tạo sườn xuống.
### 3. Hàm xử lý ngắt 
```c
void EXTI2_IRQHandler(){
	uint8_t led1_state = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0){
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, !led1_state);
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}
``` 
- Đọc trạng thái hiện tại của LED PA1.
- Khi nhấn nút (PA2 = 0), đảo trạng thái LED PA1..
- ` EXTI_ClearITPendingBit(EXTI_Line2) ` xóa cơ ngắt để sử dụng ngắt cho lần tiếp theo.
### 4.Hàm delay
```c
void Delay(unsigned int t){
	unsigned int i, j;
	for(i=0; i<t;i++){
		for(j=0;j<0x2aff;j++);
	}
} 
```
- Hàm trên tạo độ trễ cho led, đơn vị là ms. Dùng để nhấp nháy LED PA3 trong hàm main.
### 5. Hàm main
```c
int main(){
	Config_GPIO();
	Config_Nvic();
	Config_Exti();
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, 1);
	while(1){
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		Delay(500);
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
		Delay(500);
	}
}
```
- Trong main để GPIO_WriteBit(GPIOA, GPIO_Pin_1, 1) nhằm phục vụ led PA1 luôn tắt khi RESET lại.
- Trong vòng lặp, led PA3 nhấp nháy chu kì 1s.

Tóm tắt chương trình: Chương trình chính vẫn nhấp nháy led PA3 với chu kì 1s. Khi ấn nút PA2 thì led PA1 đảo trạng thái.

## Video Mô phỏng
> https://youtube.com/shorts/MtFDtrvz5Uk?feature=share

