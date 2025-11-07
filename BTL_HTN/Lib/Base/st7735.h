#ifndef __ST7735__
#define __ST7735__

#include "stm32f10x.h"
#include "spi.h"

#define DEVICER_MAX_W 160
#define DEVICER_MAX_H 128

#define WHITE             0xFFFF
#define BLACK             0x0000      
#define BLUE              0x001F  
#define RED               0xF800
#define MAGENTA           0xF81F
#define GREEN             0x07E0
#define CYAN              0x7FFF
#define YELLOW            0xFFE0
#define BROWN 		  			0XBC40 
#define GRAY  		  			0X8430


#define ST7735_CS_HIGH()   GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define ST7735_CS_LOW()    GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define ST7735_DC_HIGH()   GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define ST7735_DC_LOW()    GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define ST7735_RST_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define ST7735_RST_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_2)



void ST7735_WriteCMD(uint8_t cmd);
void ST7735_WriteData(uint8_t data);
void ST7735_Reset(void);
void ST7735_Init(void);
void ST7735_SetWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void ST7735_Clr(void);
void ST7735_FillScreen(uint16_t color);
void ST7735_draw(uint16_t color);
void ST7735_drawPixel(int16_t x,int16_t y,uint16_t color);
void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ST7735_SetRotation(uint8_t mode);

#endif
