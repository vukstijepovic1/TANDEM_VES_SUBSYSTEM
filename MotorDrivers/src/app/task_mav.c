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
extern QueueHandle_t queue_status_1;
extern QueueHandle_t queue_status_2;
extern QueueHandle_t queue_status_3;
extern QueueHandle_t queue_status_4;
extern QueueHandle_t queue_status_5;
extern QueueHandle_t queue_status_6;

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
static CanardTransferID tid_lisum_vesc_status_1_data = 0;
static CanardTransferID tid_lisum_vesc_status_2_data = 0;
static CanardTransferID tid_lisum_vesc_status_3_data = 0;
static CanardTransferID tid_lisum_vesc_status_4_data = 0;
static CanardTransferID tid_lisum_vesc_status_5_data = 0;
static CanardTransferID tid_lisum_vesc_status_6_data = 0;
static CanardTransferID tid_lisum_vesc_ping_data = 0;

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
static void handle_cyphal_transfer(const struct CanardRxTransfer *tr);

/*******************************************************************************
 * Code
 ******************************************************************************/

void task_mav(void *arg)
{
    QueueSetMemberHandle_t queue_member = NULL;

    messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 status_1_data;
    messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 status_2_data;
    messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 status_3_data;
    messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 status_4_data;
    messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 status_5_data;
    messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 status_6_data;
    vesc_command_message_t response_data;

    /* Initialize deterministic memory pools and Cyphal*/
    cyphal_pool_init();

    canard = canardInit(cyphal_instance_memory);
    canard.node_id = CYPHAL_MOTOR_DRIVERS_ID; /* Local node-ID *(adjust as nead) */

	/* TX queue for one CAN interface (classic CAN MTU=8). Increase capacity to
	 * reduce OOM on multi-frame. */
	tx_queue = canardTxInit(512, CANARD_MTU_CAN_CLASSIC, cyphal_txq_memory);

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
        queue_member = xQueueSelectFromSet(queueset_mav, portMAX_DELAY);

        if (queue_member == queue_status_1)
        {
            
            if(xQueueReceive(queue_status_1, &status_1_data, 0) == pdPASS) {

                if (messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_serialize_(
                    &status_1_data, lisum_vesc_status_1_buf, &lisum_vesc_status_1_sz) >= 0){
                    TX_QUEUE_MUTEX_TAKE{
                        validate = cyphal_publish_lisum_lisum_vesc_status1_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_1_buf, lisum_vesc_status_1_sz,
                            &tid_lisum_vesc_status_1_data, CYPHAL_MEDIUM_TIMEOUT);

                        TX_QUEUE_MUTEX_GIVE;    
                    }
                }
            }
        }

        else if (queue_member == queue_status_2)
        {
            if (xQueueReceive(queue_status_2, &status_2_data, 0) == pdPASS)
            {
                if (messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_serialize_(
                        &status_2_data, lisum_vesc_status_2_buf, &lisum_vesc_status_2_sz) >= 0)
                {
                    TX_QUEUE_MUTEX_TAKE
                    {
                        validate = cyphal_publish_lisum_lisum_vesc_status2_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_2_buf, lisum_vesc_status_2_sz,
                            &tid_lisum_vesc_status_2_data, CYPHAL_MEDIUM_TIMEOUT);
                    }
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }

        else if (queue_member == queue_status_3)
        {
            if (xQueueReceive(queue_status_3, &status_3_data, 0) == pdPASS)
            {
                if (messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_serialize_(
                        &status_3_data, lisum_vesc_status_3_buf, &lisum_vesc_status_3_sz) >= 0)
                {
                    TX_QUEUE_MUTEX_TAKE
                    {
                        validate = cyphal_publish_lisum_lisum_vesc_status3_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_3_buf, lisum_vesc_status_3_sz,
                            &tid_lisum_vesc_status_3_data, CYPHAL_MEDIUM_TIMEOUT);
                    }
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }

        else if (queue_member == queue_status_4)
        {
            if (xQueueReceive(queue_status_4, &status_4_data, 0) == pdPASS)
            {
                if (messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_serialize_(
                        &status_4_data, lisum_vesc_status_4_buf, &lisum_vesc_status_4_sz) >= 0)
                {
                    TX_QUEUE_MUTEX_TAKE
                    {
                        validate = cyphal_publish_lisum_lisum_vesc_status4_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_4_buf, lisum_vesc_status_4_sz,
                            &tid_lisum_vesc_status_4_data, CYPHAL_MEDIUM_TIMEOUT);
                    }
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }

        else if (queue_member == queue_status_5)
        {
            if (xQueueReceive(queue_status_5, &status_5_data, 0) == pdPASS)
            {
                if (messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_serialize_(
                        &status_5_data, lisum_vesc_status_5_buf, &lisum_vesc_status_5_sz) >= 0)
                {
                    TX_QUEUE_MUTEX_TAKE
                    {
                        validate = cyphal_publish_lisum_lisum_vesc_status5_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_5_buf, lisum_vesc_status_5_sz,
                            &tid_lisum_vesc_status_5_data, CYPHAL_MEDIUM_TIMEOUT);
                    }
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }

        else if (queue_member == queue_status_6)
        {
            if (xQueueReceive(queue_status_6, &status_6_data, 0) == pdPASS)
            {
                if (messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_serialize_(
                        &status_6_data, lisum_vesc_status_6_buf, &lisum_vesc_status_6_sz) >= 0)
                {
                    TX_QUEUE_MUTEX_TAKE
                    {
                        validate = cyphal_publish_lisum_lisum_vesc_status6_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_FIXED_PORT_ID_,
                            lisum_vesc_status_6_buf, lisum_vesc_status_6_sz,
                            &tid_lisum_vesc_status_6_data, CYPHAL_MEDIUM_TIMEOUT);
                    }
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }

        else if (queue_member == queue_response)
        {
            if (xQueueReceive(queue_response, &response_data, 0) == pdPASS)
            {   
                if(response_data.commmand_ID == CAN_PACKET_PONG){
                    TX_QUEUE_MUTEX_TAKE
                    {
                        validate = cyphal_publish_lisum_lisum_vesc_ping_subject(
                            &canard, &tx_queue, CanardPriorityExceptional,
                            messages_cyphal_uavcan_lisum_LisumVescPing_1_0_FIXED_PORT_ID_,
                            lisum_vesc_ping_buf, lisum_vesc_ping_sz,
                            &tid_lisum_vesc_ping_data, CYPHAL_MEDIUM_TIMEOUT);
                    }
                    TX_QUEUE_MUTEX_GIVE;
                }
            }
        }
        
    }
}


/****************************** static functions ******************************/

/********************************* End Of File ********************************/