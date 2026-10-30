/**
 * @file    main.c
 * @brief   UBX example.
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
#include "ubx_parser.h"

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

#define UBX_UART_INSTANCE UART_HAL_INSTANCE_2
// #define DISABLE_SYNC

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

QueueHandle_t queue_uart_rx3;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task UBX Variables */
mavlink_lisum_gnss_recv_data_t gnss_data = {0};
ubx_t ubx;
uint32_t pvt_cnt = 0;
uint32_t hpposllh_cnt = 0;
uint32_t relposned_cnt = 0;
uint32_t unknown_cnt = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_ubx(void *arg);

void uart_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			 lStatus_t status, void *userData);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
	/*------------------------------- FreeRTOS -------------------------------*/
	queue_uart_rx3 = xQueueCreate(512, sizeof(uint8_t));

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
	uart_cfg.baudrate = 230400;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_NONE;
	uart_cfg.stopbits = UART_HAL_STOPBIT_1;

	uart_hal_fifo_cfg_t uart_fifo_cfg;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg);

	// UART Init
	HAL_UART_Init(UBX_UART_INSTANCE, &uart_cfg);
	HAL_UART_ConfigureFIFO(UBX_UART_INSTANCE, &uart_fifo_cfg);
	HAL_UART_EnableInterrupt(UBX_UART_INSTANCE);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_ubx, "ubx", 256, NULL, 2, NULL);

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

void task_ubx(void *arg)
{
	uart_hal_handle_t uart_handle;
	uint8_t process_byte;
	uint8_t receive_byte;
	int32_t ret = 0;
	uint32_t recv_status = 0x00;

#ifndef DISABLE_SYNC
	uint32_t itow = 0;
#endif

	/* create new transfer handle */
	HAL_UART_TransferCreateHandle(UBX_UART_INSTANCE, &uart_handle, uart_cb,
								  &queue_uart_rx3);

	/* receive data from UART via ISR */
	HAL_UART_ReceiveISR(UBX_UART_INSTANCE, &uart_handle, &receive_byte, 1);

	/* initialize UBX */
	ubx_init(&ubx);

	for (;;) {
		/* wait to receive data */
		xQueueReceive(queue_uart_rx3, &process_byte, portMAX_DELAY);

		/* parse data */
		ret = ubx_parse(&ubx, process_byte);
		if (ret != 0)
			continue;

		/* process only packets from NAV class */
		if (ubx.packet_class != UBX_PACKET_CLASS_NAV)
			continue;

		if (ubx.packet_id == UBX_PACKET_ID_NAV_PVT) {
			/* process UBX Navigation position velocity time solution. */
			ubx_nav_pvt_t pvt;

			ubx_nav_pvt_decode(&ubx, &pvt);
			pvt_cnt++;

			gnss_data.head = pvt.headmot * 1e-5;
			gnss_data.vel_ne = pvt.gspeed * 1e-3;
			gnss_data.vel_d = pvt.veld * 1e-3;
			gnss_data.fix_type = pvt.fixtype;
			gnss_data.carr_sol = (pvt.flags & 0xC0) >> 6;
			gnss_data.sv_num = pvt.numsv;

#ifndef DISABLE_SYNC
			if (itow != pvt.itow) {
				itow = pvt.itow;
				recv_status = 0x00;
			}
#endif

			recv_status |= 0x01;
		} else if (ubx.packet_id == UBX_PACKET_ID_NAV_HPPOSLLH) {
			/* process UBX High precision geodetic position solution. */
			ubx_nav_hpposllh_t hpposllh;

			ubx_nav_hpposllh_decode(&ubx, &hpposllh);
			hpposllh_cnt++;

			int64_t tmp;

			tmp = (int64_t)hpposllh.lon * 100 + hpposllh.lonhp;
			gnss_data.lon = tmp * 1e-9;
			tmp = (int64_t)hpposllh.lat * 100 + hpposllh.lathp;
			gnss_data.lat = tmp * 1e-9;
			tmp = (int64_t)hpposllh.hmsl * 10 + hpposllh.hmslhp;
			gnss_data.alt = tmp * 1e-4;

#ifndef DISABLE_SYNC
			if (itow != hpposllh.itow) {
				itow = hpposllh.itow;
				recv_status = 0x00;
			}
#endif

			recv_status |= 0x02;
		} else if (ubx.packet_id == UBX_PACKET_ID_NAV_RELPOSNED) {
			/* process UBX Relative positioning information in NED frame. */
			ubx_nav_relposned_t relposned;

			ubx_nav_relposned_decode(&ubx, &relposned);
			relposned_cnt++;

			gnss_data.head_rtk = relposned.relposheading * 1e-5;
			gnss_data.rtk_head_valid = (relposned.flags & 0x0100) >> 8;

#ifndef DISABLE_SYNC
			if (itow != relposned.itow) {
				itow = relposned.itow;
				recv_status = 0x00;
			}
#endif

			recv_status |= 0x04;
		} else {
			/* untracked UBX packet received */
			unknown_cnt++;
		}

		if (recv_status == 0x07) {
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