/**
 * @file    task_work.c
 * @brief   Task work - process background tasks
 * @version 1.0.0
 * @date    09.04.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "task_work.h"

/* Peripherals */

/* External hardware drivers */

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "task.h"

#include "dev_config.h"

#include "uart_tl.h"

#include "udp.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* gratuitous ARP period [ms] */
#define ARP_GRAT_PERIOD_MS 10000

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern uart_tl_t mav_gw_sky_uart_tl;
extern uart_tl_t mav_gs_mp_uart_tl;
extern udp_t udp;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void task_work(void *arg)
{
	TickType_t arp_grat_time = xTaskGetTickCount();

	for (;;) {
		/* process UDP data */
		udp_process(&udp);
		/* process UART transport layer data */
		//uart_tl_process(&mav_gw_sky_uart_tl);

		/* process MISSION PLANNER UART transport layer data */
	#if MISSION_PLANNER_UART_TRANSPORT_LAYER

		uart_tl_process(&mav_gs_mp_uart_tl);

	#endif

		/* send gratuitous ARP */
		if (xTaskGetTickCount() - arp_grat_time >= ARP_GRAT_PERIOD_MS) {
			arp_grat_time = xTaskGetTickCount();

			udp_arp_grat(&udp);
		}
	}
}
