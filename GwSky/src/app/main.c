/**
 * @file    main.c
 * @brief   Gateway Sky project.
 * @details This project acquires data from aicraft subsystems (Actuator Master,
 *          INS, INS_COTS and Power Management) and transmits it to gateway
 *          ground via multiple links (5GHz and 915MHz). Also deduplicates
 *          messages received via said links and routes them to the desired
 *          destinations.
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
#include "task_tx_can.h"
#include "types.h"

/* Peripherals */
#include "can.h"
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
const uint8_t dev_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x00};
const uint8_t dev_ip[4] = {192, 168, 1, 100};

// gateway ground UDP information
const uint8_t gw_gnd_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x05, 0x00};
const uint8_t gw_gnd_ip[4] = {192, 168, 1, 150};
const uint16_t gw_gnd_port = 2048;
const uint16_t gw_gnd_local_port = 2048;

#if MAVLINK_OR_CYPHAL
// power management UDP information
const uint8_t pwr_man_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x01};
const uint8_t pwr_man_ip[4] = {192, 168, 1, 101};
const uint16_t pwr_man_port = 1001;
const uint16_t pwr_man_local_port = 1001;

// black box UDP information
const uint8_t black_box_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x02};
const uint8_t black_box_ip[4] = {192, 168, 1, 102};
const uint16_t black_box_port = 1002;
const uint16_t black_box_local_port = 1002;

// inertial navigation system UDP information
const uint8_t ins_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x03};
const uint8_t ins_ip[4] = {192, 168, 1, 103};
const uint16_t ins_port = 1003;
const uint16_t ins_local_port = 1003;

// actuator master UDP information
const uint8_t act_master_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x00, 0x04};
const uint8_t act_master_ip[4] = {192, 168, 1, 104};
const uint16_t act_master_port = 1004;
const uint16_t act_master_local_port = 1004;

// inertial navigation system cots UDP information
const uint8_t ins_cots_mac[6] = {0x4C, 0x50, 0x43, 0x01, 0x01, 0x03};
const uint8_t ins_cots_ip[4] = {192, 168, 1, 113};
const uint16_t ins_cots_port = 1013;
const uint16_t ins_cots_local_port = 1013;

#endif /* MAVLINK_OR_CYPHAL */

/* Transport layers */
eth_hal_phy_t eth_phy;
// uart_tl_t mav_gw_gnd_uart_tl;
udp_tl_t mav_gw_gnd_udp_tl;

#if MAVLINK_OR_CYPHAL

udp_tl_t mav_pwr_man_tl;
udp_tl_t mav_black_box_tl;
udp_tl_t mav_ins_tl;
udp_tl_t mav_act_master_tl;
udp_tl_t mav_ins_cots_tl;

#endif /* MAVLINK_OR_CYPHAL */

