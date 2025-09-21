# MÔN HỆ THỐNG NHÚNG - Bài 5.2: Điều khiển LED qua UART trên STM32F103C8T6

## Giới thiệu:

Chương trình này sử dụng **STM32F103C8T6** để điều khiển **LED nối với PA0** thông qua giao tiếp UART.  
- Nhận lệnh từ máy tính (PC) qua cổng UART.  
- Nếu nhận chuỗi `"ON"` → LED sáng.  
- Nếu nhận chuỗi `"OFF"` → LED tắt.  
- Chương trình sử dụng **ngắt UART RX** để xử lý dữ liệu đầu vào.

---

## Các bước thực hiện:

### 1. Cấu hình GPIO.
```c
void Config_GPIO(){
    GPIO_InitTypeDef gpio;
    GPIO_InitTypeDef uart;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // LED PA0
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin   = GPIO_Pin_0;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    
    // TX PA9
    uart.GPIO_Mode  = GPIO_Mode_AF_PP;
    uart.GPIO_Pin   = GPIO_Pin_9;
    uart.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &uart);
    
    // RX PA10
    uart.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    uart.GPIO_Pin   = GPIO_Pin_10;
    GPIO_Init(GPIOA, &uart);
}
```
- PA0: LED (Output Push-Pull).
- Cấu hình PA9 làm TX (truyền dữ liệu) kiểu Push-Pull.
- Cấu hình PA10 làm RX (nhận dữ liệu) kiểu Floating Input.
  
### 2. Cấu hình UART.
```c
void Config_Uart(){
    USART_InitTypeDef usart;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    usart.USART_BaudRate            = 9600;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_Parity              = USART_Parity_No;
    usart.USART_StopBits            = USART_StopBits_1;
    usart.USART_WordLength          = USART_WordLength_8b;
    
    USART_Init(USART1, &usart);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // Bật ngắt khi nhận dữ liệu
    NVIC_EnableIRQ(USART1_IRQn);                  // Cho phép ngắt USART1 trong NVIC
    USART_Cmd(USART1, ENABLE);
}
```
- Baudrate 9600, 8N1 (8bit dữ liệu, không parity, 1 stop bit).
- Bật ngắt RX để xử lý dữ liệu khi nhận.
  
### 3. Xử lý ngắt UART RX
```c
void USART1_IRQHandler(){
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
        pcr = UARTx_Getc(USART1);
        if(pcr == '\r'){
            stt = 1;
            res[cnt] = '\0';
            cnt = 0;
        }
        else{
            res[cnt] = pcr;
            cnt++;
        }
    }
}
```
- Mỗi ký tự nhận được lưu vào **mảng** `res`.
- Khi gặp **ký tự** `\r` **(Enter)** đặt cờ `stt = 1` để main loop xử lý lệnh.

### 4. Main Loop
```c
int main(){
    Config_GPIO();
    Config_Uart();
    GPIO_SetBits(GPIOA, GPIO_Pin_0); // LED ban đầu tắt
    while(1){
        if(stt){
            if(strstr(res, ONLED) != NULL){
                GPIO_ResetBits(GPIOA, GPIO_Pin_0); // Bật LED
            }
            else if(strstr(res, OFFLED) != NULL){
                GPIO_SetBits(GPIOA, GPIO_Pin_0);   // Tắt LED
            }
            stt = 0;
        }
    }
}
```
- Kiểm tra cờ  `stt`.
- Nếu nhận `"ON"` LED sáng.
- Nếu nhận `"OFF"` LED tắt.

---
## Video mô phỏng
>https://youtube.com/shorts/UdlpXTTn7Zg?feature=share
