#ifndef __DELAY__
#define __DELAY__

#include "stm32f10x.h"               

extern __inline uint32_t millis(void);
extern __inline uint32_t micros(void);

void delay_ms(uint32_t time_delay); 
void delay_us(uint32_t time_delay);


#endif
