/**
 ******************************************************************************
  * @file    graphic.c
  ******************************************************************************
  * @author  Dao Nguyen Iot
	* @blog    iot47.com
	* @email   daonguyen20798@gmail.com
	* @Date:   9/7/2018
  ******************************************************************************
	* Đây là chương trình xử lí đồ họa cơ bản, sử dụng được cho mọi vi điều khiển dựa trên nền tảng ngôn ngữ C
  *	Sử dụng hàm IOT47_GFX_connectToDriver để kết nối tới hàm có nhiệm vụ vẽ 1 điểm ảnh (set_px). Hàm set_px có nhiệm vụ bật hoặc tắt 1 điểm ảnh
	* 1 số hàm trong này sử dụng cơ chế DMA để tăng tốc độ, hãy hủy định nghĩa Enable DMA nếu bạn không sử dụng nó
	* English Translate by goole:
	* This is a basic graphics processing program, usable for all microcontrollers based on the C language platform
	* Use IOT47_GFX_connectToDriver function according to your set_px function name! The function set_px is responsible for turning on or off 1 pixel
	* Some of these functions use the DMA mechanism to speed up, cancel the Enable DMA definition if you don't use it
	******************************************************************************
  */
#ifndef __I47GRAPHIC_H
#define __I47GRAPHIC_H

//-------------------------------Định nghĩa cơ bản-------------------------------------------------------------//
#define enable_dma_ok          // bạn có sử dụng DMA không ? ( Do you use DMA? )

//------------------------------------------
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "IOT47_VN_font.h"
#include "Fontmaker.h"
//----------------------------------------------------------------------------------------------------
typedef void (*Driver_function)(int16_t x,int16_t y,uint32_t color);
void IOT47_GFX_connectToDriver(Driver_function funt);
//----------------------------------------------------------------------------------------------------
void IOT47_GFX_veduongngang(int16_t x,int16_t y, int16_t dodai, uint32_t color); //hàm này dùng để vẽ 1 đường ngang ra màn hình
void IOT47_GFX_veduongdoc(int16_t x,int16_t y, int16_t docao, uint32_t color);   //hàm này sẽ vẽ 1 đường dọc
void IOT47_GFX_veduongthang(int16_t x, int16_t y,int16_t x1, int16_t y1,uint32_t color);  //hàm này vẽ đường thẳng ( bao gồm cả đường chéo)
void IOT47_GFX_vehinhchunhat(int16_t x, int16_t y,int16_t rong, int16_t cao,uint32_t color); //vẽ hình chữ nhật rỗng
void IOT47_GFX_vehinhchunhat_kin(int16_t x, int16_t y,int16_t rong, int16_t cao,uint32_t color); //vẽ hình chữ nhật đặc
void IOT47_GFX_vehinhtron(int16_t x0, int16_t y0, int16_t r, uint32_t color);                    //vẽ hình tròn rỗng
void IOT47_GFX_vehinhtron_kin(int16_t x0, int16_t y0, int16_t r, uint32_t color); //vẽ hình tròn đặc
void IOT47_GFX_Bitmap(int16_t x, int16_t y, uint16_t w, uint16_t h,const uint8_t *bitmap, uint32_t color) ; //vẽ hình ảnh bitmap
void IOT47_GFX_Plus_Bitmap(int16_t x0, int16_t y0, uint16_t w, uint16_t h,const uint8_t *bitmap , int16_t goc, int8_t mirror, uint32_t color); //vẽ hình và xoay 0 90 180 270 độ
void IOT47_GFX_Corner(int16_t x,int16_t y,int16_t w,int16_t h,int16_t r, uint32_t color); //vẽ hình chữ nhật rỗng bo góc
void IOT47_GFX_FillCorner(int16_t x,int16_t y,int16_t w,int16_t h,int16_t r, uint32_t color); //vẽ hinh chữ nhật đặc bo góc
//----------------------------------------------------------------------------------------------------
void IOT47_GFX_putFont8(int16_t x,int16_t y,uint8_t txt,uint32_t TextColor,uint32_t BackColor);
void IOT47_GFX_putFont16(int16_t x,int16_t y,uint8_t txt,uint32_t TextColor,uint32_t BackColor);
void IOT47_GFX_StringFont8(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor);
void IOT47_GFX_StringFont8_clearChar(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor,int8_t c_clear);
void IOT47_GFX_StringFont16(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor);
void IOT47_GFX_StringFont16_clearChar(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor,int8_t c_clear);
void IOT47_GFX_StringFont8_with_Size(int16_t x,int16_t y,uint8_t *s,uint16_t size,uint32_t TextColor,uint32_t BackColor);
void IOT47_GFX_StringFont16_with_Size(int16_t x,int16_t y,uint8_t *s,uint16_t size,uint32_t TextColor,uint32_t BackColor);
uint16_t IOT47_GFX_StringFont16_getWidth(uint8_t *txt1);
uint16_t IOT47_GFX_StringFont8_getWidth(uint8_t *txt1);
//---------------------------Make Font-----------------------------------------------------------------
unsigned char FontMakerPutChar(int16_t x,int16_t y,uint16_t txt,const Font *my_font,uint32_t color,uint32_t backcolor);
void FontMakerPutString(int16_t x,int16_t y,const char *s,const Font *my_font,uint32_t color,uint32_t backcolor);
unsigned char FontMakerPutCharTRANPARENT(int16_t x,int16_t y,uint16_t txt,const Font *my_font,uint32_t color);
void FontMakerPutStringTRANPARENT(int16_t x,int16_t y,const char *s,const Font *my_font,uint32_t color);
		
