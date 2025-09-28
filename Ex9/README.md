# MÔN HỆ THỐNG NHÚNG - Bài 9: Đọc ADC với DMA và hiển thị qua UART

## Giới thiệu:

Ví dụ này sử dụng **STM32F103C8T6** để:
- Đọc tín hiệu analog từ chân **PA0 (ADC1_IN0)**.  
- Sử dụng **DMA1 Channel 1** để chuyển dữ liệu ADC sang bộ nhớ RAM (buffer 16 mẫu).  
- Tính giá trị trung bình và quy đổi sang điện áp (mV).  
- Gửi kết quả qua **UART1** để hiển thị trên terminal (Hercules, Putty, RealTerm...).  
- Nhấp nháy LED trên **PA1** mỗi khi DMA hoàn thành một vòng truyền.   

---

## Các bước thực hiện:

### 1. Cấu hình UART1
```c
void USART1_Init(void) {
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // TX (PA9)
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    // RX (PA10)
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    usart.USART_BaudRate = 9600;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}
```
- PA9 = TX, PA10 = RX.
- Baudrate 9600, 8N1, No Parity. Cho phép truyền và nhận dữ liệu.

### 2. Cấu hình ADC1
```c
void ADC1_Init(void) {
    GPIO_InitTypeDef gpio;
    ADC_InitTypeDef adc;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // PA0 -> ADC1_IN0
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio);

    // PA1 -> LED
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = GPIO_Pin_1;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_ContinuousConvMode = ENABLE;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &adc);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
```
- Đọc ADC từ PA0, LED báo trạng thái trên PA1.
- ADC chạy chế độ Continuous Conversion
  
### 3. Cấu hình DMA1 Channel 1
```c
void DMA1_Channel1_Init(void) {
    DMA_InitTypeDef dma;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel1);

    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    dma.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize = ADC_BUFFER_SIZE;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel1, &dma);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}
```
- DMA copy dữ liệu từ ADC->DR sang adc_buffer[]/.
- Chế độ circular: lặp liên tục.
- Tự động sinh ngắt khi đủ 16 mẫu.

### 4. Ngắt DMA
```c
void DMA1_Channel1_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_TC1)) {
        dma_transfer_done = 1;
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}
```
- Đặt cờ `dma_transfer_done = 1` khi DMA hoàn thành 1 vòng.

### 5. Hàm main
```c
int main(void) {
    int i;
    uint32_t sum;
    uint16_t avg;
    unsigned long voltage_mV;

    USART1_Init();
    DMA1_Channel1_Init();
    Nvic_Init();
    ADC1_Init();

    while(1) {
        if (dma_transfer_done) {
            GPIOA->ODR ^= GPIO_Pin_1;  // Toggle LED
            dma_transfer_done = 0;
            sum = 0;
            for(i=0; i<ADC_BUFFER_SIZE; i++) {
                sum += adc_buffer[i];
            }
            avg = sum / ADC_BUFFER_SIZE;
            voltage_mV = (avg * 3300) / 4095;
            printf("ADC avg = %u, Voltage = %lu mV\r\n", avg, voltage_mV);
            Delay_ms(5000);
        }
    }
}
```
- Tính giá trị trung bình ADC.
- Quy đổi sang điện áp mV.
- In kết quả qua UART.
- LED trên PA1 nhấp nháy mỗi vòng DMA.


## Video mô phỏng
> https://youtube.com/shorts/Jrh28g9ues8?feature=share