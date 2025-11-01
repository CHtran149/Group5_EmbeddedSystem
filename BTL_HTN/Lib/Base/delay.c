#include "delay.h"
#include "FreeRTOS.h"


__inline uint32_t millis(void){
    return (uint32_t)(xTaskGetTickCount() * (1000 / configTICK_RATE_HZ));
}

__inline uint32_t micros(void){
	uint32_t us_per_tick = 1000000 / configTICK_RATE_HZ;
	uint32_t reload = SysTick->LOAD + 1;
	return millis() * 1000 + (reload - SysTick->VAL) * us_per_tick / reload;
}

void delay_ms(uint32_t time_delay){
	uint32_t cur_time = millis();
	while(millis() - cur_time < time_delay);
}

void delay_us(uint32_t time_delay){
	uint32_t cur_time = micros();
	while(micros() - cur_time < time_delay);
}