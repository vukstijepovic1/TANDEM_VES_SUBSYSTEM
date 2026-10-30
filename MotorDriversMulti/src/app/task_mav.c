/**
 * @file    task_mav.c
 * @brief   Library used as template.
 * @version 0.0.0
 * @date    12.12.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "main.h"
#include "task_mav.h"
#include "task_bridge.h"
#include "task_work.h"
#include "types.h"

/* External hardware drivers */
#include "gpio.h"
#include "can.h"
#include "canard.h"
#include "cyphal_utility.h"

/* Middleware */
#include "FreeRTOS.h"
#include "mav.h"
#include "queue.h"
#include "timers.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define MAXIMUM_FRAMES_IN_QUEUE 400

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*FreeRTOS object*/ 

/* Mutex */
extern SemaphoreHandle_t tx_queue_mutex;

/* Queues */ 
extern QueueSetHandle_t queueset_mav;

/* Queueset */
extern QueueHandle_t queue_response;


/*  Component Information Basic defines */
const char vendor_name[] = VENDOR_NAME;
const char model_name[] = MODEL_NAME;
const char software_version[] = SOFTWARE_VERSION;
const char hardware_version[] = HARDWARE_VERSION;
const char serial_number[] = SERIAL_NUMBER;

/* CANARD DATA STRUCTURES */
struct CanardInstance canard;
struct CanardTxQueue tx_queue;

/* Cyphal node whitelist */
extern const uint8_t CYPHAL_NODES_ID_ARRAY[CYPHAL_NODES_IDX_MAX];
cyphal_nodes_ids_t cyphal_nodes_ids;

/* Canard Transfer ID */
static CanardTransferID tid_lisum_vesc_ping_data = 0;

typedef struct {
    CanardTransferID status_1;
    CanardTransferID status_2;
    CanardTransferID status_3;
    CanardTransferID status_4;
    CanardTransferID status_5;
    CanardTransferID status_6;
} vesc_transfer_ids_t;

static vesc_transfer_ids_t transfer_ids;

