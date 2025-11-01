#ifndef __BUTTON__
#define __BUTTON__
#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f10x.h"                  // Device header

#include <stdint.h>

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
	uint8_t first_press_status;
	uint8_t pre_pressed_status;
	uint8_t pre_multi_pressed_status;
	uint8_t pre_holded_status;
	uint8_t press_count;
	uint32_t last_multi_press_time;
	uint32_t last_press_time;
	uint32_t last_hold_time;
	
	
} Button;

void Button_Init(Button *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
extern __inline uint8_t Button_Read(Button *button);
uint8_t Button_IsPressed(Button *button);
uint8_t Button_IsMultiPressed(Button *button, uint8_t num_of_presses);
uint8_t Button_IsHolded(Button *button, uint16_t hold_time);

#ifdef __cplusplus
}
#endif
#endif