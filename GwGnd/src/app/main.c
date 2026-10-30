/**
 * @file    main.c
 * @brief   Gateway Ground project.
 * @details This project acquires data from ground station subsystems
 *          (Helicopter, Motor, FligtGear and Maps applications) and transmits
 *          it to gateway sky via multiple links (5GHz and 915MHz). Also
 *          deduplicates messages received via said links and routes them to the
 *          desired destinations.
 * @version 1.0.0
 * @date    09.04.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "task_mav.h"
#include "task_work.h"
// #include "task_ftp.h" //just fot testing
#include "dev_config.h"
#include "types.h"

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
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "uart_tl.h"

#include "udp.h"
#include "udp_tl.h"

#include "mav.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Temporary variables */
uint64_t boot_time_ms = 0;

/* Device information */
// UDP
const uint8_t dev_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x05, 0x00};
const uint8_t dev_ip[4] = {192, 168, 1, 150};

// gateway ground UDP information
const uint8_t gw_sky_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x00};
const uint8_t gw_sky_ip[4] = {192, 168, 1, 100};
const uint16_t gw_sky_port = 2048;
const uint16_t gw_sky_local_port = 2048;

// ground station helicopter application UDP information
const uint8_t gs_heli_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t gs_heli_ip[4] = {192, 168, 1, 151};
const uint16_t gs_heli_port = 1024;
const uint16_t gs_heli_local_port = 1024;

// ground station motor application UDP information
const uint8_t gs_motor_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t gs_motor_ip[4] = {192, 168, 1, 152};
const uint16_t gs_motor_port = 1024;
const uint16_t gs_motor_local_port = 1024;

// ground station flight gear application UDP information
const uint8_t gs_fg_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t gs_fg_ip[4] = {192, 168, 1, 153};
const uint16_t gs_fg_port = 1024;
const uint16_t gs_fg_local_port = 1024;

// ground station maps application UDP information
const uint8_t gs_maps_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t gs_maps_ip[4] = {192, 168, 1, 154};
const uint16_t gs_maps_port = 1024;
const uint16_t gs_maps_local_port = 1024;

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER
// Mission Planner application UDP information
const uint8_t gs_mp_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t gs_mp_ip[4] = {192, 168, 1, 155};
const uint16_t gs_mp_port = 1024;
const uint16_t gs_mp_local_port = 1024;
#endif

/* ---------------------------- Transport layers ---------------------------*/

/* UART Transport layers */
#if MISSION_PLANNER_UART_TRANSPORT_LAYER
uart_tl_t mav_gs_mp_uart_tl;
#endif
uart_tl_t mav_gw_sky_uart_tl;

/* UDP transport layers */
#if MISSION_PLANNER_UDP_TRANSPORT_LAYER
udp_tl_t mav_gs_mp_tl;
#endif
udp_tl_t mav_gw_sky_udp_tl;
udp_tl_t mav_heli_tl;
udp_tl_t mav_motor_tl;
udp_tl_t mav_fg_tl;
udp_tl_t mav_maps_tl;
udp_tl_t mav_bb_tl;

/* Ethernet PHY */
eth_hal_phy_t eth_phy;

/* UDP */
udp_t udp;

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

// Mutexes
SemaphoreHandle_t mutex_mav;
// Timers
TimerHandle_t timer_blinky;
// Queues
QueueHandle_t queue_mav_hb;
// QueueSets
QueueSetHandle_t queueset_mav;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void timer_blinky_cb(TimerHandle_t xTimer);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()

{
	/*------------------------------- FreeRTOS -------------------------------*/
	/* Mutexes */
	mutex_mav = xSemaphoreCreateMutex();

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
	uart_cfg.baudrate = TELEMETRY_UART_BAUD_RATE;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_NONE;
	uart_cfg.stopbits = UART_HAL_STOPBIT_1;

	uart_hal_fifo_cfg_t uart_fifo_cfg;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg);

	// Telemetry UART Init
	HAL_UART_Init(TELEMETRY_UART_INSTANCE, &uart_cfg);
	HAL_UART_ConfigureFIFO(TELEMETRY_UART_INSTANCE, &uart_fifo_cfg);
	HAL_UART_EnableInterrupt(TELEMETRY_UART_INSTANCE);

	/*------------------------ UART Mission Planner --------------------------*/

