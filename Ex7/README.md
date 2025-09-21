# MÔN HỆ THỐNG NHÚNG - Bài 7: Đọc ID bộ nhớ Flash W25Q64 qua SPI và hiển thị qua UART

## Giới thiệu:

Ví dụ này sử dụng **STM32F103C8T6** để đọc **Manufacturer ID, Memory Type, và Capacity** từ bộ nhớ Flash **W25Q64** qua giao tiếp **SPI** và gửi giá trị lên máy tính qua **UART1**.  
Chương trình sử dụng **Standard Peripheral Library (SPL)** và hiển thị dữ liệu trên terminal bằng phần mềm Hercules hoặc bất kỳ terminal COM nào.

---

## Các bước thực hiện:

### 1. Cấu hình UART1

```c
void Config_Uart(){
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    
    // TX (PA9)
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin   = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    
    // RX (PA10)
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin   = GPIO_Pin_10;
    GPIO_Init(GPIOA, &gpio);
    
    uart.USART_BaudRate            = 9600;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    uart.USART_Parity              = USART_Parity_No;
    uart.USART_StopBits            = USART_StopBits_1;
    uart.USART_WordLength          = USART_WordLength_8b;
    
    USART_Init(USART1, &uart);
    USART_Cmd(USART1, ENABLE);
}
```

- PA9 là TX và PA10 là RX.
- Baudrate 9600, 8 bit dữ liệu, 1 bit stop, không parity
- Cho phép truyền và nhận dữ liệu.
### 3. Cấu hình SPI1
```c
void Config_SPI(){
    GPIO_InitTypeDef gpio;
    SPI_InitTypeDef spi;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    
    // PA5 CLK; PA7 MOSI
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    
    // PA6 - MISO
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin   = GPIO_Pin_6;
    GPIO_Init(GPIOA, &gpio);
    
    // PA4 - CS (chip select)
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin   = GPIO_Pin_4;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // Tốc độ phù hợp với W25Q64
    spi.SPI_CPHA              = SPI_CPHA_1Edge;
    spi.SPI_CPOL              = SPI_CPOL_Low;
    spi.SPI_CRCPolynomial     = 7;
    spi.SPI_DataSize          = SPI_DataSize_8b;
    spi.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_FirstBit          = SPI_FirstBit_MSB;
    spi.SPI_Mode              = SPI_Mode_Master;
    spi.SPI_NSS               = SPI_NSS_Soft;
    SPI_Init(SPI1, &spi);
    
    SPI_Cmd(SPI1, ENABLE);
}
```
- PA4 = CS, PA5 = CLK, PA6 = MISO, PA7 = MOSI.
- SPI master, 8-bit, MSB trước, tốc độ chia 16 từ APB2.
  
### 4. Hàm truyền và nhận dữ liệu SPI.
```c
uint8_t SPI1_Transmit(uint8_t data){
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}
```
- Truyền 1 byte dữ liệu qua SPI và đồng thời nhận byte dữ liệu từ slave.

### 5. Hàm đọc ID từ W25Q64
```c
void W25Q64_ReadID(uint8_t *id){
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	
	SPI1_Transmit(0x9F);
	id[0] = SPI1_Transmit(0xFF);
	id[1] = SPI1_Transmit(0xFF);
	id[2] = SPI1_Transmit(0xFF);

	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
```
- Kéo CS xuống để chọn chip.
- Gửi lệnh 0x9F đọc ID.
- Nhận 3 byte ID và giải phóng CS.

