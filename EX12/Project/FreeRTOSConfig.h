#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f10x.h"   // Để lấy SystemCoreClock

/*-----------------------------------------------------------
 * Cấu hình kernel
 *----------------------------------------------------------*/
#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configCPU_CLOCK_HZ              ( ( unsigned long ) SystemCoreClock )
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )   // Tick 1ms
#define configMAX_PRIORITIES            ( 5 )
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 10 * 1024 ) ) // 10KB heap
#define configMAX_TASK_NAME_LEN         ( 16 )
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1

/*-----------------------------------------------------------
 * Cấu hình đồng bộ (queue, semaphore, mutex…)
 *----------------------------------------------------------*/
#define configUSE_MUTEXES               1
#define configUSE_COUNTING_SEMAPHORES   1
#define configQUEUE_REGISTRY_SIZE       8

/*-----------------------------------------------------------
 * Cấu hình hook & debug
 *----------------------------------------------------------*/
#define configCHECK_FOR_STACK_OVERFLOW  0
#define configUSE_MALLOC_FAILED_HOOK    0
#define configUSE_TRACE_FACILITY        0

/*-----------------------------------------------------------
 * Cấu hình API
 *----------------------------------------------------------*/
#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskDelayUntil         1

/* Interrupt nesting behaviour configuration. */
#define configKERNEL_INTERRUPT_PRIORITY         ( 255 )      /* Lowest priority */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 191 )      /* Equivalent to priority 11, do not use 0 */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY ( 15 )


#endif /* FREERTOS_CONFIG_H */
