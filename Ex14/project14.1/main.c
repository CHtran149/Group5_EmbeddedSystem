#include "stm32f10x.h"                  // Device header
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

void GPIO_Config(void) {
	  GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void TIM2_Config(void) {
	  TIM_TimeBaseInitTypeDef TIM_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_InitStructure.TIM_Period = 1000 - 1; // 1ms x 1000 = 1s
    TIM_InitStructure.TIM_Prescaler = 7200 - 1; // 72MHz / 7200 = 10kHz
    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // Cho phép ngắt khi Timer tràn
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE); // Bắt đầu Timer
}

void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        // Đảo trạng thái LED
        GPIOC->ODR ^= GPIO_Pin_13;
    }
}

void enter_sleep_mode(void) {
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; // Chọn chế độ Sleep (không phải Stop hay Standby)
    __WFI(); // Chờ ngắt để đánh thức
}

int main(void) {
    GPIO_Config();
    TIM2_Config();

    while (1) {
        enter_sleep_mode(); // MCU sẽ ngủ và thức dậy mỗi 1 giây để đảo LED
    }
}