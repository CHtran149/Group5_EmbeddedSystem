#include "st7735.h"

void ST7735_WriteCMD(uint8_t cmd){
    ST7735_CS_LOW();		// -> Keo chan CS =0
    ST7735_DC_LOW(); //-> keo chan A0 xuong muc thap de truyen cmd.
    SPI1_SendByte(cmd);
    ST7735_CS_HIGH();
}

void ST7735_WriteData(uint8_t data)
{
	  ST7735_CS_LOW();
    ST7735_DC_HIGH();
    SPI1_SendByte(data);
    ST7735_CS_HIGH();
}

void ST7735_Reset(void)
{
    ST7735_RST_LOW();
    DelayMs(200);
    ST7735_RST_HIGH();
		DelayMs(200);
}

void ST7735_SetWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    ST7735_WriteCMD(0x2A);
    ST7735_WriteData(0x00);
    ST7735_WriteData(x);
    ST7735_WriteData(0x00);
    ST7735_WriteData(x + w - 1);

    ST7735_WriteCMD(0x2B);
    ST7735_WriteData(0x00);
    ST7735_WriteData(y);
    ST7735_WriteData(0x00);
    ST7735_WriteData(y + h - 1);

    ST7735_WriteCMD(0x2C);
}

void ST7735_Clr(void)
{
		uint32_t i;
    ST7735_SetWindow(0, 0, 160, 160);
    for (i = 0; i < 160 * 160; i++)
    {
        ST7735_WriteData(0x00);
        ST7735_WriteData(0x00);
    }
}

void ST7735_Init(void)
{
		int i;
		uint8_t gamma_pos[] = {0x02,0x1C,0x07,0x12,0x37,0x32,0x29,0x2D,0x29,0x25,0x2B,0x39,0x00,0x01,0x03,0x10};
		uint8_t gamma_neg[] = {0x03,0x1D,0x07,0x06,0x2E,0x2C,0x29,0x2D,0x2E,0x2E,0x37,0x3F,0x00,0x00,0x02,0x10};
    ST7735_Reset();

    ST7735_WriteCMD(0x01); DelayMs(5); // Software Reset
    ST7735_WriteCMD(0x11); DelayMs(5); // Sleep Out

    ST7735_WriteCMD(0xB1); ST7735_WriteData(0x01); ST7735_WriteData(0x2C); ST7735_WriteData(0x2D);
    ST7735_WriteCMD(0xB2); ST7735_WriteData(0x01); ST7735_WriteData(0x2C); ST7735_WriteData(0x2D);
    ST7735_WriteCMD(0xB3); ST7735_WriteData(0x01); ST7735_WriteData(0x2C); ST7735_WriteData(0x2D);
                         ST7735_WriteData(0x01); ST7735_WriteData(0x2C); ST7735_WriteData(0x2D);
    ST7735_WriteCMD(0xB4); ST7735_WriteData(0x07);
    ST7735_WriteCMD(0xC0); ST7735_WriteData(0xA2); ST7735_WriteData(0x02); ST7735_WriteData(0x84);
    ST7735_WriteCMD(0xC1); ST7735_WriteData(0xC5);
    ST7735_WriteCMD(0xC2); ST7735_WriteData(0x0A); ST7735_WriteData(0x00);
    ST7735_WriteCMD(0xC3); ST7735_WriteData(0x8A); ST7735_WriteData(0x2A);
    ST7735_WriteCMD(0xC4); ST7735_WriteData(0x8A); ST7735_WriteData(0xEE);
    ST7735_WriteCMD(0xC5); ST7735_WriteData(0x0E);

    ST7735_WriteCMD(0x20); // Display Inversion Off
    ST7735_WriteCMD(0x36); ST7735_WriteData(0xA0); // RGB + landscape
    ST7735_WriteCMD(0x3A); ST7735_WriteData(0x05); // 16-bit/pixel

    ST7735_WriteCMD(0x2A); ST7735_WriteData(0x00); ST7735_WriteData(0x00); ST7735_WriteData(0x00); ST7735_WriteData(127);
    ST7735_WriteCMD(0x2B); ST7735_WriteData(0x00); ST7735_WriteData(0x00); ST7735_WriteData(0x00); ST7735_WriteData(159);

    ST7735_WriteCMD(0xE0);
    for (i = 0; i < 16; i++) ST7735_WriteData(gamma_pos[i]);

    ST7735_WriteCMD(0xE1);
    for (i = 0; i < 16; i++) ST7735_WriteData(gamma_neg[i]);

    ST7735_WriteCMD(0x13); // Normal display mode
    ST7735_WriteCMD(0x29); // Display ON
    ST7735_Clr();
}

void ST7735_FillScreen(uint16_t color)
{
		uint32_t i;
    ST7735_SetWindow(0, 0, 128, 160); // ho?c 128x128 n?u mᮠh􊀠b?n l࡬o?i nh? hon
    for (i = 0; i < 128 * 160; i++)
    {
        ST7735_WriteData(color >> 8);   // byte cao
        ST7735_WriteData(color & 0xFF); // byte th?p
    }
}
void ST7735_draw(uint16_t color)
{
	ST7735_WriteData(color>>8);
  ST7735_WriteData(color);
}
void ST7735_drawPixel(int16_t x,int16_t y,uint16_t color)
{
	if(x<0)return;
	if(y<0)return;
	if(x>=DEVICER_MAX_W)return;
	if(y>=DEVICER_MAX_H)return;
	ST7735_SetWindow(x,y,1,1);
	ST7735_draw(color);
}
void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for(int16_t i = 0; i < w; i++){
        for(int16_t j = 0; j < h; j++){
            ST7735_drawPixel(x + i, y + j, color);
        }
    }
}

// Hàm xoay 
void ST7735_SetRotation(uint8_t mode)
{
    ST7735_WriteCMD(0x36);
    switch (mode)
    {
        case 0: ST7735_WriteData(0x00); break; // Portrait
        case 1: ST7735_WriteData(0x60); break; // Portrait 180
        case 2: ST7735_WriteData(0xA0); break; // Landscape
        case 3: ST7735_WriteData(0xC0); break; // Landscape 270
        default: ST7735_WriteData(0x00); break;
    }
}
