#ifndef __SPI__
#define __SPI__

#include "stm32f10x.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

void Config_SPI(void);
void SPI1_SendByte(uint8_t data);
void SPI1_SendByte_RTOS(uint8_t data);

void DelayMs(int t);
#endif
