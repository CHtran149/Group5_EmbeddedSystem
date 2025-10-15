#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include <stdio.h>
#include <string.h>


volatile uint16_t adc_value = 0;
volatile uint8_t flag_stt = 0;

void Delay_ms(unsigned int t){
	unsigned int i, j;
	for(i=0; i<t; i++){
		for(j=0; j<0x2aff; j++);
	}
}

void Config_Usart(){
	GPIO_InitTypeDef gpio;
 
	USART_InitTypeDef usart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	gpio.GPIO_Mode		= GPIO_Mode_AF_PP;
	gpio.GPIO_Pin			= GPIO_Pin_9;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Mode		= GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin			= GPIO_Pin_10;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	usart.USART_BaudRate 			= 9600;
	usart.USART_HardwareFlowControl			= USART_HardwareFlowControl_None;
	usart.USART_Mode					= USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_Parity				= USART_Parity_No;
	usart.USART_StopBits			= USART_StopBits_1;
	usart.USART_WordLength		= USART_WordLength_8b;
	
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);
}

void uart_SendChar(char _chr){ 
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET); 
	USART_SendData(USART1, _chr); 
} 

struct __FILE { 
    int dummy; 
}; 
FILE __stdout; 
  
int fputc(int ch, FILE *f) { 
    uart_SendChar(ch); 
    return ch; 
} 

void Config_ADC(){
	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_AIN;
	gpio.GPIO_Pin			= GPIO_Pin_0;
	GPIO_Init(GPIOA, &gpio);
	//ADC co max_clock ~ 14MHz - 72/6 = 12MHz->chuan
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	adc.ADC_ContinuousConvMode			= ENABLE; // ADC lien tuc la mau.
	adc.ADC_DataAlign								= ADC_DataAlign_Right; // can le right-12bit thap
	adc.ADC_ExternalTrigConv				= ADC_ExternalTrigConv_None; //chon triger ben ngoai de chuyen doi ADC.
	adc.ADC_Mode										= ADC_Mode_Independent; // chi chay rieng 1 channel ADC
	adc.ADC_NbrOfChannel						= 1;				// chi 1 kenh chuyen doi khi scan.
	adc.ADC_ScanConvMode						= DISABLE; // chi do co dinh 1 kenh.
	ADC_Init(ADC1, &adc);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);  // ADC1, kenh 1, rank:1 khi scan, tan so lay mau 55,5T
	ADC_Cmd(ADC1, ENABLE);
	// hieu chuan ADC
	ADC_ResetCalibration(ADC1); // xoa gia tri hieu chuan cu
	while(ADC_GetResetCalibrationStatus(ADC1)); //cho thao tac reset hoan tat
	ADC_StartCalibration(ADC1); // bat dau quy trinh hieu chuan
	while(ADC_GetCalibrationStatus(ADC1)); // cho hieu chuan hoan tat	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //chi can goi lai 1 lan
	// Bat NVIC ADC
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); // cho phep ngat khi chuyen doi xong
	NVIC_EnableIRQ(ADC1_2_IRQn);
}

void ADC1_2_IRQHandler(void) {
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
				flag_stt = 1;
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);   // xóa co ngat
    }
}

int Conversion(int x){
	return (x * 3300) / 4095;
}

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