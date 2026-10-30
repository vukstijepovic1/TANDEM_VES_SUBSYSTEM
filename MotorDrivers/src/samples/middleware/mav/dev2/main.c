/**
 * @file    main.c
 * @brief   MAVLink example
 * @version 1.0.0
 * @date    10.02.2025
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
#include "uart.h"
#include "util.h"

/* External hardware drivers */

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "task.h"

#include "uart_tl.h"

#include "udp.h"
#include "udp_tl.h"

#include "mav.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* Enable or disable MAVLink signing */
#define MAV_SIGN_ENABLE 1

/* UART which shall be used by MAVLink */
#define MAV_UART_INSTANCE UART_HAL_INSTANCE_2
#define MAV_UART_BAUD_RATE 230400

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Transport Layers */
eth_hal_phy_t eth_phy;
udp_tl_t udp_tl1;
udp_tl_t udp_tl2;
uart_tl_t uart_tl;

/* MAVLink */
static const uint8_t mav_key[32] = "********************************";

const uint8_t dev_mav_sysid = 0;
const uint8_t dev_mav_compid = 2;

const uint8_t client_mav_sysid = 0;
const uint8_t client_mav_compid = 1;

mav_t mav_handle;
mav_link_t mav_link_udp1;
mav_link_t mav_link_udp2;
mav_link_t mav_link_uart;

/* UDP */
udp_t udp;

// Device information
const uint8_t dev_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x01};
const uint8_t dev_ip[4] = {192, 168, 1, 101};

// Client UDP information
const uint8_t client_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x00};
const uint8_t client_ip[4] = {192, 168, 1, 100};
const uint16_t client_port1 = 2048;
const uint16_t client_port2 = 2049;
const uint16_t client_local_port1 = 2048;
const uint16_t client_local_port2 = 2049;

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

QueueSetHandle_t queueset_mav;
QueueHandle_t queue_mav_hb;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Misc */
uint32_t recv_cnt = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_service(void *arg);

void mav_router(void *arg);

void mav_recv_cb(mav_t *mav, mavlink_message_t *msg, void *arg);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
	/*------------------------------- FreeRTOS -------------------------------*/
	/* Queues */
	queue_mav_hb = xQueueCreate(2, sizeof(mavlink_heartbeat_t));

	/* Queuesets */
	queueset_mav = xQueueCreateSet(2);
	xQueueAddToSet(queue_mav_hb, queueset_mav);

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
	uart_cfg.baudrate = MAV_UART_BAUD_RATE;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_NONE;
	uart_cfg.stopbits = UART_HAL_STOPBIT_1;

	uart_hal_fifo_cfg_t uart_fifo_cfg;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg);

	// UART Init
	HAL_UART_Init(MAV_UART_INSTANCE, &uart_cfg);
	HAL_UART_ConfigureFIFO(MAV_UART_INSTANCE, &uart_fifo_cfg);
	HAL_UART_EnableInterrupt(MAV_UART_INSTANCE);

	/*------------------------------- UART TL --------------------------------*/
	uart_tl_init(&uart_tl, MAV_UART_INSTANCE, 1024, 1024);

	/*--------------------------------- ETH ----------------------------------*/
	HAL_ETH_Init(&eth_phy, dev_mac);

	/*--------------------------------- UDP ----------------------------------*/
	/* initialize UDP */
	udp_init(&udp, dev_mac, dev_ip, (udp_phy_t *)&eth_phy);

	/* add client MAC & IP to static ARP table */
	udp_arptab_add(&udp, client_mac, client_ip);

	/*-------------------------------- UDP TL --------------------------------*/
	udp_tl_init(&udp_tl1, &udp, (uint8_t *)client_ip, client_local_port1,
				client_port1);
	udp_tl_init(&udp_tl2, &udp, (uint8_t *)client_ip, client_local_port2,
				client_port2);

	/*--------------------------------- MAV ----------------------------------*/
	mav_init(&mav_handle, dev_mav_sysid, dev_mav_compid);

	mav_link_init(&mav_link_udp1, MAVLINK_COMM_0, mav_recv_cb, NULL,
				  (tl_t *)&udp_tl1);
	mav_link_init(&mav_link_udp2, MAVLINK_COMM_1, mav_recv_cb, NULL,
				  (tl_t *)&udp_tl2);
	mav_link_init(&mav_link_uart, MAVLINK_COMM_2, mav_recv_cb, NULL,
				  (tl_t *)&uart_tl);

	mav_link(&mav_handle, &mav_link_udp1);
	mav_link(&mav_handle, &mav_link_udp2);
	mav_link(&mav_handle, &mav_link_uart);

#if MAV_SIGN_ENABLE
	mav_sign(&mav_handle, mav_key, NULL);
#endif

	mav_track(&mav_handle, MAVLINK_MSG_ID_HEARTBEAT, client_mav_sysid,
			  client_mav_compid);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_service, "service", 256, NULL, 1, NULL);

	xTaskCreate(mav_router, "mav_router", 512, &mav_handle, 2, NULL);

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

		mavlink_heartbeat_t mav_hb = {.type = MAV_TYPE_GENERIC,
									  .autopilot = MAV_AUTOPILOT_GENERIC,
									  .base_mode = 0,
									  .custom_mode = 0,
									  .system_status = MAV_STATE_STANDBY};

		xQueueSendToBack(queue_mav_hb, &mav_hb, 0);
	}
}

void task_service(void *arg)
{
	for (;;) {
		/* process UDP data */
		udp_process(&udp);

		/* process UART data*/
		uart_tl_process(&uart_tl);
	}
}

void mav_router(void *arg)
{
	QueueSetMemberHandle_t queue_member = NULL;
	mavlink_message_t msg;
	mav_t *mav;
	uint8_t chan;

	mav = (mav_t *)arg;

	chan = mav_get_chan(mav);

	for (;;) {
		queue_member = xQueueSelectFromSet(queueset_mav, portMAX_DELAY);

		if (queue_member == queue_mav_hb) {
			mavlink_heartbeat_t data;

			xQueueReceive(queue_member, &data, 0);

			mavlink_msg_heartbeat_encode_chan(mav->sysid, mav->compid, chan,
											  &msg, &data);

			mav_send(mav, &msg);
		}
	}
}

/* ============================= User Callbacks ============================= */

void mav_recv_cb(mav_t *mav, mavlink_message_t *msg, void *arg)
{
	recv_cnt++;

	switch (msg->msgid) {
	case MAVLINK_MSG_ID_HEARTBEAT:
		HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26,
							 !HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26));
		break;
	}
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