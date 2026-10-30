/**
 * @file    main.c
 * @brief   UDP Example
 * @version 1.0.0
 * @date    03.02.2025
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

/* Middleware */
#include "FreeRTOS.h"
#include "task.h"

#include "udp.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define UDP_SEND_ARP_GRAT

#ifdef UDP_SEND_ARP_GRAT
#define UDP_ARP_GRAT_PERIOD_MS 10000
#endif

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
uint32_t err_cnt = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_udp(void *arg);

int udp_master_recv_cb(udp_t *udp, uint8_t *data, uint16_t size, void *arg);

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

	/* bind source IP and local port to receive callback */
	udp_track(&udp_handle, master_ip, master_local_port, udp_master_recv_cb,
			  NULL);

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
#ifdef UDP_SEND_ARP_GRAT
	TickType_t last_arp_grat = xTaskGetTickCount();
#endif

	for (;;) {
		/* process UDP data */
		udp_process(&udp_handle);

#ifdef UDP_SEND_ARP_GRAT
		/* send gratuitous ARP */
		if (xTaskGetTickCount() - last_arp_grat >= UDP_ARP_GRAT_PERIOD_MS) {
			last_arp_grat = xTaskGetTickCount();
			udp_arp_grat(&udp_handle);
		}
#endif
	}
}

/* ============================= User Callbacks ============================= */

int udp_master_recv_cb(udp_t *udp, uint8_t *data, uint16_t size, void *arg)
{
	/* echo data back & toggle diode */
	udp_send(udp, master_ip, master_port, data, size);
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26,
						 !HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26));

	return 0;
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