/* UDP */
udp_t udp;

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;
// Mutexes
SemaphoreHandle_t mutex_mav;
SemaphoreHandle_t tx_queue_mutex;
// Timers
TimerHandle_t timer_blinky;
// Queues
QueueHandle_t queue_mav_hb;
QueueHandle_t queue_cyphal_rx;
// QueueSets
QueueSetHandle_t queueset_mav;

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
	mutex_mav = xSemaphoreCreateMutex();
	tx_queue_mutex = xSemaphoreCreateMutex();

	/* Queues */
	queue_mav_hb = xQueueCreate(2, sizeof(mavlink_file_transfer_protocol_t));
	queue_cyphal_rx = xQueueCreate(10, sizeof(can_hal_msg_t));

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
	
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 13, &gpio_cfg);
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 13, 0);



	/*--------------------------------- UART ---------------------------------*/
	uart_hal_cfg_t uart_cfg;
	uart_cfg.baudrate = TELEMETRY_UART_BAUD_RATE;
	uart_cfg.databits = UART_HAL_DATABIT_8;
	uart_cfg.parity = UART_HAL_PARITY_NONE;
	uart_cfg.stopbits = UART_HAL_STOPBIT_1;

	uart_hal_fifo_cfg_t uart_fifo_cfg;
	HAL_UART_FIFOConfigStructInit(&uart_fifo_cfg);

	// UART Init
	HAL_UART_Init(TELEMETRY_UART_INSTANCE, &uart_cfg);
	HAL_UART_ConfigureFIFO(TELEMETRY_UART_INSTANCE, &uart_fifo_cfg);
	HAL_UART_EnableInterrupt(TELEMETRY_UART_INSTANCE);

	/*------------------------------- UART TL --------------------------------*/
	// uart_tl_init(&mav_gw_gnd_uart_tl, TELEMETRY_UART_INSTANCE, 1024, 1024);

	/* --------------------------------- CAN ------------------------------ */
	HAL_CAN_Init(CAN_HAL_INSTANCE_0, 1000000);

	HAL_CAN_EnableInterrupt(CAN_HAL_INSTANCE_0, cyphal_cb, NULL);

	HAL_CAN_IntCmd(CAN_HAL_INSTANCE_0, CAN_HAL_INT_RI, lFunctionalState_Enable);

	/*--------------------------------- ETH ----------------------------------*/
	HAL_ETH_Init(&eth_phy, dev_mac);

	/*--------------------------------- UDP ----------------------------------*/
	/* initialize UDP */
	udp_init(&udp, dev_mac, dev_ip, (udp_phy_t*)&eth_phy);

	/* add client MAC & IP to static ARP table */
	udp_arptab_add(&udp, gw_gnd_mac, gw_gnd_ip);

#if MAVLINK_OR_CYPHAL

	udp_arptab_add(&udp, pwr_man_mac, pwr_man_ip);

	udp_arptab_add(&udp, black_box_mac, black_box_ip);

	udp_arptab_add(&udp, ins_mac, ins_ip);

	udp_arptab_add(&udp, act_master_mac, act_master_ip);

	udp_arptab_add(&udp, ins_cots_mac, ins_cots_ip);

#endif /* MAVLINK_OR_CYPHAL */

	/*-------------------------------- UDP TL --------------------------------*/
	udp_tl_init(&mav_gw_gnd_udp_tl, &udp, (uint8_t*)gw_gnd_ip, gw_gnd_local_port, gw_gnd_port);

#if MAVLINK_OR_CYPHAL

	udp_tl_init(&mav_pwr_man_tl, &udp, (uint8_t*)pwr_man_ip, pwr_man_local_port, pwr_man_port);

	udp_tl_init(&mav_black_box_tl, &udp, (uint8_t*)black_box_ip, black_box_local_port,
				black_box_port);

	udp_tl_init(&mav_ins_tl, &udp, (uint8_t*)ins_ip, ins_local_port, ins_port);

	udp_tl_init(&mav_act_master_tl, &udp, (uint8_t*)act_master_ip, act_master_local_port,
				act_master_port);

	udp_tl_init(&mav_ins_cots_tl, &udp, (uint8_t*)ins_cots_ip, ins_cots_local_port, ins_cots_port);

#endif /* MAVLINK_OR_CYPHAL */

	/*------------------------------- FreeRTOS -------------------------------*/
	/* work task - lowest priority */
	xTaskCreate(task_work, "work", 768, NULL, 1, NULL);

	/* MAVLink task - medium priority */
	xTaskCreate(task_mav, "mav", 768, NULL, 2, NULL);

	/* TX task - medium priority */
	xTaskCreate(task_tx_can, "tx_can", 256, NULL, 2, NULL);

	/* blinky timer */
	timer_blinky =
		xTimerCreate("tim_blinky", pdMS_TO_TICKS(BLINKY_PERIOD_MS), pdTRUE, NULL, timer_blinky_cb);

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
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26, !HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26));
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