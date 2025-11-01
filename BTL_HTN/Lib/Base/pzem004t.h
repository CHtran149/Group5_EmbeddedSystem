#ifndef __PZEM004T_H__
#define __PZEM004T_H__

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

// Hàm khởi tạo & đọc
void PZEM_ReadAll(TickType_t timeout_ticks);

// Getter (lấy dữ liệu đã đọc)
float PZEM_GetVoltage(void);
float PZEM_GetCurrent(void);
float PZEM_GetPower(void);
float PZEM_GetEnergy(void);
float PZEM_GetFrequency(void);
float PZEM_GetPF(void);

#endif
