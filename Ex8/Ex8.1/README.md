# MÔN HỆ THỐNG NHÚNG - Bài 8.1: Đọc ADC kết hợp nút bấm  và hiển thị qua UART
# Giới thiệu :
# Ví dụ này sử dụng STM32F103C8T6 để :
- Sử dụng nút nhấn tại PA1 để điều khiển việc đọc và in kết quả.
- Đọc tín hiệu analog từ chân PA0 (ADC1_IN0)
- Tính giá trị trung bình và quy đổi sang điện áp (mV).
- Gửi kết quả qua UART1 để hiển thị trên terminal (Hercules, Putty, RealTerm...).

# Các bước thực hiện : 

## 1. Hàm cấu hình button 
``` 
void Config_GPIO(){
	GPIO_InitTypeDef button;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	button.GPIO_Mode = GPIO_Mode_IPU;
	button.GPIO_Pin  = GPIO_Pin_1;
	button.GPI0_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &button);
}
```
- PA1 → Input Pull-Up (nút nhấn).
- Khi nhấn, chân PA1 sẽ đọc mức 0.
## 2.Hàm cấu hình USART
``` 
void Config_Usart(){
	USART_InitTypeDef usart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

	usart.USART_BaudRate 			= 9600;
	usart.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	usart.USART_Mode				= USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_Parity				= USART_Parity_No;
	usart.USART_StopBits			= USART_StopBits_1;
	usart.USART_WordLength			= USART_WordLength_8b;
	
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);
}
```
- PA9 = TX, PA10 = RX.
- Baudrate 9600, 8N1, No Parity. Cho phép truyền và nhận dữ liệu.

## 3. Cấu hình ADC 
```
void Config_ADC(){
	ADC_InitTypeDef adc;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);
	
	adc.ADC_ContinuousConvMode	= DISABLE;
	adc.ADC_DataAlign           = ADC_DataAlign_Right; 
	adc.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_None;
    adc. ADC_Mode			    = ADC_Mode_Independent;
	adc.ADC_NbrOfChannel		= 1;				
	adc.ADC_ScanConvMode		= DISABLE;
	ADC_Init(ADC1, &adc);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);  
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1); 
	while(ADC_GetResetCalibrationStatus(ADC1)); 
	ADC_StartCalibration(ADC1); 
	while(ADC_GetCalibrationStatus(ADC1)); 
}
```
- ADC chỉ chuyển đội khi gọi lệnh bắt đầu.
- Sử dụng ADC1, chọn kênh PA0 để lấy mẫu.
- 55.5 cycles → thời gian lấy mẫu cho mỗi phép đo (càng lớn càng chính xác, nhưng tốn thời gian).
## 4. Hàm đọc giá trị ADC 
```
unsigned int ADCx_Read(void){ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);  
	return(ADC_GetConversionValue(ADC1)); 
} 
``` 
- Hàm này trả về 1 giá trị ADC (kiểu unsigned int ~ 16 bit).
- kết quả chạy từ 0 - 4095.

## 5. Hàm  chuyển đổi kết quả ADC sang điện áp (mV)
```

int Conversion(int x){
	return (x * 3300) / 4095;
}
``` 
- Điện áp được chuyển đổi theo công thức trên.
## 6.Hàm main
``` 
int main(){
	uint16_t adc_value;
	int vol_mV;
	
	Config_GPIO();
	Config_Usart();
	Config_ADC();
	
	adc_value 	= ADCx_Read();
	vol_mV 			= Conversion(adc_value);
	printf("ADC: %u\t Voltage: %d mV\r\n", adc_value, vol_mV);
	
	while(1){
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0){
			adc_value 	= ADCx_Read();
			vol_mV 			= Conversion(adc_value);
			printf("ADC: %u\t Voltage: %d mV\r\n", adc_value, vol_mV);
			Delay_ms(200);
		}
	}
}
```
- Gọi biến để lưu kết quả ADC và điện áp.
- Trong vòng lặp: nếu nhấn nút PA1 → đọc ADC, đổi sang mV và in qua UART.
