/**
 ******************************************************************************
  * @file    graphic.c
  ******************************************************************************
  * @author  Dao Nguyen Iot
	* @blog    iot47.com
	* @email   daonguyen20798@gmail.com
	* @Date:   9/7/2018
  ******************************************************************************
	* Đây là chương trình xử lý đồ họa cơ bản, file .c này chứa mã nguồn, hãy đọc file .h để biết cách sử dụng tôi, hoặc đọc file này để hiểu về tôi ! các hàm đồ họa tham khảo từ thư viện Hoàng Sa - LCD5110
	* English Translate by goole: This is a basic graphics program, this file contains source code, please read the .h file to know how to use me, or read this file to understand me.
	******************************************************************************
  */
#include "graphic.h"
//---------------------------------Mã nguồn----------------------------------------------------------------------//
#define swap(a, b) {int t = a; a = b; b = t; }

//--------------Gọi hàm này để liến kết thư viện đồ họa với thư viện phần cứng-----------//
Driver_function set_px;
void IOT47_GFX_connectToDriver(Driver_function funt)
{
	set_px = funt;
}
//-----------------------------Hàm hình học cơ bản  -------------------------------------//

void IOT47_GFX_veduongngang(int16_t x,int16_t y, int16_t dodai, uint32_t color)
{
 int16_t dem;
 for(dem=0;dem<dodai;dem++)
 {
   set_px(x+dem,y,color);
 }
}
void IOT47_GFX_veduongdoc(int16_t x,int16_t y, int16_t docao, uint32_t color)
{
 int16_t dem;
 for(dem=0;dem<docao;dem++)
 {
   set_px(x,y+dem,color);
 }
}
void IOT47_GFX_veduongthang(int16_t x, int16_t y,int16_t x1, int16_t y1,uint32_t color)
{
 int16_t dx,dy,err,ystep; 
 int16_t steep=abs(y1-y) > abs(x1-x);   
 int16_t xtd,ytd;
 if(x==x1)
 {
  ytd=y1-y;  
  ytd=abs(ytd);
  if(y1>y)IOT47_GFX_veduongdoc(x,y,ytd+1,color);    
  else IOT47_GFX_veduongdoc(x1,y1,ytd+1,color);  
  return;
 } 
 
 if(y==y1)
 {    
  xtd=x1-x;
  xtd=abs(xtd);
  if(x1>x)IOT47_GFX_veduongngang(x,y,xtd+1,color);    
  else IOT47_GFX_veduongngang(x1,y1,xtd+1,color);
  return;
 }
 if (steep) 
  {
    swap(x, y);
    swap(x1, y1);
  }    
  
  if (x > x1) 
  {
    swap(x, x1);
    swap(y, y1);
  }
 dx=x1-x;
 dy=abs(y1-y);
 err=dx/2;  
  
  if (y < y1)ystep = 1;    
  else ystep = -1;
    
  for (; x<=x1; x++) 
  {
    if (steep)
		{
				set_px(y,x,color);
		}
    else 
		{
				 set_px(x,y,color);
    }
		
    err -= dy;      
    if (err < 0) 
    {
      y += ystep;
      err += dx;
    }
  }
}
void IOT47_GFX_vehinhchunhat(int16_t x, int16_t y,int16_t rong, int16_t cao,uint32_t color)
{
   int16_t x1,y1;
   x1=x+rong-1;
   y1=y+cao-1;
   IOT47_GFX_veduongdoc(x,y,cao,color); IOT47_GFX_veduongdoc(x1,y,cao,color);     
   IOT47_GFX_veduongngang(x,y,rong,color); IOT47_GFX_veduongngang(x,y1,rong,color);  
}
void IOT47_GFX_vehinhchunhat_kin(int16_t x, int16_t y,int16_t rong, int16_t cao,uint32_t color)
{
   int16_t x1;
   x1=x+rong;        
   for(;x<x1;x++)IOT47_GFX_veduongdoc(x,y,cao,color);
}
void IOT47_GFX_vehinhtron(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
  int16_t ddF_x,f,x0x,x0y ,x0_x,x0_y,y0x,y0y,y0_x,y0_y,ddF_y,x,y;
  if(r<1){return;}//thoát

  f = 1 - r;
  ddF_x = 1;
  ddF_y = -2 * r;
  x = 0;
  y = r;

  set_px(x0, y0+r, color);
  set_px(x0, y0-r, color);
  set_px(x0+r, y0, color);
  set_px(x0-r, y0, color);

  while (x<y) 
   {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;      

    ddF_x += 2;
    f += ddF_x;
    x0x=x0+x;x0y=x0+y; x0_x=x0-x; x0_y=x0-y;
    y0x=y0+x; y0y=y0+y; y0_x=y0-x; y0_y=y0-y;
    set_px(x0x, y0_y, color);
    set_px(x0y, y0_x, color);
    set_px(x0_x, y0_y, color);
    set_px(x0_y, y0_x, color);
    set_px(x0_y, y0x, color);
    set_px(x0_x, y0y, color);
    set_px(x0y, y0x, color); 
    set_px(x0x, y0y, color);   
  }
}
void IOT47_GFX_vehinhtron_kin(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
  int16_t i,f,y0x,y0y,ddF_x,ddF_y,x,y;
  if(r<1){return;}//thoát
  f = 1 - r;
  ddF_x = 1;
  ddF_y = -2 * r;
  x = 0;
  y = r;

  for (i=y0-r; i<=y0+r; i++)set_px(x0, i, color);
  
  while (x<y) 
   {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    y0y=y0+y;
    y0x=y0+x;
    for (i=y0-y; i<=y0y; i++) 
    {
      set_px(x0+x, i, color);
      set_px(x0-x, i, color);    
    } 
    for (i=y0-x; i<=y0x; i++) 
    {
      set_px(x0+y, i, color);
      set_px(x0-y, i, color);
    }    
  }
}
void IOT47_GFX_Bitmap(int16_t x, int16_t y, uint16_t w, uint16_t h,const uint8_t *bitmap, uint32_t color) 
{
  for (uint16_t i=0; i<w; i++ ) 
	{
    for (uint16_t j=0; j<h; j++) 
		{
					set_px(x+i, y+j, color);
    }
  }
}
void IOT47_GFX_Plus_Bitmap(int16_t x0, int16_t y0, uint16_t w, uint16_t h,const uint8_t *bitmap , int16_t goc, int8_t mirror, uint32_t color) 
{
// xoay ảnh bitmap trong4 góc 0-90-180-270
int16_t x,y;
uint16_t w_1,h_1;
w_1=w-1;
h_1=h-1;
for (uint16_t i=0; i<w; i++ ) 
{//con trỏ dich byte
  for (uint16_t j=0; j<h; j++) 
		{// con trỏ dịch bit
       switch(goc)
	     {
				 case 0:       
						y=y0+j;//0
				 if(mirror)
						x=(x0+w_1)-i; // xoay 0 độ  gương
				 else
					 x=x0+i;   // xoay 0 độ  thường 
				break;
				case 90:
				y=(y0+w_1)-i;//90
        if(mirror)
			   x=(x0+h_1)-j;// xoay 90 độ  gương
        else
				 x=x0+j;// xoay 90 độ  thường
				break;
        case 180:
				y=(y0+h_1)-j;//180
        if(mirror)
         x=x0+i; // xoay 180 độ gương
        else
         x=(x0+w_1)-i;// xoay 180 độ thường
         break;
         case 270:    
         y=y0+i;//270
        if(mirror)
          x=x0+j;// xoay 270 độ gương
 				else
					x=(x0+h_1)-j;// xoay 270 độ thường
        break;
        default:
        return;  //thoát        
      }// switch
      set_px(x,y,color);
 }
}
}
void IOT47_GFX_Corner(int16_t x,int16_t y,int16_t w,int16_t h,int16_t r, uint32_t color)
{ 
// Thủ thuật 
//vẽ 4 cạnh hcn + 4 góc phần tư của hình tròn

if( (r>h/2)||(r>w/2))
{
  return;// thoát
}
int16_t xr=x+r, xw=x+w, xw_r=x+w-r;
int16_t yr=y+r, yh=y+h, yh_r=y+h-r;
  //cạnh trên
  IOT47_GFX_veduongngang(xr,y,w-r-r+1,color);
  // cạnh trái
  IOT47_GFX_veduongdoc(x, yr, h-r-r+1,color);
  //cạnh dưới
  IOT47_GFX_veduongngang( xr,yh,w-r-r+1,color);
  // cạnh phải
  IOT47_GFX_veduongdoc(xw, yr, h-r-r+1,color);
  if(r<1) 
  return;//thoát
 // Bắt đầu vẽ góc
 int16_t f = 1 - r;
 int16_t ddF_a = 1;
 int16_t ddF_b = -2 * r;
 int16_t a = 0;
 int16_t b = r;
  while (a<b) 
	{ 
   if (f >= 0) 
		{
      b--;
      ddF_b += 2;
      f += ddF_b;
    }
    a++;
    ddF_a += 2;
    f += ddF_a;
  
    int16_t xw_ra=xw_r +a, xw_rb=xw_r+b;
    int16_t xr_a=xr-a, xr_b=xr-b;
    int16_t yr_a=yr-a, yr_b=yr-b;
    int16_t yh_ra=yh_r+a, yh_rb=yh_r+b;
    // bo góc phải- trên
    set_px( xw_ra, yr_b, color);
    set_px(xw_rb, yr_a, color);
    // bo góc trái-trên
    set_px(xr_a, yr_b, color);
    set_px(xr_b, yr_a, color);
    // bo góc trái dưới
    set_px(xr_b, yh_ra, color);
    set_px(xr_a, yh_rb, color);
    // bo góc phải- dưới 
    set_px(xw_rb, yh_ra, color); 
    set_px(xw_ra, yh_rb, color);   
  }
}
void IOT47_GFX_FillCorner(int16_t x,int16_t y,int16_t w,int16_t h,int16_t r, uint32_t color)
{ 
   // đành lười biếng kiểu này
   // vẽ 4 hình tròn
   int16_t xw_r=x+w-r, yr=y+r, xr=x+r, yh_r=y+h-r;
   int16_t w_2r=w-2*r, h_2r=h-2*r;
   IOT47_GFX_vehinhtron_kin(xw_r, yr, r, color);
   IOT47_GFX_vehinhtron_kin(xr, yr, r, color);
   IOT47_GFX_vehinhtron_kin(xr, yh_r, r,color);
   IOT47_GFX_vehinhtron_kin(xw_r, yh_r, r,color);
   // vẽ hình chữ nhật chèn vào
   IOT47_GFX_vehinhchunhat_kin(xr, y, w_2r, h+1, color);
   IOT47_GFX_vehinhchunhat_kin(x, yr, r, h_2r, color);
   IOT47_GFX_vehinhchunhat_kin(xw_r, yr, r+1,h_2r,color);
}
//----------------------------------------------Hàm vẽ chữ hỗ trợ tiếng Việt -------------------------------------//
void IOT47_GFX_putFont8(int16_t x,int16_t y,uint8_t txt,uint32_t TextColor,uint32_t BackColor)
{ 
    int32_t w = size_font_min[txt];
    for(int16_t i=0;i<w;i++)
        for(int16_t h=0;h<16;h++)
	           {
							 if(read_font_min(i,h,txt) != 0)
								  set_px(i+x,h+y,TextColor);
							 else if(BackColor != COLOR_TRANSPARENT)
								  set_px(i+x,h+y,BackColor);
						 }
}
void IOT47_GFX_putFont16(int16_t x,int16_t y,uint8_t txt,uint32_t TextColor,uint32_t BackColor)
{ 
    int32_t w = size_font16[txt];
    for(int i=0;i<w;i++)
        for(int h=0;h<16;h++)
		        {
							 if(read_font16(i,h,txt) != 0)
								  set_px(i+x,h+y,TextColor);
							 else if(BackColor != COLOR_TRANSPARENT)
								  set_px(i+x,h+y,BackColor);
						 }
}
void IOT47_GFX_StringFont8(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor)
{
   uint8_t offset=0;
   uint8_t utf8_addr;
   while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
      IOT47_GFX_putFont8(x,y,utf8_addr,TextColor,BackColor);
      s+=offset;
      x += size_font_min[utf8_addr]; 
   }
}
void IOT47_GFX_StringFont8_clearChar(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor,int8_t c_clear)
{
   uint8_t offset=0;
   uint8_t utf8_addr;
   while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
		  c_clear--;
		  if(c_clear == (-1))
				IOT47_GFX_putFont8(x,y,utf8_addr,BackColor,BackColor);
			else IOT47_GFX_putFont8(x,y,utf8_addr,TextColor,BackColor);	
      s+=offset;
      x += size_font_min[utf8_addr]; 
   }
}
void IOT47_GFX_StringFont8_with_Size(int16_t x,int16_t y,uint8_t *s,uint16_t size,uint32_t TextColor,uint32_t BackColor)
{
   uint8_t offset=0;
   uint8_t utf8_addr;
	 uint16_t x_max=0;
   while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
      IOT47_GFX_putFont8(x,y,utf8_addr,TextColor,BackColor);
      s+=offset;
      x += size_font_min[utf8_addr]; 
		  x_max+= size_font_min[utf8_addr];
      if(x_max + 8 > size)return;
   }
}
void IOT47_GFX_StringFont16(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor)
{
   uint8_t offset=0;
   uint8_t utf8_addr;
   while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
      IOT47_GFX_putFont16(x,y,utf8_addr,TextColor,BackColor);
      s+=offset;
      x += size_font16[utf8_addr]; 
   }
}
void IOT47_GFX_StringFont16_clearChar(int16_t x,int16_t y,uint8_t *s,uint32_t TextColor,uint32_t BackColor,int8_t c_clear)
{
   uint8_t offset=0;
   uint8_t utf8_addr;
   while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
		  c_clear--; 
		  if(c_clear == (-1)) IOT47_GFX_putFont16(x,y,utf8_addr,BackColor,BackColor);
      else IOT47_GFX_putFont16(x,y,utf8_addr,TextColor,BackColor);
      s+=offset;
      x += size_font16[utf8_addr]; 
   }
}
void IOT47_GFX_StringFont16_with_Size(int16_t x,int16_t y,uint8_t *s,uint16_t size,uint32_t TextColor,uint32_t BackColor)
{
   uint8_t offset=0;
   uint8_t utf8_addr;
	 uint16_t x_max=0;
   while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
      IOT47_GFX_putFont16(x,y,utf8_addr,TextColor,BackColor);
      s+=offset;
      x += size_font16[utf8_addr]; 
		  x_max+= size_font_min[utf8_addr];
		  if(x_max + 8 > size)return;
   }
}
uint16_t IOT47_GFX_StringFont16_getWidth(uint8_t *txt1)
{
  unsigned char *s = txt1;
  unsigned char offset=0;
  unsigned utf8_addr;
  uint16_t x_max=0;
  while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
      s+=offset;
      x_max += size_font16[utf8_addr]; 
   }
  return x_max;
}
uint16_t IOT47_GFX_StringFont8_getWidth(uint8_t *txt1)
{
  unsigned char *s = txt1;
  unsigned char offset=0;
  unsigned utf8_addr;
  uint16_t x_max=0;
  while(*s)
   {
      utf8_addr=UTF8_GetAddr(s,&offset);
      s+=offset;
      x_max += size_font_min[utf8_addr]; 
   }
  return x_max;
}

