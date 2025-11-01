#include "button.h"
#include "delay.h"


void Button_Init(Button *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	
	button->port = GPIOx;
	button->pin  = GPIO_Pin;
	
	if(button->port == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	if(button->port == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	if(button->port == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
	GPIO_InitStruct.GPIO_Pin = button->pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(button->port, &GPIO_InitStruct);

	for(volatile int i = 0; i < 0xffff; i++); // Delay for stable

	button->first_press_status = (button->port->IDR & button->pin) && 1;
	button->pre_pressed_status = Button_Read(button);
}

__inline uint8_t Button_Read(Button *button)
{
	uint8_t status = (button->port->IDR & button->pin) && 1;
	return button->first_press_status ? !status : status;
}

uint8_t Button_IsPressed(Button *button)
{
	uint8_t ispressed = 0;
	uint8_t status = Button_Read(button);
	
	if((int8_t)(status - button->pre_pressed_status) < 0)
	{
		if((millis() - button->last_press_time) < 200)  // Debounce time
		{
			ispressed = 1;
		}
	}
	else if((int8_t)(status - button->pre_pressed_status) > 0)
	{
		button->last_press_time = millis();
	}
	
	button->pre_pressed_status = status;
	
	return ispressed;
}

uint8_t Button_IsMultiPressed(Button *button, uint8_t num_of_presses)
{
	uint8_t ismulti = 0;
	uint8_t status = Button_Read(button);
	
	if((int8_t)(status - button->pre_multi_pressed_status) < 0)
	{
		if((millis() - button->last_multi_press_time) < 200)  // Debounce time
		{
			button->press_count++;
			if(button->press_count == num_of_presses)
			{
				button->press_count = 0;
				ismulti = 1;
			}
		}
	}
	else if((int8_t)(status - button->pre_multi_pressed_status) > 0)
	{
		button->last_multi_press_time = millis();
	}
	else if(!status)
	{
		if((millis() - button->last_multi_press_time) > 500) // Reset count if timeout
		{
			button->press_count = 0;
		}
	}
	
	button->pre_multi_pressed_status = status;
	
	return ismulti;
}


uint8_t Button_IsHolded(Button *button, uint16_t hold_time)
{
	uint8_t isholded = 0;
	uint8_t status = Button_Read(button);
	
	if(!status) 
	{
		button->last_hold_time = millis();
		button->pre_holded_status = status;
	}

	if(status != button->pre_holded_status && status && ((millis() - button->last_hold_time) > hold_time))
	{
		isholded = 1;
		button->pre_holded_status = status;
	}
	
	return isholded;
}