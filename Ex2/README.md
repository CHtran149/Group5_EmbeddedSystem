# Mô tả chương trình nhấp nháy 2 led với chu kì 1s trên STM32F103C8T6
Chương trình này điều khiển hai chân PA1 và PA2 nhấp nháy theo chu kì 1s

## Các bước thực hiện: 

### 1.Cấu hình GPIO:
```
void Config_GPIO(){
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	led.GPIO_Mode 	= GPIO_Mode_Out_PP;
	led.GPIO_Pin 		= GPIO_Pin_1;
	led.GPIO_Speed	= GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOA, &led);
	
	button.GPIO_Mode	= GPIO_Mode_IPU;
	button.GPIO_Pin		= GPIO_Pin_2;
	
	GPIO_Init(GPIOA, &button);
	
}
```
Cấu hình chân PA1 ở chế độ Output Push Pull, tốc độ xung nhịp 50MHz. Chân PA2 được cấu hình ở chế độ Input Up.

### 3. Hàm main
```
int main(){
	Config_GPIO();
	while(1){
		uint8_t status = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0){
			GPIO_WriteBit(GPIOA, GPIO_Pin_1, !status);
		}
	}
}
```
Trong hàm main, chức năng thay đổi trạng thái led khi ấn nút được thực hiện bằng cách sử dụng một biến ` status ` để lưu trạng thái trước đấy của led và sau khi ấn nút thì sẽ lấy giá trị đảo của biến ` status `.

