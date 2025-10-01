#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    /* Infinite loop */
    /* Có th? g?i thông tin qua UART d? debug */
    while (1)
    {
    }
}
#endif


int main(void)
{
	   GPIO_InitTypeDef GPIO_InitStructure;
    // B?t clock GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // C?u hình PC13 làm output push-pull

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    while (1)
    {
					int i;
        // Ð?o tr?ng thái LED
        GPIOC->ODR ^= GPIO_Pin_13;

        // Delay don gi?n
        for (i = 0; i < 500000; i++);
    }
}
