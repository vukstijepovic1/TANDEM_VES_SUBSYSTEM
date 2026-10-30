/**
 * @file    main.c
 * @brief   Motor Drivers project.
 * @details This project funtlion like bridge between VESC and GwSky.
 * @version 1.0.0
 * @date    25.01.2026
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "task_mav.h"
#include "task_work.h"
#include "task_bridge.h"
#include "task_tx_can.h"
#include "types.h"

/* Peripherals */
#include "can.h"
#include "common.h"
#include "gpio.h"
#include "util.h"
 
/* External hardware drivers */
#include "vesc.h"

/* Lib */
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"

/* Middleware */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "spi.h"
#include "task.h"
#include "timers.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;
// Mutexes
SemaphoreHandle_t mutex_mav;
SemaphoreHandle_t tx_queue_mutex;
// Queues
QueueHandle_t queue_command;
QueueHandle_t queue_response;
QueueHandle_t queue_cyphal_rx;
QueueHandle_t queue_vesc_rx;

// QueueSets
QueueSetHandle_t queueset_mav;
// Handlers
TaskHandle_t xTaskBridgeHandle = NULL;
// Timers
TimerHandle_t timer_blinky;

/* Temporary variables */
uint64_t boot_time_ms = 0;

// UART RX Interupt Variables
uart_hal_handle_t vesc_uart_handle;


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void timer_blinky_cb(TimerHandle_t xTimer);
void cyphal_cb(void* usr_arg, uint32_t int_status);
void vesccan_cb(void* usr_arg, uint32_t int_status);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{

	/*------------------------------- FreeRTOS -------------------------------*/
	/* Mutexes */
	mutex_mav      = xSemaphoreCreateMutex();
	tx_queue_mutex = xSemaphoreCreateMutex();

	/* Queues */
	queue_command  = xQueueCreate(2, sizeof(vesc_command_message_t));

	queue_response =
	xQueueCreate(2, sizeof(vesc_command_message_t));
	queue_cyphal_rx = xQueueCreate(20, sizeof(can_hal_msg_t));
	queue_vesc_rx   = xQueueCreate(40, sizeof(can_hal_msg_t));

	/* Queuesets */
	queueset_mav = xQueueCreateSet(20);

	xQueueAddToSet(queue_response, queueset_mav);

	/*--------------------------------- NVIC ---------------------------------*/
	HAL_NVIC_SetPriority(SysTick_IRQn, 31);
	HAL_NVIC_SetPriority(PendSV_IRQn, 30);

	/*------------------------------- Dev Time -------------------------------*/
	HAL_DevTimeInit(TIM_HAL_INSTANCE_0);

	/*--------------------------------- GPIO ---------------------------------*/
	gpio_hal_cfg_type_t gpio_cfg;
	gpio_cfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpio_cfg.pinDir = GPIO_HAL_OUTPUT;

	HAL_GPIO_Init(GPIO_HAL_INSTANCE_3, 25, &gpio_cfg);
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_3, 26, &gpio_cfg);

	//This is for Can Tranciever
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 13, &gpio_cfg); 
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 13, 0);

	/* --------------------------------- SPI -------------------------------- */
	spi_hal_cfg_t spi_cfg;
	spi_cfg.databit   = SPI_HAL_DATABIT_8;
	spi_cfg.cpha      = SPI_HAL_CPHA_FIRST;
	spi_cfg.cpol      = SPI_HAL_CPOL_LO;
	spi_cfg.mode      = SPI_HAL_MASTER_MODE;
	spi_cfg.dataOrder = SPI_HAL_DATA_MSB_FIRST;
	spi_cfg.clockRate = 1000000;

	HAL_SPI_Init(SPI_HAL_INSTANCE_0, &spi_cfg);

  	/* --------------------------------- CAN -------------------------------- */

	/*--- This can is Init for VESCCAN ---*/
	HAL_CAN_Init(CAN_HAL_INSTANCE_1, 1000000); 
	/*--- This can is Init for CYPHAL ---*/
	HAL_CAN_Init(CAN_HAL_INSTANCE_0, 1000000); 

	LPC_CAN1->IER = (1 << 0); 
	LPC_CAN2->IER = (1 << 0); 

	HAL_CAN_EnableInterrupt(CAN_HAL_INSTANCE_1, vesccan_cb, NULL);
	HAL_CAN_EnableInterrupt(CAN_HAL_INSTANCE_0, cyphal_cb, NULL);
	/*------------------------------- FreeRTOS -------------------------------*/

	/*Init VESC instances*/
	init_vesc_fleet(); 

	/* work task - lowest priority */
	xTaskCreate(task_work, "work", 512, NULL, 1, NULL);	

	/* Bridge task - medium priority */
	xTaskCreate(task_bridge, "bridge", 512, NULL, 2, &xTaskBridgeHandle);

	/* TX task - medium priority */
	xTaskCreate(task_tx_can, "tx_can", 512, NULL, 4, NULL);

	/* MAVLink task - highest priority */
	xTaskCreate(task_mav, "mav", 512, NULL, 3, NULL);

	/* blinky timer */
	timer_blinky =
	 	xTimerCreate("tim_blinky", 
					 pdMS_TO_TICKS(BLINKY_PERIOD_MS), 
					 pdTRUE, 
					 NULL, 
					 timer_blinky_cb);

	xTimerStart(timer_blinky, 0);

	/* Time past from initialization (we will use as a boot time for System) */
	boot_time_ms = HAL_GetTimeUS() / 1000;

	vTaskStartScheduler();

	/* loop */
	while (1) {

	}
}

