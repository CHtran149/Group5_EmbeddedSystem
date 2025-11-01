#ifndef __UART__
#define __UART__

#include "stm32f10x.h"
#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

// Mutex bảo vệ UART
extern SemaphoreHandle_t xUARTMutex;
// Hàm config Button
void Config_Button(void);
// Hàm config UART
void Config_UART(void);

// Gửi/nhận byte RTOS
void UART_SendByte_RTOS(USART_TypeDef* USARTx, uint8_t data);
uint8_t UART_ReceiveByte_RTOS(USART_TypeDef* USARTx, TickType_t timeout_ticks);

// Gửi/nhận byte 
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data);
uint8_t UART_ReceiveByte(USART_TypeDef* USARTx);

// Gửi mảng byte
void UART_SendArray(USART_TypeDef* USARTx, uint8_t *array, uint8_t length);

// Hàm gửi chuỗi
void UART_SendString(USART_TypeDef* USARTx, const char *str);

#endif