//----------------------------------------------------Font maker-------------------------------------------------//
unsigned char FontMakerPutChar(int16_t x,int16_t y,uint16_t txt,const Font *my_font,uint32_t color,uint32_t backcolor)
{
    unsigned int str = my_font->font_map[txt];
    int size_w = my_font->font[str-4]; //lấy chiều rộng Font   
    int size_h = my_font->font[str-3]; //lấy chiều cao Font 
    int start_y=my_font->font[str-1];
    for(int i=0;i<size_w;i++)
        for(int h=0;h<size_h;h++)
        {
            if(FontMakerRead_font(i,h,txt,my_font) != 0)
							set_px(i+x,h+y+start_y,color);
            else 
							set_px(i+x,h+y+start_y,backcolor);
				}
    return size_w;
} 
void FontMakerPutString(int16_t x,int16_t y,const char *s,const Font *my_font,uint32_t color,uint32_t backcolor)
{
   unsigned char offset=0;
   uint16_t utf8_addr;
   while(*s)
   {
      utf8_addr=FontMakerUTF8_GetAddr((unsigned char *)s,&offset);
      x +=FontMakerPutChar(x,y,utf8_addr,my_font,color,backcolor)+1;
      s+=offset;
   }
}
unsigned char FontMakerPutCharTRANPARENT(int16_t x,int16_t y,uint16_t txt,const Font *my_font,uint32_t color)
{
    unsigned int str = my_font->font_map[txt];
    int size_w = my_font->font[str-4]; //lấy chiều rộng Font   
    int size_h = my_font->font[str-3]; //lấy chiều cao Font 
    int start_y=my_font->font[str-1];
    for(int i=0;i<size_w;i++)
        for(int h=0;h<size_h;h++)
        {
            if(FontMakerRead_font(i,h,txt,my_font) != 0)
							set_px(i+x,h+y+start_y,color);
				}
    return size_w;
} 
void FontMakerPutStringTRANPARENT(int16_t x,int16_t y,const char *s,const Font *my_font,uint32_t color)
{
   unsigned char offset=0;
   uint16_t utf8_addr;
   while(*s)
   {
      utf8_addr=FontMakerUTF8_GetAddr((unsigned char *)s,&offset);
      x +=FontMakerPutCharTRANPARENT(x,y,utf8_addr,my_font,color)+1;
      s+=offset;
   }
}

