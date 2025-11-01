#include "pzem004t.h"
#include "uart.h"
#include <stdio.h>

//====================================================
// 1. Gói lệnh và buffer
//====================================================
static uint8_t PZEM_Request[8] = {0x01,0x04,0x00,0x00,0x00,0x0A,0x70,0x0D};
static uint8_t PZEM_Response[25];

//====================================================
// 2. Biến lưu dữ liệu đọc được
//====================================================
static float voltage = 0.0f;
static float current = 0.0f;
static float power   = 0.0f;
static float energy  = 0.0f;
static float freq    = 0.0f;
static float pf      = 0.0f;

//====================================================
// 3. CRC16 chuẩn Modbus
//====================================================
static uint16_t Modbus_CRC16(uint8_t *buf, uint8_t len){
    uint16_t crc = 0xFFFF;
    for(uint8_t pos=0; pos<len; pos++){
        crc ^= (uint16_t)buf[pos];
        for(uint8_t i=0;i<8;i++){
            if(crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

//====================================================
// 4. Đọc dữ liệu từ PZEM (an toàn trong FreeRTOS)
//====================================================
void PZEM_ReadAll(TickType_t timeout_ticks){
    uint16_t crc_calc, crc_recv;

    // Gửi lệnh đọc
    UART_SendArray(USART1, PZEM_Request, 8);

    // Nhận 25 byte phản hồi
    for(uint8_t i=0;i<25;i++){
        PZEM_Response[i] = UART_ReceiveByte(USART1);
        if(PZEM_Response[i]==0xFF){  // lỗi timeout
            return;
        }
    }

    // Kiểm tra CRC
    crc_calc = Modbus_CRC16(PZEM_Response, 23);
    crc_recv = (PZEM_Response[24]<<8) | PZEM_Response[23];
    if(crc_calc != crc_recv){
        return; // bỏ qua nếu sai CRC
    }

    // Giải mã dữ liệu
    voltage = ((PZEM_Response[3]<<8)|PZEM_Response[4]) / 10.0f;

    uint32_t i_current = (PZEM_Response[5]) | (PZEM_Response[6]<<8) | (PZEM_Response[7]<<16) | (PZEM_Response[8]<<24);
    current = i_current / 1000.0f;

    uint32_t i_power = (PZEM_Response[9]) | (PZEM_Response[10]<<8) | (PZEM_Response[11]<<16) | (PZEM_Response[12]<<24);
    power = i_power / 10.0f;

    uint32_t i_energy = (PZEM_Response[13]) | (PZEM_Response[14]<<8) | (PZEM_Response[15]<<16) | (PZEM_Response[16]<<24);
    energy = (float)i_energy;

    freq = ((PZEM_Response[17]<<8)|PZEM_Response[18]) / 10.0f;
    pf   = ((PZEM_Response[19]<<8)|PZEM_Response[20]) / 100.0f;
}

//====================================================
// 5. Getter – để hiển thị lên màn hình TFT
//====================================================
float PZEM_GetVoltage(void)   { return voltage; }
float PZEM_GetCurrent(void)   { return current; }
float PZEM_GetPower(void)     { return power; }
float PZEM_GetEnergy(void)    { return energy; }
float PZEM_GetFrequency(void) { return freq; }
float PZEM_GetPF(void)        { return pf; }
