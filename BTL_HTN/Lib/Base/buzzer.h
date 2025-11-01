#ifndef __BUZZER__
#define __BUZZER__


#include "stm32f10x.h"                  // Device header

void Buzzer_Init(uint32_t period, uint16_t prescaler);
void Buzzer_SetFrequency(uint32_t frequency);
void Buzzer_SetDuty(float percent);
void Buzzer_Start(void);
void Buzzer_Stop(void);
uint16_t Buzzer_GetARR(void);

#endif
