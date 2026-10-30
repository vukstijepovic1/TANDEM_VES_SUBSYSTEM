/**
 * @file    task_mav.c
 * @brief   Task MAV - handle MAVLink packets
 * @version 1.0.0
 * @date    09.04.2025
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

/* Peripherals */
#include "gpio.h"

/* Middleware */
#include "FreeRTOS.h"
#include "mav.h"
#include "queue.h"
#include "timers.h"

#include "udp_tl.h"

#include "uart_tl.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* MAVLink heartbeat period [ms] */
#define MAV_HB_PERIOD_MS 1000

/* MAVLink gateway ground track count */
#define MAV_GW_GND_TRACK_COUNT 7

/* MAVLink ground station helicopter application track count */
#define MAV_HELI_TRACK_COUNT 0

#define CYPHAL_SUBSCRIPTION_MESSAGES_COUNT_GW_SKY 8

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern SemaphoreHandle_t mutex_mav;
extern SemaphoreHandle_t tx_queue_mutex;
extern QueueSetHandle_t queueset_mav;
extern QueueHandle_t queue_mav_hb;

extern uart_tl_t mav_gw_gnd_uart_tl;
extern udp_tl_t mav_gw_gnd_udp_tl;

/* MAVLink Component Information Basic defines */
const char vendor_name[] = VENDOR_NAME;
const char model_name[] = MODEL_NAME;
const char software_version[] = SOFTWARE_VERSION;
const char hardware_version[] = HARDWARE_VERSION;
const char serial_number[] = SERIAL_NUMBER;

/* Extern temporary variables */
extern uint64_t boot_time_ms;

// MAVLink information
static const uint8_t mav_key[32] = "Kolega zatvorite vrata";

const uint8_t dev_mav_sysid = 0;
const uint8_t dev_mav_compid = MAV_COMP_ID_USER1;

static const uint8_t gw_gnd_mav_sysid = 1;
static const uint8_t gw_gnd_mav_compid = MAV_COMP_ID_AUTOPILOT1;

static const uint8_t heli_mav_sysid = 0;
static const uint8_t heli_mav_compid = MAV_COMP_ID_USER52;


static const mav_track_t mav_gw_gnd_tracks[MAV_GW_GND_TRACK_COUNT] = {
	{.msgid = MAVLINK_MSG_ID_HEARTBEAT, 
	 .sysid = gw_gnd_mav_sysid, 
	 .compid = gw_gnd_mav_compid},
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_SET_DUTY, 
	 .sysid = heli_mav_sysid, 
	 .compid = heli_mav_compid},
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_SET_CURRENT, 
	 .sysid = heli_mav_sysid, 
	 .compid = heli_mav_compid},
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_SET_CURRENT_BRAKE, 
	 .sysid = heli_mav_sysid, 
	 .compid = heli_mav_compid},
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_SET_RPM, 
	 .sysid = heli_mav_sysid, 
	 .compid = heli_mav_compid},
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_SET_POS, 
	 .sysid = heli_mav_sysid, 
	 .compid = heli_mav_compid},
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_PING, 
	 .sysid = heli_mav_sysid, 
	 .compid = heli_mav_compid}};

mav_t mav_gw_gnd_handle;

static mav_link_t mav_link_gw_gnd_udp __attribute__((section("AHBSRAM0")));


#if !MAVLINK_OR_CYPHAL
/* CANARD DATA STRUCTURES */
struct CanardInstance canard;
struct CanardTxQueue tx_queue;

/* Canard Transfer ID */
static CanardTransferID heartbeat_tid     = 0;  /* Unused for now*/
static CanardTransferID vesc_set_duty_tid = 0;
static CanardTransferID vesc_set_erpm_tid = 0;
static CanardTransferID vesc_set_curr_tid = 0;
static CanardTransferID vesc_set_curb_tid = 0;
static CanardTransferID vesc_set_pos_tid  = 0;
static CanardTransferID vesc_ping_tid     = 0;