static uint8_t lisum_vesc_status_1_buf
    [messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_status_1_sz = sizeof(lisum_vesc_status_1_buf); 

static uint8_t lisum_vesc_status_2_buf
    [messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_status_2_sz = sizeof(lisum_vesc_status_2_buf); 

static uint8_t lisum_vesc_status_3_buf
    [messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_status_3_sz = sizeof(lisum_vesc_status_3_buf); 

static uint8_t lisum_vesc_status_4_buf
    [messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_status_4_sz = sizeof(lisum_vesc_status_4_buf); 

static uint8_t lisum_vesc_status_5_buf
    [messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_status_5_sz = sizeof(lisum_vesc_status_5_buf); 

static uint8_t lisum_vesc_status_6_buf
    [messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_status_6_sz = sizeof(lisum_vesc_status_6_buf); 

static uint8_t lisum_vesc_ping_buf
    [messages_cyphal_uavcan_lisum_LisumVescPing_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_ping_sz = sizeof(lisum_vesc_ping_buf); 

/* Canard Transfer ID Subscriptions */
static struct CanardRxSubscription sub_lisum_vesc_set_duty_1_0;
static struct CanardRxSubscription sub_lisum_vesc_set_current_1_0;
static struct CanardRxSubscription sub_lisum_vesc_set_current_brake_1_0;
static struct CanardRxSubscription sub_lisum_vesc_set_rpm_1_0;
static struct CanardRxSubscription sub_lisum_vesc_set_pos_1_0;
static struct CanardRxSubscription sub_lisum_vesc_ping_1_0;

volatile int validate = 0;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void handle_vesc_queueset(vesc_instance_t *v, int fleet_idx);

/*******************************************************************************
 * Code
 ******************************************************************************/

void task_mav(void *arg)
{
    //QueueSetMemberHandle_t queue_member = NULL;

    vesc_command_message_t response_data;

    /* Init transfer ID arrays to zero */
    memset(&transfer_ids, 0, sizeof(transfer_ids));

    /* Initialize deterministic memory pools and Cyphal*/
    cyphal_pool_init();

    canard = canardInit(cyphal_instance_memory);
    canard.node_id = CYPHAL_MOTOR_DRIVERS_ID; /* Local node-ID *(adjust as nead) */

	/* TX queue for one CAN interface (classic CAN MTU=8). Increase capacity to
	 * reduce OOM on multi-frame. */
	tx_queue = canardTxInit(1024, CANARD_MTU_CAN_CLASSIC, cyphal_txq_memory);

    /* Subscribe to all messages that we will track */
    (void)canardRxSubscribe(
        &canard, CanardTransferKindMessage,
        messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0_FIXED_PORT_ID_,
        messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
        CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC, &sub_lisum_vesc_set_duty_1_0);

    (void)canardRxSubscribe(
        &canard, CanardTransferKindMessage,
        messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0_FIXED_PORT_ID_,
        messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
        CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC, &sub_lisum_vesc_set_current_1_0);

    (void)canardRxSubscribe(
        &canard, CanardTransferKindMessage,
        messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0_FIXED_PORT_ID_,
        messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
        CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC, &sub_lisum_vesc_set_current_brake_1_0);

    (void)canardRxSubscribe(
        &canard, CanardTransferKindMessage,
        messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0_FIXED_PORT_ID_,
        messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
        CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC, &sub_lisum_vesc_set_rpm_1_0);

    (void)canardRxSubscribe(
        &canard, CanardTransferKindMessage,
        messages_cyphal_uavcan_lisum_LisumVescSetPos_1_0_FIXED_PORT_ID_,
        messages_cyphal_uavcan_lisum_LisumVescSetPos_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
        CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC, &sub_lisum_vesc_set_pos_1_0);

    (void)canardRxSubscribe(
        &canard, CanardTransferKindMessage,
        messages_cyphal_uavcan_lisum_LisumVescPing_1_0_FIXED_PORT_ID_,
        messages_cyphal_uavcan_lisum_LisumVescPing_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
        CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC, &sub_lisum_vesc_ping_1_0);



	/* Initialize Cyphal node ID whitelist */
    for (uint8_t i = 0; i < CYPHAL_NODES_IDX_MAX; i++)
	{
		cyphal_nodes_ids.uavcan_node_id[i] = CYPHAL_NODES_ID_ARRAY[i];
	}

    for(;;){
        /* --- Check each active VESC's queueset --- */
        for (int i = 0; i < MAX_VESC_COUNT; i++) {
            if (!vesc_fleet[i].active) continue;
            handle_vesc_queueset(&vesc_fleet[i], i);
        }

        /* --- Check shared response queue (ping/pong) --- */
        if (xQueueReceive(queue_response, &response_data, 0) == pdPASS) {
            if (response_data.commmand_ID == CAN_PACKET_PONG) {
                lisum_vesc_ping_sz = sizeof(lisum_vesc_ping_buf);
                TX_QUEUE_MUTEX_TAKE {
                    validate = cyphal_publish_lisum_lisum_vesc_ping_subject(
                        &canard, &tx_queue, CanardPriorityExceptional,
                        messages_cyphal_uavcan_lisum_LisumVescPing_1_0_FIXED_PORT_ID_,
                        lisum_vesc_ping_buf, lisum_vesc_ping_sz,
                        &tid_lisum_vesc_ping_data, CYPHAL_MEDIUM_TIMEOUT);
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1));

    }
}

/**
 * @brief Check one VESC instance's queueset (non-blocking) and publish
 *        any status messages that are ready.
 *
 * @param v         Pointer to the VESC instance.
 * @param fleet_idx Index into vesc_fleet[], used to select transfer IDs.
 */
static void handle_vesc_queueset(vesc_instance_t *v, int fleet_idx)
{
    QueueSetMemberHandle_t active_queue;
    vesc_transfer_ids_t   *tids = &transfer_ids;

    while ((active_queue = xQueueSelectFromSet(v->queueset, 0)) != NULL)
    {

        /* --- Status 1 --- */
        if (active_queue == v->queue_status_1) {
            messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 msg;
            if (xQueueReceive(v->queue_status_1, &msg, 0) == pdPASS) {
                lisum_vesc_status_1_sz = sizeof(lisum_vesc_status_1_buf);
                if (messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_serialize_(
                        &msg, lisum_vesc_status_1_buf, &lisum_vesc_status_1_sz) >= 0) {
                    TX_QUEUE_MUTEX_TAKE {
                        validate = cyphal_publish_lisum_lisum_vesc_status1_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_1_buf, lisum_vesc_status_1_sz,
                            &tids->status_1, CYPHAL_MEDIUM_TIMEOUT);
                        TX_QUEUE_MUTEX_GIVE;
                    }
                }
            }

        /* --- Status 2 --- */
        } else if (active_queue == v->queue_status_2) {
            messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 msg;
            if (xQueueReceive(v->queue_status_2, &msg, 0) == pdPASS) {
                lisum_vesc_status_2_sz = sizeof(lisum_vesc_status_2_buf);
                if (messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_serialize_(
                        &msg, lisum_vesc_status_2_buf, &lisum_vesc_status_2_sz) >= 0) {
                    TX_QUEUE_MUTEX_TAKE {
                        validate = cyphal_publish_lisum_lisum_vesc_status2_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_2_buf, lisum_vesc_status_2_sz,
                            &tids->status_2, CYPHAL_MEDIUM_TIMEOUT);
                        TX_QUEUE_MUTEX_GIVE;
                    }
                }
            }

        /* --- Status 3 --- */
        } else if (active_queue == v->queue_status_3) {
            messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 msg;
            if (xQueueReceive(v->queue_status_3, &msg, 0) == pdPASS) {
                lisum_vesc_status_3_sz = sizeof(lisum_vesc_status_3_buf);
                if (messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_serialize_(
                        &msg, lisum_vesc_status_3_buf, &lisum_vesc_status_3_sz) >= 0) {
                    TX_QUEUE_MUTEX_TAKE {
                        validate = cyphal_publish_lisum_lisum_vesc_status3_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_3_buf, lisum_vesc_status_3_sz,
                            &tids->status_3, CYPHAL_MEDIUM_TIMEOUT);
                        TX_QUEUE_MUTEX_GIVE;
                    }
                }
            }

        /* --- Status 4 --- */
        } else if (active_queue == v->queue_status_4) {
            messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 msg;
            if (xQueueReceive(v->queue_status_4, &msg, 0) == pdPASS) {
                lisum_vesc_status_4_sz = sizeof(lisum_vesc_status_4_buf);
                if (messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_serialize_(
                        &msg, lisum_vesc_status_4_buf, &lisum_vesc_status_4_sz) >= 0) {
                    TX_QUEUE_MUTEX_TAKE {
                        validate = cyphal_publish_lisum_lisum_vesc_status4_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_4_buf, lisum_vesc_status_4_sz,
                            &tids->status_4, CYPHAL_MEDIUM_TIMEOUT);
                        TX_QUEUE_MUTEX_GIVE;
                    }
                }
            }

        /* --- Status 5 --- */
        } else if (active_queue == v->queue_status_5) {
            messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 msg;
            if (xQueueReceive(v->queue_status_5, &msg, 0) == pdPASS) {
                lisum_vesc_status_5_sz = sizeof(lisum_vesc_status_5_buf);
                if (messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_serialize_(
                        &msg, lisum_vesc_status_5_buf, &lisum_vesc_status_5_sz) >= 0) {
                    TX_QUEUE_MUTEX_TAKE {
                        validate = cyphal_publish_lisum_lisum_vesc_status5_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_5_buf, lisum_vesc_status_5_sz,
                            &tids->status_5, CYPHAL_MEDIUM_TIMEOUT);
                        TX_QUEUE_MUTEX_GIVE;
                    }
                }
            }

        /* --- Status 6 --- */
        } else if (active_queue == v->queue_status_6) {
            messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 msg;
            if (xQueueReceive(v->queue_status_6, &msg, 0) == pdPASS) {
                lisum_vesc_status_6_sz = sizeof(lisum_vesc_status_6_buf);
                if (messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_serialize_(
                        &msg, lisum_vesc_status_6_buf, &lisum_vesc_status_6_sz) >= 0) {
                    TX_QUEUE_MUTEX_TAKE {
                        validate = cyphal_publish_lisum_lisum_vesc_status6_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_6_buf, lisum_vesc_status_6_sz,
                            &tids->status_6, CYPHAL_MEDIUM_TIMEOUT);
                        TX_QUEUE_MUTEX_GIVE;
                    }
                }
            }
        }
    }
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/