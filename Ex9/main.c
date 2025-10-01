#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "string.h"
#include "stdio.h"

#define ADC_BUFFER_SIZE 16   

volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];  
volatile uint8_t dma_transfer_done = 0;         

/* Delay */
void Delay_ms(uint32_t t) {
  uint32_t i, j;
	for(i=0; i<t;i++){
		for(j=0; j<0x2aff;j++);
	}
}
/* UART */
void USART1_Init(void) {
   GPIO_InitTypeDef gpio;
   USART_InitTypeDef usart;
	
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // TX (PA9)
   gpio.GPIO_Pin = GPIO_Pin_9;
   gpio.GPIO_Mode = GPIO_Mode_AF_PP;
   gpio.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &gpio);

    // RX (PA10)
   gpio.GPIO_Pin = GPIO_Pin_10;
   gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &gpio);

   usart.USART_BaudRate = 9600;
   usart.USART_WordLength = USART_WordLength_8b;
   usart.USART_StopBits = USART_StopBits_1;
   usart.USART_Parity = USART_Parity_No;
   usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
   USART_Init(USART1, &usart);
   USART_Cmd(USART1, ENABLE);
}

void USART1_SendChar(char c) {
   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
   USART_SendData(USART1, c);
}

struct __FILE { 
    int dummy; 
}; 
FILE __stdout; 
  
int fputc(int ch, FILE *f) { 
    USART1_SendChar(ch); 
    return ch; 
} 
/* ADC */
void ADC1_Init(void) {
  GPIO_InitTypeDef gpio;
  ADC_InitTypeDef adc;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // PA0 -> ADC1_IN0
  gpio.GPIO_Pin = GPIO_Pin_0;
  gpio.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &gpio);
	
		// PA1 -> LED
	gpio.GPIO_Mode 		= GPIO_Mode_Out_PP;
	gpio.GPIO_Pin			= GPIO_Pin_1;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);

    // clock ADC: PCLK2/6 = 72/6 = 12MHz
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  adc.ADC_Mode = ADC_Mode_Independent;
  adc.ADC_ScanConvMode = DISABLE;
  adc.ADC_ContinuousConvMode = ENABLE;
  adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  adc.ADC_DataAlign = ADC_DataAlign_Right;
  adc.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &adc);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    // Bat DMA cho ADC1
  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
   // Hieu chuan
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  // Bat dau ADC bang phan mem 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/* DMA */
void DMA1_Channel1_Init(void) {
	DMA_InitTypeDef dma;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;         // nguon: thanh ghi ADC
  dma.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;            // dích: mang RAM
  dma.DMA_DIR = DMA_DIR_PeripheralSRC;                      // chieu: tu peripheral -> RAM
	dma.DMA_BufferSize = ADC_BUFFER_SIZE;                     // so phan tu: 16ptu
  dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				// khong tang dia chi o nho peripheral
  dma.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // auto tang dia chi RAM
  dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16 bit
  dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  dma.DMA_Mode = DMA_Mode_Circular;                         // vòng lap liên tuc
  dma.DMA_Priority = DMA_Priority_High;
  dma.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &dma);
  // Bat ngat DMA khi truyen xong
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

void Nvic_Init(){
	NVIC_InitTypeDef nvic;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	nvic.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

}

/* ISR DMA */
void DMA1_Channel1_IRQHandler(void) {
  if (DMA_GetITStatus(DMA1_IT_TC1)) { //ktra co transfer complete channel1 khi du 16 ptu
    dma_transfer_done = 1; // báo hi?u dã xong 1 vòng DMA
    DMA_ClearITPendingBit(DMA1_IT_TC1); // xoa co ngat
  }
}

int main(void) {
	int i;
  uint32_t sum;
  uint16_t avg;
  unsigned long voltage_mV;
	USART1_Init();
	ADC1_Init();
	DMA1_Channel1_Init();
  Nvic_Init();
    while(1) {
    if (dma_transfer_done) {
				GPIOA->ODR ^= GPIO_Pin_1;
        dma_transfer_done = 0;
				sum = 0;
        for(i=0; i<ADC_BUFFER_SIZE; i++) {
            sum += adc_buffer[i];
        }
        avg = sum / ADC_BUFFER_SIZE;
        voltage_mV = (avg * 3300) / 4095;
        printf("ADC avg = %u, Voltage = %lu mV\r\n", avg, voltage_mV);
				Delay_ms(5000);
    }
	}
}
