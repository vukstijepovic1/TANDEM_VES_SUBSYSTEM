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
#include <string.h>

#include "task_work.h"
#include "types.h"

/* Peripherals */

/* External hardware drivers */

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "mav.h"
#include "queue.h"
#include "timers.h"

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
extern SemaphoreHandle_t mutex_mav;

extern uart_tl_t mav_gw_gnd_uart_tl;
extern udp_t udp;

#if !MAVLINK_OR_CYPHAL

extern const uint8_t dev_mav_sysid;
extern const uint8_t dev_mav_compid;

extern struct CanardInstance canard;
extern struct CanardTxQueue tx_queue;

extern cyphal_nodes_ids_t cyphal_nodes_ids;

extern mav_t mav_gw_gnd_handle;

extern uint16_t ftp_seq_num;


static mavlink_message_t tx_msg;
static mavlink_heartbeat_t mavlink_heartbeat_data;

extern QueueHandle_t queue_cyphal_rx;
static uint32_t lost_counter = 0;

#endif /* MAVLINK_OR_CYPHAL */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#if !MAVLINK_OR_CYPHAL

static void handle_cyphal_transfer(const struct CanardRxTransfer* tr);

#endif /* MAVLINK_OR_CYPHAL */

/*******************************************************************************
 * Code
 ******************************************************************************/
void task_work(void* arg)
{
	TickType_t arp_grat_time = xTaskGetTickCount();
	can_hal_msg_t rx;

	for (;;) {
		/* process UDP data */
		udp_process(&udp);

		/* send gratuitous ARP periodically */
		if (xTaskGetTickCount() - arp_grat_time >= ARP_GRAT_PERIOD_MS) {
			arp_grat_time = xTaskGetTickCount();
			udp_arp_grat(&udp);
		}

#if !MAVLINK_OR_CYPHAL
		/* Praznimo queue dokle god ima poruka */
		while (xQueueReceive(queue_cyphal_rx, &rx, 0) == pdPASS) {
			if (rx.idFormat != CAN_HAL_ID_FORMAT_EXT)
				continue;

			struct CanardFrame fr = {.extended_can_id = rx.messageId,
									 .payload = {.size = rx.length, .data = rx.data}};

			const CanardMicrosecond ts = (CanardMicrosecond)HAL_GetTimeUS();
			struct CanardRxTransfer tr = {0};
			struct CanardRxSubscription* sub = NULL;

			int8_t acc = canardRxAccept(&canard, ts, &fr, 0, &tr, &sub);
			if (acc > 0) {
				handle_cyphal_transfer(&tr);
				if (tr.payload.allocated_size > 0) {
					canard.memory.deallocate(canard.memory.user_reference,
											 tr.payload.allocated_size, tr.payload.data);
				}
			}
		}
#endif
	}
}

#if !MAVLINK_OR_CYPHAL

