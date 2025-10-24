# FreeRTOS STM32F103 - Bài 13.1: Ngắt ngoài điều khiển LED/Còi

## Giới thiệu

Chương trình này được viết cho **STM32F103C8T6** sử dụng **FreeRTOS**, nhằm minh họa cách **kết hợp ngắt ngoài (EXTI)** và **Task** bằng **Semaphore (Binary Semaphore)**.

Mục tiêu:
- Có **2 Task** hoạt động song song:
  1. `Task_Led` – Nhấp nháy LED định kỳ.
  2. `Task_Ctrl` – Bật đèn/còi khi có **ngắt ngoài từ nút nhấn**.
- Ngắt ngoài trên chân **PA1** sẽ kích hoạt semaphore, **đánh thức Task_Ctrl** để xử lý sự kiện.

---

## Cấu hình phần cứng

| Thành phần | Chân GPIO | Chế độ | Mô tả |
|-------------|------------|--------|--------|
| LED1 | PA0 | Output push-pull | LED nhấp nháy trong `Task_Led` |
| LED2 / Còi | PA2 | Output push-pull | Đèn/còi bật khi nhấn nút |
| Nút nhấn | PA1 | Input Pull-up | Kích hoạt ngắt ngoài EXTI Line1 khi nhấn |

---

## Nguyên lý hoạt động

### 1. **Luồng khởi tạo**
1. Gọi `Config_Led()` để cấu hình PA0, PA2 làm ngõ ra.
2. Gọi `Config_EXTI()` để cấu hình ngắt ngoài tại **PA1 (EXTI1)**.
3. Tạo semaphore nhị phân:
   ```Sema_Btn = xSemaphoreCreateBinary(); ```
4. Tạo 2 task:
    - `Task_Led_Funct`: nhấp nháy LED mỗi 500ms.
    - `Task_Ctrl_Funct`: chờ semaphore để bật đèn PA2.
5. Gọi `vTaskStartScheduler()` để chạy FreeRTOS scheduler.

### 2. **Khi chưa nhấn nút**
   - Task `Task_Ctrl` bị block vô thời hạn ở: `xSemaphoreTake(Sema_Btn, portMAX_DELAY);`
   - Do semaphore chưa được "give", nên CPU chỉ cấp thời gian cho Task_Led_Funct.

### 3. **Khi nhấn nút PA1**
1. Ngắt ngoài EXTI1 được kích hoạt, gọi hàm:
   ```c
    void EXTI1_IRQHandler(void)
   ```
2. Trong hàm ngắt
- Gửi tín hiệu Semaphore
    ```c
    xSemaphoreGiveFromISR(Sema_Btn, &flag_check);
    ```
- Xóa cơ ngắt
    ```c
    EXTI_ClearITPendingBit(EXTI_Line1);
    ```
- Gọi `portYIELD_FROM_ISR(flag_check)` để nhường quyền CPU ngay cho Task có ưu tiên cao hơn (Task_Ctrl).
3. `Task_Ctrl_Funct` lập tức được đánh thức (vì có semaphore)
   
## Video mô phỏng
> https://youtube.com/shorts/dk429zukkI0?feature=share

