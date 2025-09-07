# Mô tả chương trình nhấp nháy 2 led với chu kì 1s trên STM32F103C8T6
Chương trình này điều khiển hai chân PA1 và PA2 nhấp nháy theo chu kì 1s

## Các bước thực hiện: 

### 1.Cấu hình GPIO:
```
void Config_Led(){
	
	GPIO_InitTypeDef led;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	led.GPIO_Mode 	= GPIO_Mode_Out_PP;
	led.GPIO_Pin 		= GPIO_Pin_1 | GPIO_Pin_2;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &led);
}
```
Cấu hình hai chân PA1 và PA2 ở chế độ Output Push Pull, tốc độ xung nhịp 50MHz.
### 2.Hàm Delay()
```
void Delay_ms(unsigned int t){
	int i, j;
	for(i=0; i<t; i++){
		for(j = 0; j<0x2aff; j++);
	}
}
```
Hàm ` Delay_ms(unsigned int t) ` được tạo bằng 2 vòng for lồng nhau.
### 3. Hàm main
```
int main(){
	Config_Led();
	while(1){
		GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
		Delay_ms(500);
		GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
		Delay_ms(500);
	}
}
```
Trong hàm main, việc điều khiển nhấp nháy led bằng cách SetBits và ReSetBits 2 chân GPIO PA1 và PA2.

