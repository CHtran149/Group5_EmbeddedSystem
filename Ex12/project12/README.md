# MÔN HỆ THỐNG NHÚNG - BÀI 12 : Sử dụng FreeRTOS thực hiện nháy Blink LED   
# Giới thiệu : 
Ví dụ này sử dụng STM32F301C8T6 và FreeRTOS để :
- Task 1: Thực hiện Blink LED theo tần số, độ rộng xung nhận được qua Queue.
- Task 2: Định kỳ tạo ra tần số và độ rộng xung thay đổi và gửi sang Task 1 qua Queue.
- 2 Task hoạt động song song, kết quả LED nhấp nháy với tần số, độ rộng xung khác nhau theo thời gian.

# Các bước thực hiện :
# 1. Hàm main 
``` 
int main(){
	Config_Led();
	
	Queue01 = xQueueCreate(1, sizeof(uint32_t));
	xTaskCreate(Task_Led_Funct, "LED", 128, NULL, 1, NULL);
	xTaskCreate(Task_Ctrl_Funct, "CTRL", 128, NULL, 2, NULL);
	vTaskStartScheduler();
	while(1){
	}
}
```
- Tạo queue có 1 slot chứa uint32_t (dùng làm biến chia sẻ an toàn).
- Tạo 2 task: LED (priority 1) và Controller (priority 2).
# 2. Hàm định kỳ tạo ra tần số.
```
void Task_Ctrl_Funct(void * argument){
	uint32_t max = 100;
	uint32_t min = 500;
	uint8_t check = 0;
	while(1){
		if(check == 0){
			xQueueOverwrite(Queue01, &max);
			vTaskDelay(pdMS_TO_TICKS(5000));
			check = 1;
		}
		else{
			xQueueOverwrite(Queue01, &min);
			vTaskDelay(pdMS_TO_TICKS(10000));
			check = 0;
		}
	}
}
```
- Gửi max (100 ms) hoặc min (500 ms) vào queue. Dùng xQueueOverwrite phù hợp cho queue size 1.
- Nếu muốn đảo tốc độ cứ 5s một lần, dùng vTaskDelay(5000) ở cả nhánh.
# 3. Hàm Blink LED 
```
uint32_t delay_time;
uint32_t recv_time;
while(1){
    if(xQueueReceive(Queue01, &recv_time, 0) == pdPASS){
        delay_time = recv_time;
    }
    GPIOA->ODR ^= GPIO_Pin_0;
    vTaskDelay(pdMS_TO_TICKS(delay_time));
}
```
-  Mục tiêu: đọc lệnh (nếu có) rồi toggle LED và delay theo delay_time.
- LED đảo trạng thái.

# 4. Video mô phỏng
 - https://youtube.com/shorts/R2czfR25lpY?feature=share