//----------------------------------------------------User Font maker-------------------------------------------------//
unsigned char uFontMakerPutChar(int16_t x,int16_t y,uint16_t txt,const uFont *my_font,uint32_t color,uint32_t backcolor)
{
    unsigned int str = my_font->font_map[txt];
    int size_w = my_font->font[str-4]; //lấy chiều rộng Font   
    int size_h = my_font->font[str-3]; //lấy chiều cao Font 
    int start_y=my_font->font[str-1];
	  int i;
    for(i=0;i<size_w;i++)
    {
	      for(int h=0;h<start_y;h++)
        {
					  set_px(i+x,h+y,backcolor);
				} 
				for(int h=start_y;h<size_h+start_y;h++)
        {
            if(UserMakerRead_font(i,h-start_y,txt,my_font) != 0)
							set_px(i+x,h+y,color);
            else 
							set_px(i+x,h+y,backcolor);
				}
				for(int h=size_h+start_y;h<my_font->font[0];h++)
        {
					  set_px(i+x,h+y,backcolor);
				} 
		}
		for(int h=0;h<my_font->font[0];h++)
		   set_px(i+x,h+y,backcolor);
    return size_w;
} 
uint16_t FontMakerGetWidth(const char *s,const uFont *my_font)
{
	 unsigned char offset=0;
   uint16_t utf8_addr;
	 uint16_t x_max=0;
   while(*s)
   {
      utf8_addr=UserMakerUTF8_GetAddr((unsigned char *)s,&offset,my_font);
      s+=offset;
		  x_max += my_font->font[my_font->font_map[utf8_addr]];
   }
	 return x_max;
}
void uFontMakerPutString(int16_t x,int16_t y,const char *s,const uFont *my_font,uint32_t color,uint32_t backcolor)
{
   unsigned char offset=0;
   uint16_t utf8_addr;
   while(*s)
   {
      utf8_addr=UserMakerUTF8_GetAddr((unsigned char *)s,&offset,my_font);
      x +=uFontMakerPutChar(x,y,utf8_addr,my_font,color,backcolor)+1;
      s+=offset;
   }
}

