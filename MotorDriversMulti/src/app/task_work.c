/**
 * @file    task_work.c
 * @brief   Library used as template.
 * @version 0.0.0
 * @date    12.12.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "task_work.h"
#include "task_mav.h"
#include "types.h"
#include <stdint.h>
#include <string.h>

/* Peripherals */

/* Middleware */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "types.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern SemaphoreHandle_t mutex_mav;

extern QueueHandle_t queue_command;

extern struct CanardInstance canard;

extern cyphal_nodes_ids_t cyphal_nodes_ids;

extern QueueHandle_t queue_cyphal_rx;
static uint32_t lost_counter = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void handle_cyphal_transfer(const struct CanardRxTransfer* tr);

/*******************************************************************************
 * Code
 ******************************************************************************/

void task_work(void *arg)
{
    struct CanardRxSubscription* sub = NULL;
    struct CanardRxTransfer      tr  = {0};


    can_hal_msg_t rx;

    for (;;){
        while (xQueueReceive(queue_cyphal_rx, &rx, pdMS_TO_TICKS(5)) == pdPASS){

            if (rx.idFormat != CAN_HAL_ID_FORMAT_EXT) continue;

            struct CanardFrame fr = {.extended_can_id = rx.messageId,
                                     .payload =
                                       {.size = rx.length, .data = rx.data}};

            const  CanardMicrosecond ts = (CanardMicrosecond)HAL_GetTimeUS();

            int8_t acc = 0;
            TX_QUEUE_MUTEX_TAKE {
                acc = canardRxAccept(&canard, ts, &fr, 0, &tr, &sub);
                TX_QUEUE_MUTEX_GIVE;
            }
            if (acc > 0){
                handle_cyphal_transfer(&tr);
                TX_QUEUE_MUTEX_TAKE {
                    if(tr.payload.allocated_size > 0) canard.memory.deallocate(
                        canard.memory.user_reference,
                        tr.payload.allocated_size,
                        tr.payload.data);
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }
    }
}

/****************************** static functions ******************************/


static void handle_cyphal_transfer(const struct CanardRxTransfer* tr){

    xSemaphoreTake(mutex_mav, portMAX_DELAY);

    switch(tr->metadata.port_id) {

    case messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0_FIXED_PORT_ID_: {
        messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0 resp;
        memset(&resp, 0, sizeof(resp));

        size_t sz = tr->payload.size;

        int8_t rc = messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0_deserialize_(
            &resp, (const uint8_t*)tr->payload.data, &sz
        );

        if (rc == 0 && queue_command != NULL) {

            vesc_command_message_t cmd;

            cmd.commmand_ID = CAN_PACKET_SET_DUTY;
            cmd.vesc_id = resp.vesc_id;
            cmd.command_pharam = resp.duty_cycle;

            xQueueSend(queue_command, &cmd, 0);
        }
        break;
    }

    case messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0_FIXED_PORT_ID_: {
        messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0 resp;
        memset(&resp, 0, sizeof(resp));

        size_t sz = tr->payload.size;

        int8_t rc = messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0_deserialize_(
            &resp, (const uint8_t*)tr->payload.data, &sz
        );

        if (rc == 0 && queue_command != NULL) {

            vesc_command_message_t cmd;

            cmd.commmand_ID = CAN_PACKET_SET_CURRENT;
            cmd.vesc_id = resp.vesc_id;
            cmd.command_pharam = resp.current;

            xQueueSend(queue_command, &cmd, 0);
        }
        break;
    }

    case messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0_FIXED_PORT_ID_: {
        messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0 resp;
        memset(&resp, 0, sizeof(resp));

        size_t sz = tr->payload.size;

        int8_t rc = messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0_deserialize_(
            &resp, (const uint8_t*)tr->payload.data, &sz
        );

        if (rc == 0 && queue_command != NULL) {

            vesc_command_message_t cmd;

            cmd.commmand_ID = CAN_PACKET_SET_CURRENT_BRAKE;
            cmd.vesc_id = resp.vesc_id;
            cmd.command_pharam = resp.current;

            xQueueSend(queue_command, &cmd, 0);
        }
        break;
    }

    case messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0_FIXED_PORT_ID_: {
        messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0 resp;
        memset(&resp, 0, sizeof(resp));

        size_t sz = tr->payload.size;

        int8_t rc = messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0_deserialize_(
            &resp, (const uint8_t*)tr->payload.data, &sz
        );

        if (rc == 0 && queue_command != NULL) {

            vesc_command_message_t cmd;

            cmd.commmand_ID = CAN_PACKET_SET_RPM;
            cmd.vesc_id = resp.vesc_id;
            cmd.command_pharam = resp.rpm;

            xQueueSend(queue_command, &cmd, 0);
        }
        break;
    }

    case messages_cyphal_uavcan_lisum_LisumVescSetPos_1_0_FIXED_PORT_ID_: {
        messages_cyphal_uavcan_lisum_LisumVescSetPos_1_0 resp;
        memset(&resp, 0, sizeof(resp));

        size_t sz = tr->payload.size;

        int8_t rc = messages_cyphal_uavcan_lisum_LisumVescSetPos_1_0_deserialize_(
            &resp, (const uint8_t*)tr->payload.data, &sz
        );

        if (rc == 0 && queue_command != NULL) {

            vesc_command_message_t cmd;

            cmd.commmand_ID = CAN_PACKET_SET_POS;
            cmd.vesc_id = resp.vesc_id;
            cmd.command_pharam = resp.pos;

            xQueueSend(queue_command, &cmd, 0);
        }
        break;
    }
        
    case messages_cyphal_uavcan_lisum_LisumVescPing_1_0_FIXED_PORT_ID_: {
        messages_cyphal_uavcan_lisum_LisumVescPing_1_0 resp;
        memset(&resp, 0, sizeof(resp));

        size_t sz = tr->payload.size;

        int8_t rc = messages_cyphal_uavcan_lisum_LisumVescPing_1_0_deserialize_(
            &resp, (const uint8_t*)tr->payload.data, &sz
        );

        if (rc == 0 && queue_command != NULL) {

            vesc_command_message_t cmd;

            cmd.commmand_ID = CAN_PACKET_PING;
            cmd.vesc_id        = resp.vesc_id;
            cmd.command_pharam = resp.vesc_id;

            xQueueSend(queue_command, &cmd, 0);
        }
        break;
    }




    default: break;// Unknown port ID
    }
    
    xSemaphoreGive(mutex_mav);


}


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

/********************************* End Of File ********************************/