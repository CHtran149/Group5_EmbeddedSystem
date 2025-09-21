#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "string.h"
#include "stdio.h"


void Config_Uart(){
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef uart;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_AF_PP;
	gpio.GPIO_Pin			= GPIO_Pin_9;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Mode		= GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin			= GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	uart.USART_BaudRate		=9600;
	uart.USART_HardwareFlowControl		= USART_HardwareFlowControl_None;
	uart.USART_Mode				= USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity			= USART_Parity_No;
	uart.USART_StopBits		= USART_StopBits_1;
	uart.USART_WordLength	= USART_WordLength_8b;
	USART_Init(USART1, &uart);
	USART_Cmd(USART1, ENABLE);
}

void uart_SendChar(char c){
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

struct __FILE { 
    int dummy; 
}; 
FILE __stdout; 
  
int fputc(int ch, FILE *f) { 
    uart_SendChar(ch); 
    return ch; 
} 

void Config_SPI(){
	GPIO_InitTypeDef gpio;
	SPI_InitTypeDef spi;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	
	// PA5 CLK; PA7 MOSI
	gpio.GPIO_Mode			= GPIO_Mode_AF_PP;
	gpio.GPIO_Pin				= GPIO_Pin_5 | GPIO_Pin_7;
	gpio.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	//PA6 - MISO;
	gpio.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin				= GPIO_Pin_6;
	GPIO_Init(GPIOA, &gpio);
	
	// PA4 - SC
	gpio.GPIO_Mode			= GPIO_Mode_Out_PP;
	gpio.GPIO_Pin				= GPIO_Pin_4;
	gpio.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
	spi.SPI_BaudRatePrescaler			= SPI_BaudRatePrescaler_16; // chon 16 vi tuong thich voi slave W25Q64
	spi.SPI_CPHA									= SPI_CPHA_1Edge;
	spi.SPI_CPOL									= SPI_CPOL_Low;
	spi.SPI_CRCPolynomial					= 7;
	spi.SPI_DataSize							= SPI_DataSize_8b; // kich thuoc du lieu.
	spi.SPI_Direction							= SPI_Direction_2Lines_FullDuplex; // che do song cong toan phan 4 day.
	spi.SPI_FirstBit							= SPI_FirstBit_MSB; // truyen bit MSB truoc.
	spi.SPI_Mode									= SPI_Mode_Master; //che do master.
	spi.SPI_NSS										= SPI_NSS_Soft; // Cau hinh NSS o che do phan mem.
	SPI_Init(SPI1, &spi);
	
	SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_Transmit(uint8_t data){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); // co dem truyen trong = 1; neu khong trong = 0.
	SPI_I2S_SendData(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); // co bo dem khong trong = 1; 
	return SPI_I2S_ReceiveData(SPI1);
}

void W25Q64_ReadID(uint8_t *id){
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	
	SPI1_Transmit(0x9F);
	id[0] = SPI1_Transmit(0xFF);
	id[1] = SPI1_Transmit(0xFF);
	id[2] = SPI1_Transmit(0xFF);

	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void Delay(int t){
	int i, j;
	for(i=0; i<t; i++){
		for(j=0; j<0x2aff; j++);
	}
}

int main(){
	uint8_t id[3];
	
	Config_Uart();
	Config_SPI();
	W25Q64_ReadID(id);
	while(1){	
		printf("Manufacturer ID: 0x%02X\r\n", id[0]);
    printf("Memory Type    : 0x%02X\r\n", id[1]);
    printf("Capacity       : 0x%02X\r\n", id[2]);
		Delay(500);
	}
}