/* ============================= User Callbacks ============================= */

static void timer_blinky_cb(TimerHandle_t xTimer)
{
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26,
						 !HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26));
}

/* ============================= FreeRTOS Hooks ============================= */

/* Increment a counter while Application is in Idle. */
void vApplicationIdleHook(void)
{
	ulIdleCycleCount++;
}

/* Callback to trap FreeRTOS Misconfiguration. */
void vAssertCalled(const char* pcFile, uint32_t ulLine)
{
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/* Callback to trap FreeRTOS stack overflow. */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/* Callback to trap FreeRTOS insufficient heap memory. */
void vApplicationMallocFailedHook(void)
{
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/* =============================    HARDFAULT    ============================= */

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} HardFaultStack_t;

volatile uint32_t fault_pc = 0;
volatile uint32_t fault_lr = 0;
volatile uint32_t fault_cfsr = 0;
volatile uint32_t fault_hfsr = 0;
volatile uint32_t fault_addr = 0;
volatile uint32_t fault_task_sp = 0;
volatile char fault_task_name[configMAX_TASK_NAME_LEN] = "Unknown";
void HardFault_Handler(void) {
    uint32_t lr_value;
    __asm volatile("mov %0, lr" : "=r"(lr_value));
    
    uint32_t sp;
    char temp_name[configMAX_TASK_NAME_LEN];
    unsigned int i;
    
    if (lr_value & 0x4) {
        sp = __get_PSP();
        fault_task_sp = sp;
        
        TaskHandle_t current_task = xTaskGetCurrentTaskHandle();
        if (current_task != NULL) {
            const char *name = pcTaskGetName(current_task);
            strncpy(temp_name, name, configMAX_TASK_NAME_LEN - 1);
            temp_name[configMAX_TASK_NAME_LEN - 1] = '\0';
            
            for(i = 0; i < configMAX_TASK_NAME_LEN; i++) {
                fault_task_name[i] = temp_name[i];
                if(temp_name[i] == '\0') break;
            }
        } else {
            char unknown[] = "Unknown";
            for(i = 0; i < sizeof(unknown); i++) {
                fault_task_name[i] = unknown[i];
            }
        }
    } else {
        sp = __get_MSP();
        fault_task_sp = sp;
        
        char kernel[] = "Kernel/ISR";
        for(i = 0; i < sizeof(kernel); i++) {
            fault_task_name[i] = kernel[i];
        }
    }
    
    HardFaultStack_t *stack = (HardFaultStack_t*)sp;
    
    fault_pc = stack->pc;
    fault_lr = stack->lr;
    fault_cfsr = SCB->CFSR;
    fault_hfsr = SCB->HFSR;
    
    if (SCB->CFSR & (1 << 7)) fault_addr = SCB->BFAR;
    if (SCB->CFSR & (1 << 15)) fault_addr = SCB->MMFAR;
    
    if (lr_value & 0x4) {
        UBaseType_t free_stack = uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle());
        if (free_stack < 20) {
            fault_cfsr |= 0x80000000;
        }
    }
    
    __disable_irq();

    
    while(1) {
        int delay = (lr_value & 0x4) ? 500000 : 2000000;
        for(i = 0; i < (unsigned int)delay; i++);
    }
}