unsigned char uFontMakerPutCharTRANPARENT(int16_t x,int16_t y,uint16_t txt,const uFont *my_font,uint32_t color)
{
    unsigned int str = my_font->font_map[txt];
    int size_w = my_font->font[str-4]; //lấy chiều rộng Font   
    int size_h = my_font->font[str-3]; //lấy chiều cao Font 
    int start_y=my_font->font[str-1];
	  int i;
    for(i=0;i<size_w;i++)
    {
	      for(int h=0;h<start_y;h++)
        {
					  
				} 
				for(int h=start_y;h<size_h+start_y;h++)
        {
            if(UserMakerRead_font(i,h-start_y,txt,my_font) != 0)
							set_px(i+x,h+y,color);

				}
				for(int h=size_h+start_y;h<my_font->font[0];h++)
        {
					  
				} 
		}
		for(int h=0;h<my_font->font[0];h++)
		   {
				 
			 }
    return size_w;
}
void uFontMakerPutStringTRANPARENT(int16_t x,int16_t y,const char *s,const uFont *my_font,uint32_t color)
{
   unsigned char offset=0;
   uint16_t utf8_addr;
   while(*s)
   {
      utf8_addr=UserMakerUTF8_GetAddr((unsigned char *)s,&offset,my_font);
      x +=uFontMakerPutCharTRANPARENT(x,y,utf8_addr,my_font,color)+1;
      s+=offset;
   }
}
//-----------------------------------------------------END OF FILE------------------------------------------------//
