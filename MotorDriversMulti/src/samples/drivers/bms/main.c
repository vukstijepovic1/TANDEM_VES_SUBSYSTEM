/**
 * @file    main.c
 * @brief   Daly BMS sample.
 * @version 1.0.0
 * @date    18.05.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/* Peripherals */
#include "common.h"
#include "gpio.h"
#include "uart.h"
#include "util.h"

/* External hardware drivers */
#include "bms.h"

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* BMS UART instance */
#define BMS_UART_INSTANCE UART_HAL_INSTANCE_0

/* BMS UART baud rate */
#define BMS_UART_BAUD_RATE 9600

/* BMS receive queue length */
#define BMS_RX_QUEUE_LEN 512

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* BMS */
bms_t bms;

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

// Queues
QueueHandle_t queue_uart_rx0;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_bms(void *arg);

void uart0_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			  lStatus_t status, void *userData);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
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

	/*--------------------------------- UART ---------------------------------*/
	uart_hal_cfg_t uart_cfg;
	uart_cfg.baudrate = BMS_UART_BAUD_RATE;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_NONE;
	uart_cfg.stopbits = UART_HAL_STOPBIT_1;

	uart_hal_fifo_cfg_t uart_fifo_cfg;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg);

	HAL_UART_Init(BMS_UART_INSTANCE, &uart_cfg);
	HAL_UART_ConfigureFIFO(BMS_UART_INSTANCE, &uart_fifo_cfg);
	HAL_UART_EnableInterrupt(BMS_UART_INSTANCE);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_bms, "bms", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

	vTaskStartScheduler();

	/* loop */
	while (1) {
	}
}

/* ================================== Tasks ================================= */

void task_blinky(void *arg)
{
	uint32_t *period_ms = (uint32_t *)arg;
	TickType_t last_wake = xTaskGetTickCount();
	uint8_t led_state = 0;

	for (;;) {
		vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(*period_ms));

		if (led_state)
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 25, 0);
		else
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 25, 1);

		led_state = !led_state;
	}
}

void task_bms(void *arg)
{
	uart_hal_handle_t uart_handle;

	uint8_t parse_byte;
	uint8_t receive_byte;

	/* create receive queue */
	queue_uart_rx0 = xQueueCreate(BMS_RX_QUEUE_LEN, sizeof(uint8_t));

	/* initialize BMS parser */
	bms_init(&bms);

	/* create transfer handle */
	HAL_UART_TransferCreateHandle(BMS_UART_INSTANCE, &uart_handle, uart0_cb,
								  &queue_uart_rx0);

	/* receive byte via interrupt */
	HAL_UART_ReceiveISR(BMS_UART_INSTANCE, &uart_handle, &receive_byte, 1);

	for (;;) {
		/* wait on queue */
		xQueueReceive(queue_uart_rx0, &parse_byte, portMAX_DELAY);

		/* parse byte */
		if (bms_parse(&bms, parse_byte) == 0) {
			HAL_GPIO_SetPinValue(
				GPIO_HAL_INSTANCE_3, 26,
				!HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26));
		}
	}
}

/* ============================= User Callbacks ============================= */

void uart0_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			  lStatus_t status, void *userData)
{
	BaseType_t taskWoken;
	QueueHandle_t *queue = (QueueHandle_t *)userData;

	if (handle->rxDataSize && status == lStatus_Success) {
		/* send received byte to the back of the queue */
		xQueueSendToBackFromISR(*queue, handle->rxData, &taskWoken);
		/* receive another byte via interrupt */
		HAL_UART_ReceiveISR(instance, handle, handle->rxData, 1);
	}

	portYIELD_FROM_ISR(taskWoken);
}

/* ============================ Private Functions =========================== */

/* ============================= FreeRTOS Hooks ============================= */

/* Increment a counter while Application is in Idle. */
void vApplicationIdleHook(void) { ulIdleCycleCount++; }

/* Callback to trap FreeRTOS Misconfiguration. */
void vAssertCalled(const char *pcFile, uint32_t ulLine)
{
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/* Callback to trap FreeRTOS stack overflow. */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
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