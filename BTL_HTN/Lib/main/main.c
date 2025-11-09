#include "all_header.h"

/* ------------------ cấu trúc dữ liệu PZEM (mail) ------------------ */
typedef struct {
    float voltage;
    float current;
    float power;
    float energy;
    float freq;
    float pf;
} PZEM_Data_t;


/* ------------------ handles ------------------ */
static QueueHandle_t xPzemQueue;        // mail queue từ PZEM -> Display
static SemaphoreHandle_t xSPIMutex;     // bảo vệ SPI + driver màn hình
static SemaphoreHandle_t xDataMutex;    // bảo vệ biến gPzemData
static SemaphoreHandle_t xButtonSem;    // binary semaphore: ISR -> ButtonTask

static SemaphoreHandle_t xButtonSem0;  // semaphore riêng cho PA0

/* ------------------ Global threshold ------------------ */
static volatile float gPowerThreshold = 5.0f;   // ngưỡng mặc định
static const float thresholdLevels[] = {5.0f, 25.0f, 50.0f};
static uint8_t thresholdIndex = 0;  // vị trí hiện tại trong mảng trên

/* ------------------ global shared data ------------------ */
static PZEM_Data_t gPzemData;
static volatile uint8_t buzzer_muted = 0;

/* ------------------ prototypes tasks ------------------ */
void Task_PZEM(void *pvParameters);
void Task_Display(void *pvParameters);
void Task_Alert(void *pvParameters);
void Task_ButtonHandler(void *pvParameters);
void Task_ThresholdButton(void *pvParameters);

int main(void){

	Config_SPI();
	Config_UART();
	Config_Button_PA0();
	Config_Button_PA1();
	
  ST7735_CS_HIGH();
  ST7735_Init();
	
	ST7735_SetRotation(0);
	IOT47_GFX_connectToDriver(&ST7735_drawPixel);
//  FontMakerPutString(10,5,"Starting...", &FontDemo1, WHITE, BLACK);
	
	Buzzer_Init(1000, 72);// config PWM tim1
  Buzzer_SetFrequency(2000); // còi 2 kHz
	
	/* ---- tạo synchronization primitives ---- */
  xSPIMutex = xSemaphoreCreateMutex();
  xDataMutex = xSemaphoreCreateMutex();
  xButtonSem = xSemaphoreCreateBinary();
	xButtonSem0 = xSemaphoreCreateBinary();

  /* ---- tạo queue (mail) - length 4 (bạn chỉnh tuỳ) ---- */
  xPzemQueue = xQueueCreate(4, sizeof(PZEM_Data_t));
	
  /* ---- tạo tasks ----
		- Task_PZEM: priority 3
		- Task_Alert: priority 3 (cần phản ứng nhanh hơn display)
    - Task_Display: priority 2
    - Task_ButtonHandler: priority 4 (cao hơn để xử lý nút nhanh)
  */
  xTaskCreate(Task_PZEM,      "PZEM",    384, NULL,  3, NULL);
  xTaskCreate(Task_Alert,     "ALERT",   256, NULL,  3, NULL);
  xTaskCreate(Task_Display,   "DISP",    512, NULL,  2, NULL);
  xTaskCreate(Task_ButtonHandler,"BTN",   256, NULL,  4, NULL);
	xTaskCreate(Task_ThresholdButton, "BTN0", 256, NULL, 4, NULL);

  vTaskStartScheduler();
	while(1){
		
	}
}

