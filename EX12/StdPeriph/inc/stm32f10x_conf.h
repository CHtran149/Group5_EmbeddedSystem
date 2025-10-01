#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
// ... các peripheral khác

/* Fix assert_param undefined symbol */
#define assert_param(expr) ((void)0U)

#endif /* __STM32F10x_CONF_H */
