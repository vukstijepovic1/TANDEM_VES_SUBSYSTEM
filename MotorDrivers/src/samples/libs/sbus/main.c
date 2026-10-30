/**
 * @file    main.c
 * @brief   SBUS Example.
 * @version 1.0.0
 * @date    22.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <string.h>

/* Peripherals */
#include "common.h"
#include "gpio.h"
#include "uart.h"
#include "util.h"

/* External hardware drivers */

/* Lib */
#include "sbus.h"

/* Middleware */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;
QueueHandle_t queue_uart_rx0;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task SBUS Variables */
sbus_t sbus;
uint32_t err_cnt = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_sbus(void *arg);

void uart_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			 lStatus_t status, void *userData);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
	/*------------------------------- FreeRTOS -------------------------------*/
	queue_uart_rx0 = xQueueCreate(512, sizeof(uint8_t));

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
	uart_cfg.baudrate = 100000;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_EVEN;
	uart_cfg.stopbits = UART_HAL_STOPBIT_2;

	uart_hal_fifo_cfg_t uart_fifo_cfg;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg);

	// UART Init
	HAL_UART_Init(UART_HAL_INSTANCE_0, &uart_cfg);
	HAL_UART_ConfigureFIFO(UART_HAL_INSTANCE_0, &uart_fifo_cfg);
	HAL_UART_EnableInterrupt(UART_HAL_INSTANCE_0);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_sbus, "SBUS", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

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

void task_sbus(void *arg)
{
	uart_hal_handle_t uart_handle;
	uint8_t process_byte;
	uint8_t receive_byte;
	uint8_t led_state = 0;
	int32_t ret = 0;

	HAL_UART_TransferCreateHandle(UART_HAL_INSTANCE_0, &uart_handle, uart_cb,
								  &queue_uart_rx0);

	HAL_UART_ReceiveISR(UART_HAL_INSTANCE_0, &uart_handle, &receive_byte, 1);

	sbus_init(&sbus, HAL_GetTimeUS);

	for (;;) {
		xQueueReceive(queue_uart_rx0, &process_byte, portMAX_DELAY);

		ret = sbus_parse(&sbus, process_byte);

		if (ret == 1) {
			if (led_state)
				HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26, 0);
			else
				HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26, 1);

			led_state = !led_state;

			// Pack new SBUS packet and compare it to original.
			uint8_t arr[SBUS_PACKET_SIZE];

			sbus_pack(arr, &sbus.data);

			if (memcmp(arr, sbus.arr, sizeof(arr)) != 0)
				err_cnt++;

		} else if (ret == -1) {
			err_cnt++;
		}
	}
}

/* ============================= User Callbacks ============================= */

void uart_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			 lStatus_t status, void *userData)
{
	BaseType_t taskWoken;
	QueueHandle_t *queue = (QueueHandle_t *)userData;

	if (handle->rxDataSize && status == lStatus_Success) {
		xQueueSendToBackFromISR(*queue, handle->rxData, &taskWoken);
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