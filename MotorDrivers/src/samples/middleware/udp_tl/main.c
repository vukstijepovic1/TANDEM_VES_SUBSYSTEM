/**
 * @file    main.c
 * @brief   UDP Transport Layer Example.
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
#include "eth.h"
#include "gpio.h"
#include "util.h"

/* External hardware drivers */

/* Lib */
#include "sbus.h"

/* Middleware */
#include "FreeRTOS.h"
#include "task.h"

#include "udp.h"

#include "tl_common.h"
#include "udp_tl.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* physical layers */
eth_hal_phy_t eth_phy;

// Device information
const uint8_t dev_mac[6] = UDP_DEV_MAC;
const uint8_t dev_ip[4] = UDP_DEV_IP;

// Master UDP information
/* MAC should be configured to valid client MAC, if unknown can be broadcast */
const uint8_t master_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const uint8_t master_ip[4] = {192, 168, 1, 181};
const uint16_t master_port = 2049;
const uint16_t master_local_port = 2048;

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task UDP Variables */
udp_t udp_handle;
udp_tl_t udp_tl;
sbus_t sbus_handle;
uint32_t err_cnt = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_udp(void *arg);

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

	/*--------------------------------- ETH ----------------------------------*/
	HAL_ETH_Init(&eth_phy, dev_mac);

	/*--------------------------------- UDP ----------------------------------*/
	/* initialize UDP */
	udp_init(&udp_handle, dev_mac, dev_ip, (udp_phy_t *)&eth_phy);

	/* add master MAC & IP to static ARP table */
	udp_arptab_add(&udp_handle, master_mac, master_ip);

	/*-------------------------------- UDP TL --------------------------------*/
	udp_tl_init(&udp_tl, &udp_handle, (uint8_t *)master_ip, master_local_port,
				master_port);

	/*--------------------------------- SBUS ---------------------------------*/
	sbus_init(&sbus_handle, HAL_GetTimeUS);

	tl_on_recv_config(&udp_tl, sbus_on_recv, &sbus_handle);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_udp, "udp", 256, NULL, 1, NULL);

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

void task_udp(void *arg)
{
	for (;;) {
		/* process UDP data */
		udp_process(&udp_handle);
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
			uint8_t led_state;

			led_state = HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26);
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26, !led_state);

			uint8_t arr[SBUS_PACKET_SIZE];

			sbus_pack(arr, &sbus->data);

			tl_send(&udp_tl, arr, sizeof(arr));

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