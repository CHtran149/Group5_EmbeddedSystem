#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include <stdio.h>
#include <string.h>

void Delay_ms(unsigned int t){
	unsigned int i, j;
	for(i=0; i<t; i++){
		for(j=0; j<0x2aff; j++);
	}
}

void Config_GPIO(){
	GPIO_InitTypeDef button;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	button.GPIO_Mode		= GPIO_Mode_IPU;
	button.GPIO_Pin			= GPIO_Pin_1;
	GPIO_Init(GPIOA, &button);
}

void Config_Usart(){
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
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
 USART_SendData(USART1, _chr); 
 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET); 
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_AIN;
	gpio.GPIO_Pin			= GPIO_Pin_0;
	GPIO_Init(GPIOA, &gpio);
	//ADC co max_clock ~ 14MHz - 72/6 = 12MHz->chuan
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	adc.ADC_ContinuousConvMode			= DISABLE;//ENABLE; // ADC lien tuc la mau.
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
}

 
unsigned int ADCx_Read(void){ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //do doc 1 lan nen goi lai sau moi lan doc 
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //EOC = End Of Conversion - ADC da chuyen doi va du lieu san sang
	return(ADC_GetConversionValue(ADC1)); // lay du lieu dong thoi xoa co EOC
} 


int Conversion(int x){
	return (x * 3300) / 4095;
}

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