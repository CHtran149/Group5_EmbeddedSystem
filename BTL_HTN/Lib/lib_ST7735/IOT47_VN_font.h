#ifndef IOT47_VN_FONT___H__
#define IOT47_VN_FONT___H__

#include "stm32f10x.h"

extern const unsigned char size_font_min[];
extern const uint16_t vni_font_min[];
extern const unsigned char size_font16[];
extern const unsigned char vni_font16[];
extern unsigned char UTF8_GetAddr(unsigned char *utf8_char,unsigned char *char_offset);
extern uint16_t read_font_min(int x, int y,unsigned int txt);
extern unsigned char read_font16(int x, int y,  int txt);

#endif //IOT47_VN_FONT___H__