static void handle_cyphal_transfer(const struct CanardRxTransfer* tr)
{
	xSemaphoreTake(mutex_mav, portMAX_DELAY);

	switch (tr->metadata.port_id) {

	case messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_FIXED_PORT_ID_  :{

		mavlink_lisum_vesc_status_1_t vesc_msg;

		messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 resp;
		
		memset(&resp, 0, sizeof(resp));

		memset(&tx_msg, 0, sizeof(tx_msg));
		
		size_t sz = tr -> payload.size;
		int8_t rc = messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_deserialize_(
			&resp, (const uint8_t*)tr->payload.data, &sz);

		if (rc == 0) {

			vesc_msg.duty          = resp.duty;
			vesc_msg.total_current = resp._total_current;
			vesc_msg.erpm          = resp.erpm;
			vesc_msg.vesc_id       = resp.vesc_id;

			mavlink_msg_lisum_vesc_status_1_encode_chan(dev_mav_sysid, dev_mav_compid,
														 MAVLINK_COMM_0, &tx_msg,
														 &vesc_msg);

			mav_send(&mav_gw_gnd_handle, &tx_msg);
		}

		break;

	}

	case messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_FIXED_PORT_ID_  :{

		mavlink_lisum_vesc_status_2_t vesc_msg;

		messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 resp;
		
		memset(&resp, 0, sizeof(resp));

		memset(&tx_msg, 0, sizeof(tx_msg));
		
		size_t sz = tr -> payload.size;
		int8_t rc = messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_deserialize_(
			&resp, (const uint8_t*)tr->payload.data, &sz);

		if (rc == 0) {

			vesc_msg.amp_hours_charged = resp.amp_hours_charged;
			vesc_msg.amp_hours         = resp.amp_hours;
			vesc_msg.vesc_id           = resp.vesc_id;


			mavlink_msg_lisum_vesc_status_2_encode_chan(dev_mav_sysid, dev_mav_compid,
														 MAVLINK_COMM_0, &tx_msg,
														 &vesc_msg);

			mav_send(&mav_gw_gnd_handle, &tx_msg);
		}

		break;

	}

	case messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_FIXED_PORT_ID_  :{

		mavlink_lisum_vesc_status_3_t vesc_msg;

		messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 resp;
		
		memset(&resp, 0, sizeof(resp));

		memset(&tx_msg, 0, sizeof(tx_msg));
		
		size_t sz = tr -> payload.size;
		int8_t rc = messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_deserialize_(
			&resp, (const uint8_t*)tr->payload.data, &sz);

		if (rc == 0) {

			vesc_msg.watt_hours_charged = resp.watt_hours_charged;
			vesc_msg.watt_hours         = resp.watt_hours;
			vesc_msg.vesc_id            = resp.vesc_id;


			mavlink_msg_lisum_vesc_status_3_encode_chan(dev_mav_sysid, dev_mav_compid,
														 MAVLINK_COMM_0, &tx_msg,
														 &vesc_msg);

			mav_send(&mav_gw_gnd_handle, &tx_msg);
		}

		break;

	}
		
	case messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_FIXED_PORT_ID_  :{

		mavlink_lisum_vesc_status_4_t vesc_msg;

		messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 resp;
		
		memset(&resp, 0, sizeof(resp));

		memset(&tx_msg, 0, sizeof(tx_msg));
		
		size_t sz = tr -> payload.size;
		int8_t rc = messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_deserialize_(
			&resp, (const uint8_t*)tr->payload.data, &sz);

		if (rc == 0) {

			vesc_msg.current_in = resp.current_in;
			vesc_msg.pos        = resp.pos;
			vesc_msg.fet_temp   = resp.fet_temp;
			vesc_msg.motor_temp = resp.motor_temp;
			vesc_msg.vesc_id    = resp.vesc_id;


			mavlink_msg_lisum_vesc_status_4_encode_chan(dev_mav_sysid, dev_mav_compid,
														 MAVLINK_COMM_0, &tx_msg,
														 &vesc_msg);

			mav_send(&mav_gw_gnd_handle, &tx_msg);
		}

		break;

	}
		
	case messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_FIXED_PORT_ID_ :{

		mavlink_lisum_vesc_status_5_t vesc_msg;

		messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 resp;
		
		memset(&resp, 0, sizeof(resp));

		memset(&tx_msg, 0, sizeof(tx_msg));
		
		size_t sz = tr -> payload.size;
		int8_t rc = messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_deserialize_(
			&resp, (const uint8_t*)tr->payload.data, &sz);

		if (rc == 0) {

			vesc_msg.voltage    = resp.voltage;
			vesc_msg.tachometar = resp.tachometar;
			vesc_msg.vesc_id    = resp.vesc_id;

			mavlink_msg_lisum_vesc_status_5_encode_chan(dev_mav_sysid, dev_mav_compid,
														 MAVLINK_COMM_0, &tx_msg,
														 &vesc_msg);

			mav_send(&mav_gw_gnd_handle, &tx_msg);
		}

		break;

	}
		
	case messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_FIXED_PORT_ID_  :{

		mavlink_lisum_vesc_status_6_t vesc_msg;

		messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 resp;
		
		memset(&resp, 0, sizeof(resp));

		memset(&tx_msg, 0, sizeof(tx_msg));
		
		size_t sz = tr -> payload.size;
		int8_t rc = messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_deserialize_(
			&resp, (const uint8_t*)tr->payload.data, &sz);

		if (rc == 0) {

			vesc_msg.ppm     = resp.ppm;
			vesc_msg.adc1    = resp.adc1;
			vesc_msg.adc2    = resp.adc2;
			vesc_msg.adc3    = resp.adc3;
			vesc_msg.vesc_id = resp.vesc_id;


			mavlink_msg_lisum_vesc_status_6_encode_chan(dev_mav_sysid, dev_mav_compid,
														 MAVLINK_COMM_0, &tx_msg,
														 &vesc_msg);

			mav_send(&mav_gw_gnd_handle, &tx_msg);
		}

		break;

	}

	case messages_cyphal_uavcan_lisum_LisumVescPing_1_0_FIXED_PORT_ID_  :{

		mavlink_lisum_vesc_ping_t vesc_msg;
		messages_cyphal_uavcan_lisum_LisumVescPing_1_0 resp;
		
		memset(&resp, 0, sizeof(resp));
		memset(&tx_msg, 0, sizeof(tx_msg));
		
		size_t sz = tr -> payload.size;
		int8_t rc = messages_cyphal_uavcan_lisum_LisumVescPing_1_0_deserialize_(
			&resp, (const uint8_t*)tr->payload.data, &sz);

		if (rc == 0) {

			vesc_msg.vesc_id = resp.vesc_id;

			mavlink_msg_lisum_vesc_ping_encode_chan(dev_mav_sysid, dev_mav_compid,
														 MAVLINK_COMM_0, &tx_msg,
														 &vesc_msg);

			mav_send(&mav_gw_gnd_handle, &tx_msg);
		}
		break;
	}

	
	default:
		break;
	}

out:
	xSemaphoreGive(mutex_mav);
}

/* CAN interrupt callback */
void cyphal_cb(void* usr_arg, uint32_t int_status)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (int_status & CAN_HAL_INT_MASK_RI) {
		can_hal_msg_t rx;
		while (HAL_CAN_ReceiveMessage(CAN_HAL_INSTANCE_0, &rx, 0) == lStatus_Success) {
			if (xQueueSendFromISR(queue_cyphal_rx, &rx, &xHigherPriorityTaskWoken) != pdPASS) {
				lost_counter++;
			}
		}
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif /* MAVLINK_OR_CYPHAL */