unsigned char uFontMakerPutChar(int16_t x,int16_t y,uint16_t txt,const uFont *my_font,uint32_t color,uint32_t backcolor);
void uFontMakerPutString(int16_t x,int16_t y,const char *s,const uFont *my_font,uint32_t color,uint32_t backcolor);
unsigned char uFontMakerPutCharTRANPARENT(int16_t x,int16_t y,uint16_t txt,const uFont *my_font,uint32_t color);
void uFontMakerPutStringTRANPARENT(int16_t x,int16_t y,const char *s,const uFont *my_font,uint32_t color);
//----------------------------------------------------------------------------------------------------
#define color_mode ARGB888 //bạn sử dụng không gian màu gì ? (What color space do you use)

#if (color_mode == ARGB888)
#define COLOR_BLUE ((uint32_t)0xFF0000FF)
#define COLOR_GREEN ((uint32_t)0xFF00FF00)
#define COLOR_RED ((uint32_t)0xFFFF0000)
#define COLOR_CYAN ((uint32_t)0xFF00FFFF)
#define COLOR_MAGENTA ((uint32_t)0xFFFF00FF)
#define COLOR_YELLOW ((uint32_t)0xFFFFFF00)
#define COLOR_LIGHTBLUE ((uint32_t)0xFF8080FF)
#define COLOR_LIGHTGREEN ((uint32_t)0xFF80FF80)
#define COLOR_LIGHTRED ((uint32_t)0xFFFF8080)
#define COLOR_LIGHTCYAN ((uint32_t)0xFF80FFFF)
#define COLOR_LIGHTMAGENTA ((uint32_t)0xFFFF80FF)
#define COLOR_LIGHTYELLOW ((uint32_t)0xFFFFFF80)
#define COLOR_DARKBLUE ((uint32_t)0xFF000080)
#define COLOR_DARKGREEN ((uint32_t)0xFF008000)
#define COLOR_DARKRED ((uint32_t)0xFF800000)
#define COLOR_DARKCYAN ((uint32_t)0xFF008080)
#define COLOR_DARKMAGENTA ((uint32_t)0xFF800080)
#define COLOR_DARKYELLOW ((uint32_t)0xFF808000)
#define COLOR_WHITE ((uint32_t)0xFFFFFFFF)
#define COLOR_LIGHTGRAY ((uint32_t)0xFFD3D3D3)
#define COLOR_GRAY ((uint32_t)0xFF808080)
#define COLOR_DARKGRAY ((uint32_t)0xFF404040)
#define COLOR_BLACK ((uint32_t)0xFF000000)
#define COLOR_BROWN ((uint32_t)0xFFA52A2A)
#define COLOR_ORANGE ((uint32_t)0xFFFFA500)
#define COLOR_TRANSPARENT ((uint32_t)0x20798) //makeking
#else
#if (color_mode == RGB565)
#define COLOR_BLUE ((uint16_t)0xFF0000FF)
#define COLOR_GREEN ((uint16_t)0xFF00FF00)
#define COLOR_RED ((uint16_t)0xFFFF0000)
#define COLOR_CYAN ((uint16_t)0xFF00FFFF)
#define COLOR_MAGENTA ((uint16_t)0xFFFF00FF)
#define COLOR_YELLOW ((uint16_t)0xFFFFFF00)
#define COLOR_LIGHTBLUE ((uint16_t)0xFF8080FF)
#define COLOR_LIGHTGREEN ((uint16_t)0xFF80FF80)
#define COLOR_LIGHTRED ((uint16_t)0xFFFF8080)
#define COLOR_LIGHTCYAN ((uint16_t)0xFF80FFFF)
#define COLOR_LIGHTMAGENTA ((uint16_t)0xFFFF80FF)
#define COLOR_LIGHTYELLOW ((uint16_t)0xFFFFFF80)
#define COLOR_DARKBLUE ((uint16_t)0xFF000080)
#define COLOR_DARKGREEN ((uint16_t)0xFF008000)
#define COLOR_DARKRED ((uint16_t)0xFF800000)
#define COLOR_DARKCYAN ((uint16_t)0xFF008080)
#define COLOR_DARKMAGENTA ((uint16_t)0xFF800080)
#define COLOR_DARKYELLOW ((uint16_t)0xFF808000)
#define COLOR_WHITE ((uint16_t)0xFFFFFFFF)
#define COLOR_LIGHTGRAY ((uint16_t)0xFFD3D3D3)
#define COLOR_GRAY ((uint16_t)0xFF808080)
#define COLOR_DARKGRAY ((uint16_t)0xFF404040)
#define COLOR_BLACK ((uint16_t)0xFF000000)
#define COLOR_BROWN ((uint16_t)0xFFA52A2A)
#define COLOR_ORANGE ((uint16_t)0xFFFFA500)
#define COLOR_TRANSPARENT ((uint16_t)0x00000000)
#endif
#endif

#define CENTER_MODE  0x01 /* Center mode */
#define	RIGHT_MODE   0x02 /* Right mode */
#define	LEFT_MODE    0x03 /* Left mode */
//------------------------------------------
#endif /* __I47GRAPHIC_H */
