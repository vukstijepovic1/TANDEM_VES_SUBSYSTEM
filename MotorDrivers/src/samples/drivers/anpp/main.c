/**
 * @file    main.c
 * @brief   ANPP example.
 * @version 1.0.0
 * @date    02.04.2025
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
#include "anpp.h"

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "mavlink.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define ANPP_UART_INSTANCE UART_HAL_INSTANCE_1

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

QueueHandle_t queue_anpp;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task ANPP Variables */
mavlink_lisum_sensor_airspeed_data_t airspeed = {0};
anpp_t anpp;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_anpp(void *arg);

void uart_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			 lStatus_t status, void *userData);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
	/*------------------------------- FreeRTOS -------------------------------*/
	queue_anpp = xQueueCreate(512, sizeof(uint8_t));

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
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_1, 18, &gpio_cfg);

	/*--------------------------------- UART ---------------------------------*/
	uart_hal_cfg_t uart_cfg;
	uart_cfg.baudrate = 115200;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_NONE;
	uart_cfg.stopbits = UART_HAL_STOPBIT_1;

	uart_hal_fifo_cfg_t uart_fifo_cfg;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg);

	// UART Init
	HAL_UART_RS232Init(ANPP_UART_INSTANCE, &uart_cfg);
	HAL_UART_ConfigureFIFO(ANPP_UART_INSTANCE, &uart_fifo_cfg);
	HAL_UART_EnableInterrupt(ANPP_UART_INSTANCE);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_anpp, "anpp", 256, NULL, 2, NULL);

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

void task_anpp(void *arg)
{
	uart_hal_handle_t uart_handle;
	uint8_t process_byte;
	uint8_t receive_byte;
	int32_t ret = 0;
	uint32_t recv_status = 0x00;

	/* create new transfer handle */
	HAL_UART_TransferCreateHandle(ANPP_UART_INSTANCE, &uart_handle, uart_cb,
								  &queue_anpp);

	/* receive data from UART via ISR */
	HAL_UART_ReceiveISR(ANPP_UART_INSTANCE, &uart_handle, &receive_byte, 1);

	for (;;) {
		/* wait to receive data */
		xQueueReceive(queue_anpp, &process_byte, portMAX_DELAY);

		/* parse data */
		ret = anpp_parse(&anpp, process_byte);
		if (ret != 0)
			continue;

		if (anpp.id == ANPP_PACKET_ID_RAW_SENSORS) {
			/* process ANPP raw sensors packet */
			anpp_raw_sensor_t raw_sensor = {0};

			if (anpp_raw_sensors_decode(&anpp, &raw_sensor))
				continue;

			airspeed.raw_press = raw_sensor.diff_press * .01f;
			airspeed.temperature = (int16_t)(raw_sensor.temperature * 100);
			airspeed.id = 0x00;

			/* every sample starts with ANPP Raw Sensor Packet */
			recv_status = 0;
			recv_status |= (1 << 0);
		} else if (anpp.id == ANPP_PACKET_ID_AIR_DATA) {
			/* process ANPP air data packet */
			anpp_air_data_t air_data = {0};

			if (anpp_air_data_decode(&anpp, &air_data))
				continue;

			airspeed.airspeed = air_data.true_airspeed;

			recv_status |= (1 << 1);
		}

		if (recv_status == 0x03) {
			/* all expected data is received */
			recv_status = 0x00;
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_1, 18, 1);
			vTaskDelay(pdMS_TO_TICKS(5));
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_1, 18, 0);
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