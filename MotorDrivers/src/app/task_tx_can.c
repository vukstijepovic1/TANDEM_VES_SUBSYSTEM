/**
 * @file    task_tx_can.c
 * @brief   Task TX CAN - Drain queue and send CAN messages
 * @version 1.0.0
 * @date    09.02.2026
 * @author  LisumLab
 */

 /*******************************************************************************
 * Includes
 ******************************************************************************/

/* Standard types */
#include <stdint.h>

/* Program stuct */
#include "main.h"
#include "task_mav.h"
#include "types.h"

/* Peripherals */
#include "gpio.h"

/* External hardware drivers */
#include "cyphal_mavlink_publishers.h"
#include "cyphal_uavcan_publishers.h"


/* Lib */
#include "can.h"
#include "canard.h"
#include "cyphal_utility.h"

/* Middleware */
#include "FreeRTOS.h"
#include "mav.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* Transfer Layers */
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

/* CANARD DATA STRUCTURES*/
extern struct CanardInstance canard;
extern struct CanardTxQueue tx_queue;

extern SemaphoreHandle_t tx_queue_mutex;

int drain_tx_queue_result = 0;
static uint16_t counter = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void task_tx_can(void *arg)
{
    TickType_t last_wake = xTaskGetTickCount();

	for (;;) {

		vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1));

		TX_QUEUE_MUTEX_TAKE
		{
			drain_tx_queue_result =
				cyphal_drain_tx_queue(&canard, &tx_queue, 4);

			TX_QUEUE_MUTEX_GIVE;
		}

		if (drain_tx_queue_result == 0) {
			counter++;
			if (counter == 500) {
				counter = 0;
				HAL_GPIO_SetPinValue(
					GPIO_HAL_INSTANCE_3, 26,
					!HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 26));
			}
		}
		
		//vTaskDelay(pdMS_TO_TICKS(1));
	}

}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/