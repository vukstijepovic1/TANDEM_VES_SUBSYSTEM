/**
 * @file    main.c
 * @brief   UART Transport Layer Example.
 * @version 1.0.0
 * @date    27.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/* Peripherals */
#include "common.h"
#include "gpio.h"
#include "util.h"

/* External hardware drivers */

/* Lib */
#include "sbus.h"

/* Middleware */
#include "FreeRTOS.h"
#include "task.h"

#include "tl_common.h"
#include "uart_tl.h"

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

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task SBUS Variables */
sbus_t sbus_handle;
uint32_t err_cnt = 0;
uint8_t green_led_state = 0;

uart_tl_t uart_tl;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_service(void *arg);

static uint16_t sbus_on_recv(void *arg, uint8_t *buffer, uint16_t size);

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

	/*------------------------------- UART TL --------------------------------*/
	uart_tl_init(&uart_tl, UART_HAL_INSTANCE_0, 256, 256);

	/*--------------------------------- SBUS ---------------------------------*/
	sbus_init(&sbus_handle, HAL_GetTimeUS);

	tl_on_recv_config(&uart_tl, sbus_on_recv, &sbus_handle);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_service, "service", 256, NULL, 1, NULL);

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

void task_service(void *arg)
{
	for (;;) {
		/* Process UART Transport Layer Data. */
		uart_tl_process(&uart_tl);
	}
}

/* ============================= User Callbacks ============================= */

static uint16_t sbus_on_recv(void *arg, uint8_t *buffer, uint16_t size)
{
	sbus_t *sbus;

	sbus = (sbus_t *)arg;

	for (uint32_t i = 0; i < size; i++) {
		int ret;

		ret = sbus_parse(sbus, buffer[i]);

		if (ret == 1) {
			/* Blink LED */
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26, green_led_state);
			green_led_state = !green_led_state;

			uint8_t arr[SBUS_PACKET_SIZE + 2];

			sbus_pack(arr, &sbus->data);
			/*
			 * Add CR LF to the end of the packet so that ser.readline() may be
			 * used in test python script.
			 */
			arr[25] = 13;
			arr[26] = 10;

			if (tl_send(&uart_tl, arr, sizeof(arr)) != sizeof(arr))
				err_cnt++;

		} else if (ret == -1) {
			err_cnt++;
		}
	}

	return size;
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