#if MISSION_PLANNER_UART_TRANSPORT_LAYER

	uart_hal_cfg_t uart_cfg_mp;
	uart_cfg_mp.baudrate = MISSION_PLANNER_UART_BAUD_RATE;
	uart_cfg_mp.databits = UART_HAL_DATABIT_8;
	uart_cfg_mp.parity = UART_HAL_PARITY_NONE;
	uart_cfg_mp.stopbits = UART_HAL_STOPBIT_1;

	uart_hal_fifo_cfg_t uart_fifo_cfg_mp;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg_mp);

	// Telemetry UART Init
	HAL_UART_Init(MISSION_PLANNER_UART_INSTANCE, &uart_cfg_mp);
	HAL_UART_ConfigureFIFO(MISSION_PLANNER_UART_INSTANCE, &uart_fifo_cfg_mp);
	HAL_UART_EnableInterrupt(MISSION_PLANNER_UART_INSTANCE);

#endif

	/*------------------------------- UART TL --------------------------------*/
	// uart_tl_init(&mav_gw_sky_uart_tl, TELEMETRY_UART_INSTANCE,
	// 			 TELEMETRY_RX_QUEUE_LEN, TELEMETRY_TX_QUEUE_LEN);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER

	uart_tl_init(&mav_gs_mp_uart_tl, MISSION_PLANNER_UART_INSTANCE,
				 MISSION_PLANNER_RX_QUEUE_LEN, MISSION_PLANNER_TX_QUEUE_LEN);

#endif

	/*--------------------------------- ETH ----------------------------------*/
	HAL_ETH_Init(&eth_phy, dev_mac);

	/*--------------------------------- UDP ----------------------------------*/
	/* initialize UDP */
	udp_init(&udp, dev_mac, dev_ip, (udp_phy_t*)&eth_phy);

	/* add client MAC & IP to static ARP table */
	udp_arptab_add(&udp, gw_sky_mac, gw_sky_ip);

	udp_arptab_add(&udp, gs_heli_mac, gs_heli_ip);

	udp_arptab_add(&udp, gs_motor_mac, gs_motor_ip);

	udp_arptab_add(&udp, gs_fg_mac, gs_fg_ip);

	udp_arptab_add(&udp, gs_maps_mac, gs_maps_ip);

#if MISSION_PLANNER_UDP_TRANSPORT_LAYER

	udp_arptab_add(&udp, gs_mp_mac, gs_mp_ip);

#endif

	/*-------------------------------- UDP TL --------------------------------*/
	udp_tl_init(&mav_gw_sky_udp_tl, &udp, (uint8_t*)gw_sky_ip,
				gw_sky_local_port, gw_sky_port);

	udp_tl_init(&mav_heli_tl, &udp, (uint8_t*)gs_heli_ip, gs_heli_local_port,
				gs_heli_port);

	udp_tl_init(&mav_motor_tl, &udp, (uint8_t*)gs_motor_ip, gs_motor_local_port,
				gs_motor_port);

	udp_tl_init(&mav_fg_tl, &udp, (uint8_t*)gs_fg_ip, gs_fg_local_port,
				gs_fg_port);

	udp_tl_init(&mav_maps_tl, &udp, (uint8_t*)gs_maps_ip, gs_maps_local_port,
				gs_maps_port);

#if MISSION_PLANNER_UDP_TRANSPORT_LAYER

	udp_tl_init(&mav_gs_mp_tl, &udp, (uint8_t*)gs_mp_ip, gs_mp_local_port,
				gs_mp_port);

#endif

	/*------------------------------- FreeRTOS -------------------------------*/
	/* work task - lowest priority */
	xTaskCreate(task_work, "work", 768, NULL, 1, NULL);

	/* MAVLink task - medium priority */
	xTaskCreate(task_mav, "mav", 768, NULL, 2, NULL);

	/* blinky timer */
	timer_blinky = xTimerCreate("tim_blinky", pdMS_TO_TICKS(BLINKY_PERIOD_MS),
								pdTRUE, NULL, timer_blinky_cb);

	xTimerStart(timer_blinky, 0);

	/* Time past from initialization (we will use as a boot time for MAV) */
	boot_time_ms = HAL_GetTimeUS() / 1000;

	vTaskStartScheduler();

	/* loop */
	while (1) {
	}
}

/* ================================== Tasks ================================= */

/* ============================= User Callbacks ============================= */

static void timer_blinky_cb(TimerHandle_t xTimer)
{
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26,
						 !HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26));
}

/* ============================ Private Functions =========================== */

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