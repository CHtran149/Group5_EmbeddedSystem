#include "uart.h"

// Mutex UART
SemaphoreHandle_t xUARTMutex;

void Config_Button(void){
	GPIO_InitTypeDef button;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	button.GPIO_Mode		= GPIO_Mode_IPU;
	button.GPIO_Pin			= GPIO_Pin_1;
	GPIO_Init(GPIOA, &button);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	exti.EXTI_Line		= EXTI_Line1;
	exti.EXTI_LineCmd	= ENABLE;
	exti.EXTI_Mode		= EXTI_Mode_Interrupt;
	exti.EXTI_Trigger	= EXTI_Trigger_Falling;
	EXTI_Init(&exti);
	
	nvic.NVIC_IRQChannel		= EXTI1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 3;
	nvic.NVIC_IRQChannelSubPriority	= 0;
	nvic.NVIC_IRQChannelCmd	= ENABLE;
	NVIC_Init(&nvic);
}

// Cấu hình UART1
void Config_UART(void){
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;

    // Tạo mutex
    xUARTMutex = xSemaphoreCreateMutex();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // TX
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    // RX
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    uart.USART_BaudRate = 9600;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_WordLength = USART_WordLength_8b;

    USART_Init(USART1, &uart);
    USART_Cmd(USART1, ENABLE);
}

// Gửi byte có mutex
void UART_SendByte_RTOS(USART_TypeDef* USARTx, uint8_t data){
    if (xUARTMutex != NULL){
        if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE){
            while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET){
                taskYIELD(); // nhường CPU cho task khác
            }
            USART_SendData(USARTx, data);
            xSemaphoreGive(xUARTMutex);
        }
    }
}

// Nhận byte với timeout (tick)
uint8_t UART_ReceiveByte_RTOS(USART_TypeDef* USARTx, TickType_t timeout_ticks){
    TickType_t start = xTaskGetTickCount();
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET){
        if ((xTaskGetTickCount() - start) >= timeout_ticks)
            return 0xFF; // timeout
        taskYIELD();
    }
    return USART_ReceiveData(USARTx);
}
/////////////
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, data);
}

uint8_t UART_ReceiveByte(USART_TypeDef* USARTx)
{
    uint32_t timeout = 100000; // Gi?i h?n ch?
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
    {
        if (--timeout == 0)
        {
            return 0xFF;
        }
    }
    return USART_ReceiveData(USARTx);
}

// Gửi mảng byte
void UART_SendArray(USART_TypeDef* USARTx, uint8_t *array, uint8_t length){
	uint8_t  i;
    for(i=0; i<length; i++){
        UART_SendByte(USARTx, array[i]);
    }
}

// Gửi chuỗi
void UART_SendString(USART_TypeDef* USARTx, const char *str){
    while(*str){
        UART_SendByte(USARTx, *str++);
    }
}

// printf support
struct __FILE { int dummy; };
FILE __stdout;

int fputc(int ch, FILE *f){
    UART_SendByte(USART1, ch);
    return ch;
}