static uint8_t lisum_vesc_set_duty_buf
    [messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_set_duty_sz = sizeof(lisum_vesc_set_duty_buf); 

static uint8_t lisum_vesc_set_erpm_buf
    [messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_set_erpm_sz = sizeof(lisum_vesc_set_erpm_buf); 

static uint8_t lisum_vesc_set_curr_buf
    [messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_set_curr_sz = sizeof(lisum_vesc_set_curr_buf); 

static uint8_t lisum_vesc_set_curb_buf
    [messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_set_curb_sz = sizeof(lisum_vesc_set_curb_buf); 

static uint8_t lisum_vesc_set_pos_buf
    [messages_cyphal_uavcan_lisum_LisumVescSetPos_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_set_pos_sz = sizeof(lisum_vesc_set_pos_buf); 

static uint8_t lisum_vesc_ping_buf
    [messages_cyphal_uavcan_lisum_LisumVescPing_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
static size_t lisum_vesc_ping_sz = sizeof(lisum_vesc_ping_buf); 

/* Canard Transfer ID */
static struct CanardRxSubscription sub_heartbeat;
static struct CanardRxSubscription sub_si_unit_vesc_1_0;
static struct CanardRxSubscription sub_si_unit_vesc_2_0;
static struct CanardRxSubscription sub_si_unit_vesc_3_0;
static struct CanardRxSubscription sub_si_unit_vesc_4_0;
static struct CanardRxSubscription sub_si_unit_vesc_5_0;
static struct CanardRxSubscription sub_si_unit_vesc_6_0;
static struct CanardRxSubscription sub_si_unit_ping_0;

cyphal_subscription_messages_t cyphal_subscriptions[CYPHAL_SUBSCRIPTION_MESSAGES_COUNT_GW_SKY] = {

	{.kind = CanardTransferKindMessage,
	 .port_id = messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size =
		 messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0_EXTENT_BYTES_,
	 .subscription = &sub_si_unit_vesc_1_0},
	
	{.kind = CanardTransferKindMessage,
	 .port_id = messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size =
		 messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0_EXTENT_BYTES_,
	 .subscription = &sub_si_unit_vesc_2_0},
	
	{.kind = CanardTransferKindMessage,
	 .port_id = messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size =
		 messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0_EXTENT_BYTES_,
	 .subscription = &sub_si_unit_vesc_3_0},
	
	{.kind = CanardTransferKindMessage,
	 .port_id = messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size =
		 messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0_EXTENT_BYTES_,
	 .subscription = &sub_si_unit_vesc_4_0},
	
	{.kind = CanardTransferKindMessage,
	 .port_id = messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size =
		 messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0_EXTENT_BYTES_,
	 .subscription = &sub_si_unit_vesc_5_0},

	{.kind = CanardTransferKindMessage,
	 .port_id = messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size =
		 messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0_EXTENT_BYTES_,
	 .subscription = &sub_si_unit_vesc_6_0},	 
	
	{.kind = CanardTransferKindMessage,
	 .port_id = messages_cyphal_uavcan_lisum_LisumVescPing_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size =
		 messages_cyphal_uavcan_lisum_LisumVescPing_1_0_EXTENT_BYTES_,
	 .subscription = &sub_si_unit_ping_0},	

	{.kind = CanardTransferKindMessage,
	 .port_id = uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_,
	 .serialization_buffer_size = uavcan_node_Heartbeat_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
	 .subscription = &sub_heartbeat},

};

messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0         set_duty_data;
messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0      set_curr_data;
messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0 set_curb_data;
messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0          set_erpm_data;
messages_cyphal_uavcan_lisum_LisumVescSetPos_1_0          set_pos_data;
messages_cyphal_uavcan_lisum_LisumVescPing_1_0            ping_data;

extern const uint8_t CYPHAL_NODES_ID_ARRAY[CYPHAL_NODES_IDX_MAX];
cyphal_nodes_ids_t cyphal_nodes_ids;

volatile int validate = 0;

#endif /* MAVLINK_OR_CYPHAL */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* MAVLink gateway ground receive callback */
static void mav_gw_gnd_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg);

/* heartbeat timer callback */
static void timer_mav_hb_cb(TimerHandle_t xTimer);

static void handle_cyphal_transfer(const struct CanardRxTransfer* tr);
/*******************************************************************************
 * Code
 ******************************************************************************/

void task_mav(void* arg)
{
	QueueSetMemberHandle_t queue_member = NULL;
	mavlink_message_t msg;
	TimerHandle_t timer_mav_hb;

	/*--------------------------------- MAV ----------------------------------*/
	/* initialize MAVLink handle */
	mav_init(&mav_gw_gnd_handle, dev_mav_sysid, dev_mav_compid);

	/* initialize MAVLink UDP links */
	mav_link_init(&mav_link_gw_gnd_udp, MAVLINK_COMM_0, mav_gw_gnd_recv_cb, NULL,
				  (tl_t*)&mav_gw_gnd_udp_tl);

	/* connect MAVLink links to MAVLink handles */
	mav_link(&mav_gw_gnd_handle, &mav_link_gw_gnd_udp);

	/* configure signing */
	//mav_sign(&mav_gw_gnd_handle, mav_key, NULL);

	/* track gateway ground messages */
	for (uint8_t i = 0; i < MAV_GW_GND_TRACK_COUNT; i++) {
		mav_track_t* track;

		track = (mav_track_t*)&mav_gw_gnd_tracks[i];
		mav_track(&mav_gw_gnd_handle, track->msgid, track->sysid, track->compid);
	}



	/* Initialize deterministic memory pools and Cyphal */
	cyphal_pool_init();

	canard = canardInit(cyphal_instance_memory);
	canard.node_id = CYPHAL_GW_SKY_ID; /* Local node-ID (adjust as needed) */

	/* TX queue for one CAN interface (classic CAN MTU=8). Keep capacity moderate to fit RAM. */
	tx_queue = canardTxInit(512, CANARD_MTU_CAN_CLASSIC, cyphal_txq_memory);

	/* ------------ Subscribe to all messages that we will track ----------- */

	for (uint8_t i = 0; i < CYPHAL_SUBSCRIPTION_MESSAGES_COUNT_GW_SKY; i++) {
		(void)canardRxSubscribe(
			&canard, cyphal_subscriptions[i].kind, cyphal_subscriptions[i].port_id,
			cyphal_subscriptions[i].serialization_buffer_size,
			CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC, cyphal_subscriptions[i].subscription);
	}

	/* Initialize Cyphal node ID whitelist */
	for (uint8_t i = 0; i < CYPHAL_NODES_IDX_MAX; i++) {
		cyphal_nodes_ids.uavcan_node_id[i] = CYPHAL_NODES_ID_ARRAY[i];
	}

	/* create MAVLink heartbeat timer */
	timer_mav_hb =
		xTimerCreate("tim_mav_hb", pdMS_TO_TICKS(MAV_HB_PERIOD_MS), pdTRUE, NULL, timer_mav_hb_cb);

	/* start heartbeat timer */
	xTimerStart(timer_mav_hb, 0);

	for (;;) {
		queue_member = xQueueSelectFromSet(queueset_mav, portMAX_DELAY);

		if (queue_member == queue_mav_hb) {

			mavlink_heartbeat_t data;

			xQueueReceive(queue_mav_hb, &data, portMAX_DELAY);

			xSemaphoreTake(mutex_mav, portMAX_DELAY);

			mavlink_msg_heartbeat_encode_chan(dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_0, &msg,
											  &data);

			mav_send(&mav_gw_gnd_handle, &msg);

			xSemaphoreGive(mutex_mav);
		}
	}
}

static void mav_gw_gnd_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg)
{

	xSemaphoreTake(mutex_mav, portMAX_DELAY);

	switch (msg->msgid) {
	case MAVLINK_MSG_ID_HEARTBEAT: {
		HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 25,
							 !HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 25));

		break;
	}
	
	case MAVLINK_MSG_ID_LISUM_VESC_SET_DUTY: {

		mavlink_lisum_vesc_set_duty_t data;

		mavlink_msg_lisum_vesc_set_duty_decode(msg, &data);

		set_duty_data.duty_cycle = data.duty_cycle;
		set_duty_data.vesc_id = data.vesc_id;

		if (messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0_serialize_(
			&set_duty_data, lisum_vesc_set_duty_buf, &lisum_vesc_set_duty_sz) >= 0){
			TX_QUEUE_MUTEX_TAKE{
				validate = cyphal_publish_lisum_lisum_vesc_set_duty_subject(
					&canard, &tx_queue, CanardPriorityNominal,
					messages_cyphal_uavcan_lisum_LisumVescSetDuty_1_0_FIXED_PORT_ID_,
					lisum_vesc_set_duty_buf, lisum_vesc_set_duty_sz,
					&vesc_set_duty_tid, CYPHAL_MEDIUM_TIMEOUT);
				TX_QUEUE_MUTEX_GIVE;    
			}
		}
		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_SET_CURRENT: {

		mavlink_lisum_vesc_set_current_t data;

		mavlink_msg_lisum_vesc_set_current_decode(msg, &data);

		set_curr_data.current = data.current;
		set_curr_data.vesc_id = data.vesc_id;

		if (messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0_serialize_(
			&set_curr_data, lisum_vesc_set_curr_buf, &lisum_vesc_set_curr_sz) >= 0) {
			TX_QUEUE_MUTEX_TAKE{
			(void)cyphal_publish_lisum_lisum_vesc_set_current_subject(
				&canard, &tx_queue, CanardPriorityNominal,
				messages_cyphal_uavcan_lisum_LisumVescSetCurrent_1_0_FIXED_PORT_ID_,
				lisum_vesc_set_curr_buf, lisum_vesc_set_curr_sz, 
				&vesc_set_curr_tid, CYPHAL_MEDIUM_TIMEOUT);
				TX_QUEUE_MUTEX_GIVE;    
			}
		}
		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_SET_CURRENT_BRAKE: {

		mavlink_lisum_vesc_set_current_brake_t data;

		mavlink_msg_lisum_vesc_set_current_brake_decode(msg, &data);

		set_curb_data.current = data.current;
		set_curb_data.vesc_id = data.vesc_id;

		if (messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0_serialize_(
			&set_curb_data, lisum_vesc_set_curb_buf, &lisum_vesc_set_curb_sz) >= 0) {
			TX_QUEUE_MUTEX_TAKE{
			(void)cyphal_publish_lisum_lisum_vesc_set_current_brake_subject(
				&canard, &tx_queue, CanardPriorityNominal,
				messages_cyphal_uavcan_lisum_LisumVescSetCurrentBrake_1_0_FIXED_PORT_ID_, 
				lisum_vesc_set_curb_buf, lisum_vesc_set_curb_sz, 
				&vesc_set_curb_tid, CYPHAL_MEDIUM_TIMEOUT);
				TX_QUEUE_MUTEX_GIVE;    
			}
		}
		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_SET_RPM: {

		mavlink_lisum_vesc_set_rpm_t data;

		mavlink_msg_lisum_vesc_set_rpm_decode(msg, &data);

		set_erpm_data.rpm = data.rpm;
		set_erpm_data.vesc_id = data.vesc_id;

		if (messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0_serialize_(
			&set_erpm_data, lisum_vesc_set_erpm_buf, &lisum_vesc_set_erpm_sz) >= 0) {
			TX_QUEUE_MUTEX_TAKE{
			(void)cyphal_publish_lisum_lisum_vesc_set_rpm_subject(
				&canard, &tx_queue, CanardPriorityNominal,
				messages_cyphal_uavcan_lisum_LisumVescSetRpm_1_0_FIXED_PORT_ID_, 
				lisum_vesc_set_erpm_buf, lisum_vesc_set_erpm_sz, 
				&vesc_set_erpm_tid, CYPHAL_MEDIUM_TIMEOUT);
				TX_QUEUE_MUTEX_GIVE;    
			}
		}
		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_PING: {

		mavlink_lisum_vesc_ping_t data;

		mavlink_msg_lisum_vesc_ping_decode(msg, &data);

		ping_data.vesc_id= data.vesc_id;

		if (messages_cyphal_uavcan_lisum_LisumVescPing_1_0_serialize_(
			&ping_data, lisum_vesc_ping_buf, &lisum_vesc_ping_sz) >= 0) {
			TX_QUEUE_MUTEX_TAKE{
			(void)cyphal_publish_lisum_lisum_vesc_ping_subject(
				&canard, &tx_queue, CanardPriorityNominal,
				messages_cyphal_uavcan_lisum_LisumVescPing_1_0_FIXED_PORT_ID_,
				lisum_vesc_ping_buf, lisum_vesc_ping_sz, 
				&vesc_ping_tid, CYPHAL_MEDIUM_TIMEOUT);
				TX_QUEUE_MUTEX_GIVE;    
			}
		}
		break;
	}

	default:break;
	}
	xSemaphoreGive(mutex_mav);
}

static void timer_mav_hb_cb(TimerHandle_t xTimer)
{
	mavlink_heartbeat_t mav_hb = {.type = MAV_TYPE_ONBOARD_CONTROLLER,
								  .autopilot = MAV_AUTOPILOT_GENERIC,
								  .base_mode = 0,
								  .custom_mode = 0,
								  .system_status = MAV_STATE_ACTIVE};

	xQueueSendToBack(queue_mav_hb, &mav_hb, 0);
}