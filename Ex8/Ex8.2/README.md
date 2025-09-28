# MÔN HỆ THỐNG NHÚNG - Bài 8.2: Đọc ADC kết hợp ngắt và hiển thị qua UART
# Giới thiệu :
# Ví dụ này sử dụng STM32F103C8T6 để :
- Đọc tín hiệu analog từ chân PA0 (ADC1_IN0)
- Tính giá trị trung bình và quy đổi sang điện áp (mV).
- Gửi kết quả qua UART1 để hiển thị trên terminal (Hercules, Putty, RealTerm...).

# Các bước thực hiện : 
# 1.Hàm cấu hình USART
```
void Config_Usart(){
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	gpio.GPIO_Mode		= GPIO_Mode_AF_PP;
	gpio.GPIO_Pin		= GPIO_Pin_9;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Mode		= GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin		= GPIO_Pin_10;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
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
## 2. Cấu hình ADC và ngắt 
```
void Config_ADC(){
    GPIO_InitTypeDef gpio;
    ADC_InitTypeDef adc;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Pin  = GPIO_Pin_0;
    GPIO_Init(GPIOA, &gpio);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    adc.ADC_ContinuousConvMode = ENABLE;                  
    adc.ADC_DataAlign          = ADC_DataAlign_Right;    
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; 
    adc.ADC_Mode               = ADC_Mode_Independent; 
    adc.ADC_NbrOfChannel       = 1;                      
    adc.ADC_ScanConvMode       = DISABLE;                

    ADC_Init(ADC1, &adc);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    NVIC_EnableIRQ(ADC1_2_IRQn);
}
```
- sau khi xong 1 lần chuyển đổi, ADC tự động đo tiếp → chạy liên tục.
- Chọn kênh PA0, thời gian lấy mẫu 55 chu kỳ.
## 3. Hàm cấu hình ngắt
``` 
void ADC1_2_IRQHandler(void) {
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
				flag_stt = 1;
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);   // xóa co ngat
    }
}
``` 
- Khi có ngắt kiểm tra ADC_IT_EOC.
- Nếu đúng ngắt xảy ra đặt biến cờ lên 1.
- Xóa cờ ngắt chuẩn bị cho lần đo tiếp theo.
## 4. Hàm chuyển đổi giá trị 
```
int Conversion(int x){
	return (x * 3300) / 4095;
}
``` 
- Trả về kết quả điện áp.
## 5.Hàm main
```
int main(){
	int vol_mV;
	Config_Usart();
	Config_ADC();
	while(1){
		if(flag_stt){
			adc_value = ADC_GetConversionValue(ADC1);  // doc du lieu
			vol_mV 			= Conversion(adc_value);
			printf("ADC: %u\t Voltage: %d mV\r\n", adc_value, vol_mV);
			Delay_ms(500);
		}
	}
}
```
- khi cờ báo ngắt bật 1 thì ADC bắt đầu đọc dữ liệu và chuyển đổi qua giá trị điện áp.
- Hiển thị trên heculers sau mỗi 500ms.

## Video mô phỏng
> https://youtube.com/shorts/HejG3po4_mc?feature=share