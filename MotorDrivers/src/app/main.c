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
QueueHandle_t queue_status_1;
QueueHandle_t queue_status_2;
QueueHandle_t queue_status_3;
QueueHandle_t queue_status_4;
QueueHandle_t queue_status_5;
QueueHandle_t queue_status_6;
QueueHandle_t queue_cyphal_rx;
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
	queue_status_1 = 
	xQueueCreate(2, sizeof(messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0));
	queue_status_2 = 
	xQueueCreate(2, sizeof(messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0));
	queue_status_3 = 
	xQueueCreate(2, sizeof(messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0));
	queue_status_4 = 
	xQueueCreate(2, sizeof(messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0));
	queue_status_5 = 
	xQueueCreate(2, sizeof(messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0));
	queue_status_6 = 
	xQueueCreate(2, sizeof(messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0));
	queue_cyphal_rx = xQueueCreate(10, sizeof(can_hal_msg_t));


	/* Queuesets */
	queueset_mav = xQueueCreateSet(20);

	xQueueAddToSet(queue_response, queueset_mav);
	xQueueAddToSet(queue_status_1, queueset_mav);
	xQueueAddToSet(queue_status_2, queueset_mav);
	xQueueAddToSet(queue_status_3, queueset_mav);
	xQueueAddToSet(queue_status_4, queueset_mav);
	xQueueAddToSet(queue_status_5, queueset_mav);
	xQueueAddToSet(queue_status_6, queueset_mav);

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

	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 13, &gpio_cfg);//This is for Can Tranciever 
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 13, 0);

#if VESC_CONNECTION_TYPE
	/*--------------------------------- UART ---------------------------------*/

    uart_hal_cfg_t uart_cfg;
	uart_cfg.baudrate = UART_BAUD_RATE_VESC;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_NONE;
	uart_cfg.stopbits = UART_HAL_STOPBIT_1;

    HAL_UART_Init(UART_HAL_INSTANCE_1, &uart_cfg);

	uart_hal_fifo_cfg_t fifo_uart_cfg;
	fifo_uart_cfg.resetRxBuf = ENABLE;
	fifo_uart_cfg.resetTxBuf = ENABLE;
	fifo_uart_cfg.DMAMode = DISABLE;
	fifo_uart_cfg.triggerLevel = UART_HAL_FIFO_TRIGGER_LEVEL_2;

	HAL_UART_ConfigureFIFO(UART_HAL_INSTANCE_1, &fifo_uart_cfg);

	HAL_UART_TransferCreateHandle(UART_HAL_INSTANCE_1, 
								  &vesc_uart_handle, 
								  NULL, 
								  NULL);

	HAL_UART_EnableInterrupt(UART_HAL_INSTANCE_1);

	/*------------------------------------------------------------------------*/

#else
  	/* ------------------------------- CAN VESC ----------------------------- */

	/*--- This can is Init for VESCCAN ---*/
	HAL_CAN_Init(CAN_HAL_INSTANCE_1, 500000); 

#endif
	/* ------------------------------ CYPHAL CAN ---------------------------- */

	/*--- This can is Init for CYPHAL ---*/
	HAL_CAN_Init(CAN_HAL_INSTANCE_0, 1000000); 

	HAL_CAN_EnableInterrupt(CAN_HAL_INSTANCE_0, cyphal_cb, NULL);

	/*------------------------------- FreeRTOS -------------------------------*/
	/* work task - lowest priority */
	xTaskCreate(task_work, "work", 512, NULL, 1, NULL);	

	/* Bridge task - medium priority */
	xTaskCreate(task_bridge, "bridge", 512, NULL, 2, &xTaskBridgeHandle);

	/* TX task - medium priority */
	xTaskCreate(task_tx_can, "tx_can", 256, NULL, 2, NULL);

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