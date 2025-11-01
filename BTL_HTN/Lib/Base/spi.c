#include "spi.h"

// Mutex SPI
SemaphoreHandle_t xSPIMutex;

void Config_SPI(void)
{
    GPIO_InitTypeDef gpio;
    SPI_InitTypeDef spi;
    // Tạo mutex
    xSPIMutex = xSemaphoreCreateMutex();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // --- SPI1 Pins: SCK, MOSI, NSS (CS) ---
    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;   // SCK, MOSI
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    // --- CS, DC, RST GPIO thường ---
    gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio);

    // --- Cấu hình SPI1 ---
    spi.SPI_Direction = SPI_Direction_1Line_Tx;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_1Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // t?c d?
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &spi);

    SPI_Cmd(SPI1, ENABLE);
}


void SPI1_SendByte(uint8_t data)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}

void SPI1_SendByte_RTOS(uint8_t data)
{
    if(xSemaphoreTake(xSPIMutex, portMAX_DELAY) == pdTRUE) {
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPI1, data);
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
        xSemaphoreGive(xSPIMutex);
    }
}


void DelayMs(int t){
	int i, j;
	for(i=0; i<t; i++){
		for(j=0; j<0x2aff;j++);
	}
}