/* ------------------ EXTI ISR (PA0) ------------------ */
void EXTI0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
        xSemaphoreGiveFromISR(xButtonSem0, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/* ------------------ EXTI ISR (PA1) ------------------ */
void EXTI1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
        xSemaphoreGiveFromISR(xButtonSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/* ------------------ Task: đọc PZEM, gửi mail ------------------ */
void Task_PZEM(void *pvParameters)
{
    PZEM_Data_t data;
    const TickType_t readDelay = pdMS_TO_TICKS(1000);

    for (;;)
    {
        // Gọi hàm đọc; chọn timeout phù hợp
        PZEM_ReadAll(pdMS_TO_TICKS(1000)); // hàm cập nhật internal vars

        // Lấy giá trị từ getter
        data.voltage = PZEM_GetVoltage();
        data.current = PZEM_GetCurrent();
        data.power   = PZEM_GetPower();
        data.energy  = PZEM_GetEnergy();
        data.freq    = PZEM_GetFrequency();
        data.pf      = PZEM_GetPF();

        // Cập nhật biến global an toàn
        if (xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            gPzemData = data;
            xSemaphoreGive(xDataMutex);
        }

        // Gửi mail cho task hiển thị (non-blocking small timeout)
        xQueueSend(xPzemQueue, &data, pdMS_TO_TICKS(10));

        vTaskDelay(readDelay);
    }
}

/* ------------------ Task: hiển thị dữ liệu lên TFT ------------------ */
void Task_Display(void *pvParameters)
{
    PZEM_Data_t data;
    char buf[40];
	
    static uint8_t oldMuted = 0; // Lưu trạng thái cũ của còi

    for (;;)
    {
        // Chờ mail mới từ Task_PZEM
        if (xQueueReceive(xPzemQueue, &data, portMAX_DELAY) == pdTRUE)
        {
            if (xSemaphoreTake(xSPIMutex, pdMS_TO_TICKS(100)) == pdTRUE)
            {
                // --- Tiêu đề nhóm cố định ---
                FontMakerPutString(0, 0, "Group05EmbeddedSystem", &FontDemo1, CYAN, BLACK);

								FillRect(0, 15, 128, 13, BLACK);
                sprintf(buf, "Threshold=%.0fW", gPowerThreshold);
                FontMakerPutString(5, 12, buf, &FontDemo1, GREEN, BLACK);

                // --- Voltage ---
								FillRect(0, 28, 128, 13, BLACK);
								sprintf(buf, "U=%.1f V", data.voltage);
								FontMakerPutString(5, 28, buf, &FontDemo1, WHITE, BLACK);

								// Current
								sprintf(buf, "I=%.3f A", data.current);
								FontMakerPutString(70, 28, buf, &FontDemo1, WHITE, BLACK);

								// Power
								FillRect(0, 41, 128, 13, BLACK);
								sprintf(buf, "P=%.1f W", data.power);
								FontMakerPutString(5, 41, buf, &FontDemo1, WHITE, BLACK);

								// Energy
								sprintf(buf, "E=%.0f Wh", data.energy);
								FontMakerPutString(70, 41, buf, &FontDemo1, WHITE, BLACK);

								// Frequency
								FillRect(0, 54, 128, 13, BLACK);
								sprintf(buf, "f=%.1f Hz", data.freq);
								FontMakerPutString(5, 54, buf, &FontDemo1, WHITE, BLACK);

								// PF
								sprintf(buf, "PF=%.2f", data.pf);
								FontMakerPutString(70, 54, buf, &FontDemo1, WHITE, BLACK);

                // --- Muted status ---
                if (buzzer_muted != oldMuted)
                {
                    FillRect(5, 67, 120, 13, BLACK);
                    if (buzzer_muted)
                        FontMakerPutString(5, 67, "Muted", &FontDemo1, YELLOW, BLACK);
                    else
                        FontMakerPutString(5, 67, "Unmuted", &FontDemo1, WHITE, BLACK);

                    oldMuted = buzzer_muted;
                }

                xSemaphoreGive(xSPIMutex);
            }
        }
    }
}

/* ------------------ Task: kiểm soát cảnh báo + còi ------------------ */
void Task_Alert(void *pvParameters)
{
    extern volatile float gPowerThreshold;

    PZEM_Data_t snapshot;

    for (;;)
    {
        // Lấy snapshot dữ liệu mới nhất
        if (xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(20)) == pdTRUE)
        {
            snapshot = gPzemData;
            xSemaphoreGive(xDataMutex);
        }

        // Kiểm tra ngưỡng
        if ((snapshot.power > gPowerThreshold) && (buzzer_muted == 0))
        {
            // Bật còi (cài duty >0 trước khi TIM enable)
            Buzzer_SetFrequency(2000);
            TIM_SetCompare1(TIM1, Buzzer_GetARR() / 2);
            // cho đơn giản: dùng hàm Buzzer_Start có SetCompare1 bên trong
            Buzzer_SetFrequency(2000);
            // đặt duty 50%
            TIM_SetCompare1(TIM1, Buzzer_GetARR() / 2);
            Buzzer_Start();
            // hiển thị cảnh báo (cần mutex SPI)
            if (xSemaphoreTake(xSPIMutex, pdMS_TO_TICKS(50)) == pdTRUE)
            {
								FillRect(0, 90, 128, 13, BLACK);
                FontMakerPutString(5, 90, "*** OVERLOAD! ***", &FontDemo1, RED, BLACK);
                xSemaphoreGive(xSPIMutex);
            }
						
        }
        else
        {
            Buzzer_Stop();
            // xóa cảnh báo (vẽ " " hoặc xóa vùng)
            if (xSemaphoreTake(xSPIMutex, pdMS_TO_TICKS(50)) == pdTRUE)
            {
                FillRect(0, 90, 128, 13, BLACK);
                xSemaphoreGive(xSPIMutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(300)); // kiểm tra 300 ms/lần
    }
}

/* ------------------ Task: xử lý nút (được notify từ ISR) ------------------ */
void Task_ButtonHandler(void *pvParameters)
{
    for (;;)
    {
        // chờ semaphore do ISR đưa về
        if (xSemaphoreTake(xButtonSem, portMAX_DELAY) == pdTRUE)
        {
            // debounce nhỏ: chờ 50ms rồi đọc lại (nếu cần)
            vTaskDelay(pdMS_TO_TICKS(50));
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0) // vẫn nhấn
            {
                // toggle trạng thái mute
                buzzer_muted = !buzzer_muted;
            }
            // đợi nút nhả (tránh nhiều lần trigger)
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

/* ------------------ Task: thay đổi ngưỡng cảnh báo (PA0) ------------------ */
void Task_ThresholdButton(void *pvParameters)
{
    for (;;)
    {
        if (xSemaphoreTake(xButtonSem0, portMAX_DELAY) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(50)); // debounce
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) // nút vẫn nhấn
            {
                // Tăng chỉ số ngưỡng (vòng tròn)
                thresholdIndex = (thresholdIndex + 1) % 3;
                gPowerThreshold = thresholdLevels[thresholdIndex];

                // Hiển thị lại ngưỡng hiện tại trên TFT
                if (xSemaphoreTake(xSPIMutex, pdMS_TO_TICKS(100)) == pdTRUE)
                {
                    char buf[20];
										FillRect(0, 15, 128, 13, BLACK);
										sprintf(buf, "Threshold=%.0fW", gPowerThreshold);
										FontMakerPutString(5, 12, buf, &FontDemo1, GREEN, BLACK);
                    xSemaphoreGive(xSPIMutex);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(200)); // chờ nhả
        }
    }
}
