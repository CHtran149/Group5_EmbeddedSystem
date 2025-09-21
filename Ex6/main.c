#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "string.h"
#include "stdio.h"

#define BH1750_ADDR 0x23

void Delay_ms(unsigned int t){
	unsigned int i, j;
	for(i = 0; i< t; i++){
		for(j=0; j< 0x2aff; j++);
	}
}

void Config_USART(){
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	gpio.GPIO_Mode			= GPIO_Mode_AF_PP;
	gpio.GPIO_Pin				= GPIO_Pin_9;
	gpio.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin				= GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	usart.USART_BaudRate				= 9600;
	usart.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	usart.USART_Mode						= USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_Parity					= USART_Parity_No;
	usart.USART_StopBits				= USART_StopBits_1;
	usart.USART_WordLength			= USART_WordLength_8b;
	
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);
}

void USARTx_SendChar(char c){
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET); // thanh ghi du lieu truyen khong trong thi truyen.
	USART_SendData(USART1, c);
}


struct __FILE { 
    int dummy; 
}; 
 
FILE __stdout; 
  
int fputc(int ch, FILE *f) { 
    USARTx_SendChar(ch); 
    return ch; 
}

void Config_I2C(){
	GPIO_InitTypeDef gpio_i2c;
	I2C_InitTypeDef	i2c;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	//PB6-SCL; PB7-SDA
	gpio_i2c.GPIO_Mode		= GPIO_Mode_AF_OD;
	gpio_i2c.GPIO_Pin			= GPIO_Pin_6 | GPIO_Pin_7;
	gpio_i2c.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_i2c);
	
	i2c.I2C_Ack						= I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //BH1750 dung 7bit dia chi
	i2c.I2C_ClockSpeed		= 100000; // che do tieu chuan.
	i2c.I2C_DutyCycle			= I2C_DutyCycle_2; //chi quan trong o che do fastmode
	i2c.I2C_Mode 					= I2C_Mode_I2C;
	i2c.I2C_OwnAddress1		= 0x00;
	
	I2C_Init(I2C1, &i2c);
	I2C_Cmd(I2C1, ENABLE);
}

/*================= BH1750 =================*/
void BH1750_Init(void)
{
    // Power on
    I2C_GenerateSTART(I2C1, ENABLE); // Bat tao dieu kien bat dau giao tiep I2C.
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); // ktra su kien I2C cuoi cung co bang voi su kien truyen vao khong. Master da gui start.
    I2C_Send7bitAddress(I2C1, BH1750_ADDR<<1, I2C_Direction_Transmitter); // 7bit dia chi | bit write = 0.
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // master da gui start + address + R/W = 0; slave ACK, master ss transmit.
    I2C_SendData(I2C1, 0x01); // Power ON
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));// Master da truyen xong 1 byte.
    I2C_GenerateSTOP(I2C1, ENABLE); // ket thuc truyen du lieu.

    // Reset
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, BH1750_ADDR<<1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1, 0x07); // Reset
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
    I2C_GenerateSTOP(I2C1, ENABLE);

    // Continuous H-Resolution Mode
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, BH1750_ADDR<<1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1, 0x10);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);
}

uint16_t BH1750_ReadLight(void)
{
    uint16_t value;
    uint8_t high, low;

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, BH1750_ADDR<<1, I2C_Direction_Receiver); // 7bit dia cho | bit read=1.
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));// slave da ACK master ss nhan.

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); // master da nhan 1 byte
    high = I2C_ReceiveData(I2C1);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    low = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, DISABLE); // tat ACK.
    I2C_GenerateSTOP(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE); // bat ACK cho lan doc sau.

    value = (high<<8) | low;
    return (uint16_t)(value / 1.2);
}


int main(){
	Config_USART();
	Config_I2C();
	BH1750_Init();
	while(1){
		uint16_t lux = BH1750_ReadLight();
    printf( "Lux: %u lx\r\n", lux);
		Delay_ms(500);
	}
	
}
