/**
 * @file    task_mav.c
 * @brief   Task MAV - handle MAVLink packets
 * @version 1.0.0
 * @date    07.05.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "dev_config.h"
#include "main.h"
#include "task_mav.h"
#include "task_work.h"

/* Peripherals */
#include "gpio.h"

#include "main.h"

/* External hardware drivers */

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "mav.h"
#include "queue.h"
#include "timers.h"
#include "uart_tl.h"
#include "udp_tl.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* MAVLink heartbeat period [ms] */
#define MAV_HB_PERIOD_MS 1000

/* MAVLink gateway sky track count */
#define MAV_GW_SKY_TRACK_COUNT 33

/* MAVLink ground station helicopter application track count */
#define MAV_HELI_TRACK_COUNT 15

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

#define MAV_MISSION_PLANNER_TRACK_COUNT 7

#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern SemaphoreHandle_t mutex_mav;
extern QueueSetHandle_t queueset_mav;
extern QueueHandle_t queue_mav_hb;

/* -------------------------- Transport layers ------------------------------ */
#if MISSION_PLANNER_UART_TRANSPORT_LAYER

extern uart_tl_t mav_gs_mp_uart_tl;

#endif

extern uart_tl_t mav_gw_sky_uart_tl;
extern udp_tl_t mav_gw_sky_udp_tl;
extern udp_tl_t mav_heli_tl;
extern udp_tl_t mav_motor_tl;
extern udp_tl_t mav_fg_tl;
extern udp_tl_t mav_maps_tl;
extern udp_tl_t mav_bb_tl;

#if MISSION_PLANNER_UDP_TRANSPORT_LAYER

extern udp_tl_t mav_gs_mp_tl;

#endif

/* ------------------------ MAVLink information ---------------------------- */
static const uint8_t mav_key[32] = "Kolega zatvorite vrata";

static const uint8_t dev_mav_sysid = 1;
static const uint8_t dev_mav_compid = MAV_COMP_ID_AUTOPILOT1;

static const uint8_t gw_sky_mav_sysid = 0;
static const uint8_t gw_sky_mav_compid = MAV_COMP_ID_USER1;

static const uint8_t pwr_man_mav_sysid = 0;
static const uint8_t pwr_man_mav_compid = MAV_COMP_ID_USER2;

static const uint8_t ins_mav_sysid = 0;
static const uint8_t ins_mav_compid = MAV_COMP_ID_USER4;

static const uint8_t act_master_mav_sysid = 0;
static const uint8_t act_master_mav_compid = MAV_COMP_ID_USER5;

static const uint8_t ins_cots_mav_sysid = 0;
static const uint8_t ins_cots_mav_compid = MAV_COMP_ID_USER14;

static const uint8_t heli_mav_sysid = 0;
static const uint8_t heli_mav_compid = MAV_COMP_ID_USER52;

static const uint8_t batt1_mav_sysid = 0;
static const uint8_t batt1_mav_compid = MAV_COMP_ID_BATTERY;

static const uint8_t batt2_mav_sysid = 0;
static const uint8_t batt2_mav_compid = MAV_COMP_ID_BATTERY2;

static const uint8_t bb_mav_sysid = 0;
static const uint8_t bb_mav_compid = MAV_COMP_ID_USER3;

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

static const uint8_t mp_mav_sysid = 255;
static const uint8_t mp_mav_compid = MAV_COMP_ID_MISSIONPLANNER;

#endif

/* --------------------------- MAVLink tracks ------------------------------ */

static const mav_track_t mav_gw_sky_tracks[MAV_GW_SKY_TRACK_COUNT] = {

	{.msgid = MAVLINK_MSG_ID_HEARTBEAT,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},

	{.msgid = MAVLINK_MSG_ID_HEARTBEAT,
	 .sysid = bb_mav_sysid,
	 .compid = bb_mav_compid},

	{.msgid = MAVLINK_MSG_ID_COMMAND_ACK,
	 .sysid = act_master_mav_sysid,
	 .compid = act_master_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_POWER_HORNET_ACT_DATA,
	 .sysid = act_master_mav_sysid,
	 .compid = act_master_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_MANUAL_CTRL_HORNET,
	 .sysid = act_master_mav_sysid,
	 .compid = act_master_mav_compid},

	{.msgid = MAVLINK_MSG_ID_HEARTBEAT,
	 .sysid = act_master_mav_sysid,
	 .compid = act_master_mav_compid},

	{.msgid = MAVLINK_MSG_ID_SCALED_IMU,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_SCALED_IMU2,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_SCALED_PRESSURE,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_ALTITUDE,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_ATTITUDE,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_GNSS_RECV_DATA,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_SENSOR_AIRSPEED_DATA,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_BATTERY_STATUS,
	 .sysid = batt1_mav_sysid,
	 .compid = batt1_mav_compid},

	{.msgid = MAVLINK_MSG_ID_BATTERY_STATUS,
	 .sysid = batt2_mav_sysid,
	 .compid = batt2_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_POWER_MOTOR_SCALED_DATA,
	 .sysid = pwr_man_mav_sysid,
	 .compid = pwr_man_mav_compid},

	{.msgid = MAVLINK_MSG_ID_ATTITUDE,
	 .sysid = ins_cots_mav_sysid,
	 .compid = ins_cots_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_GNSS_RECV_DATA,
	 .sysid = ins_cots_mav_sysid,
	 .compid = ins_cots_mav_compid},

	{.msgid = MAVLINK_MSG_ID_COMPONENT_INFORMATION_BASIC,
	 .sysid = bb_mav_sysid,
	 .compid = bb_mav_compid},

	{.msgid = MAVLINK_MSG_ID_COMPONENT_INFORMATION_BASIC,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},

	{.msgid = MAVLINK_MSG_ID_COMPONENT_INFORMATION_BASIC,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL,
	 .sysid = bb_mav_sysid,
	 .compid = bb_mav_compid},

	{.msgid = MAVLINK_MSG_ID_AUTOPILOT_VERSION,
	 .sysid = bb_mav_sysid,
	 .compid = bb_mav_compid},

	{.msgid = MAVLINK_MSG_ID_AUTOPILOT_VERSION,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},

	{.msgid = MAVLINK_MSG_ID_AUTOPILOT_VERSION,
	 .sysid = ins_mav_sysid,
	 .compid = ins_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_STATUS_1,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_STATUS_2,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},
	
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_STATUS_3,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},
	
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_STATUS_4,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},
	
	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_STATUS_5,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_STATUS_6,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_VESC_PING,
	 .sysid = gw_sky_mav_sysid,
	 .compid = gw_sky_mav_compid}};


static const mav_track_t mav_heli_tracks[MAV_HELI_TRACK_COUNT] = {

	{.msgid = MAVLINK_MSG_ID_HEARTBEAT,
	 .sysid = heli_mav_sysid,
	 .compid = heli_mav_compid},

	{.msgid = MAVLINK_MSG_ID_LISUM_MANUAL_CTRL_HORNET,
	 .sysid = heli_mav_sysid,
	 .compid = heli_mav_compid},

	{.msgid = MAVLINK_MSG_ID_COMMAND_LONG,
	 .sysid = heli_mav_sysid,
	 .compid = heli_mav_compid},

	{.msgid = MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL,
	 .sysid = heli_mav_sysid,
	 .compid = heli_mav_compid},

	{.msgid = MAVLINK_MSG_ID_NAMED_VALUE_FLOAT,
	 .sysid = heli_mav_sysid,
	 .compid = heli_mav_compid},
		
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
	

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

static const mav_track_t mav_gs_mp_tracks[MAV_MISSION_PLANNER_TRACK_COUNT] = {

	{.msgid = MAVLINK_MSG_ID_HEARTBEAT,
	 .sysid = mp_mav_sysid,
	 .compid = mp_mav_compid},

	{.msgid = MAVLINK_MSG_ID_REQUEST_DATA_STREAM,
	 .sysid = mp_mav_sysid,
	 .compid = mp_mav_compid},

	{.msgid = MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL,
	 .sysid = mp_mav_sysid,
	 .compid = mp_mav_compid},

	{.msgid = MAVLINK_MSG_ID_RADIO_STATUS,
	 .sysid = mp_mav_sysid,
	 .compid = mp_mav_compid},

	{.msgid = MAVLINK_MSG_ID_COMMAND_LONG,
	 .sysid = mp_mav_sysid,
	 .compid = mp_mav_compid},

	{.msgid = MAVLINK_MSG_ID_PARAM_REQUEST_LIST,
	 .sysid = mp_mav_sysid,
	 .compid = mp_mav_compid},

	{.msgid = MAVLINK_MSG_ID_NAMED_VALUE_FLOAT,
	 .sysid = mp_mav_sysid,
	 .compid = mp_mav_compid}};

#endif

/* --------------------------- MAVLink handlers --------------------------------
 */
static mav_t mav_gw_sky_handle __attribute__((section("AHBSRAM0")));
static mav_t mav_heli_handle __attribute__((section("AHBSRAM0")));
static mav_t mav_motor_handle __attribute__((section("AHBSRAM0")));
static mav_t mav_fg_handle __attribute__((section("AHBSRAM0")));
static mav_t mav_maps_handle __attribute__((section("AHBSRAM0")));
static mav_t mav_bb_handle __attribute__((section("AHBSRAM0")));

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

static mav_t mav_gs_mp_handle __attribute__((section("AHBSRAM0")));

#endif

static mav_link_t mav_link_gw_sky_udp __attribute__((section("AHBSRAM0")));
static mav_link_t mav_link_gw_sky_uart __attribute__((section("AHBSRAM0")));
static mav_link_t mav_link_heli_udp __attribute__((section("AHBSRAM0")));
static mav_link_t mav_link_motor_udp __attribute__((section("AHBSRAM0")));
static mav_link_t mav_link_fg_udp __attribute__((section("AHBSRAM0")));
static mav_link_t mav_link_maps_udp __attribute__((section("AHBSRAM0")));
static mav_link_t mav_link_bb_udp __attribute__((section("AHBSRAM0")));

#if MISSION_PLANNER_UDP_TRANSPORT_LAYER

static mav_link_t mav_link_gs_mp_udp __attribute__((section("AHBSRAM0")));

#endif

#if MISSION_PLANNER_UART_TRANSPORT_LAYER

static mav_link_t mav_link_gs_mp_uart __attribute__((section("AHBSRAM0")));

#endif

/* ------------------------------- Variables ---------------------------------*/

/* Extern temporary variables */
extern uint64_t boot_time_ms;

/* MAVLink Component Information Basic defines */
const char vendor_name[] = VENDOR_NAME;
const char model_name[] = MODEL_NAME;
const char software_version[] = SOFTWARE_VERSION;
const char hardware_version[] = HARDWARE_VERSION;
const char serial_number[] = SERIAL_NUMBER;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* MAVLink gateway sky receive callback */
static void mav_gw_sky_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg);

/* MAVLink ground station receive callback */
static void mav_gs_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

static void mav_gs_mp_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg);

#endif

/* heartbeat timer callback */
static void timer_mav_hb_cb(TimerHandle_t xTimer);

/*******************************************************************************
 * Code
 ******************************************************************************/

void task_mav(void* arg)
{
	QueueSetMemberHandle_t queue_member = NULL;
	mavlink_message_t msg;
	mavlink_message_t msg_gs_mp;
	TimerHandle_t timer_mav_hb;

	/*--------------------------------- MAV ----------------------------------*/
	/* initialize MAVLink handles */
	mav_init(&mav_gw_sky_handle, dev_mav_sysid, dev_mav_compid);

	mav_init(&mav_heli_handle, dev_mav_sysid, dev_mav_compid);

	mav_init(&mav_motor_handle, dev_mav_sysid, dev_mav_compid);

	mav_init(&mav_fg_handle, dev_mav_sysid, dev_mav_compid);

	mav_init(&mav_maps_handle, dev_mav_sysid, dev_mav_compid);

	mav_init(&mav_bb_handle, dev_mav_sysid, dev_mav_compid);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER
	mav_init(&mav_gs_mp_handle, dev_mav_sysid, dev_mav_compid);
#endif

	/* initialize MAVLink UDP links */
	mav_link_init(&mav_link_gw_sky_udp, MAVLINK_COMM_0, mav_gw_sky_recv_cb,
				  NULL, (tl_t*)&mav_gw_sky_udp_tl);

	// mav_link_init(&mav_link_gw_sky_uart,
	// 				 MAVLINK_COMM_1,
	// 				 mav_gw_sky_recv_cb,
	//  			 NULL,
	// 				 (tl_t *)&mav_gw_sky_uart_tl);

	mav_link_init(&mav_link_heli_udp, MAVLINK_COMM_2, mav_gs_recv_cb, NULL,
				  (tl_t*)&mav_heli_tl);

	mav_link_init(&mav_link_motor_udp, MAVLINK_COMM_2, mav_gs_recv_cb, NULL,
				  (tl_t*)&mav_motor_tl);

	mav_link_init(&mav_link_fg_udp, MAVLINK_COMM_2, mav_gs_recv_cb, NULL,
				  (tl_t*)&mav_fg_tl);

	mav_link_init(&mav_link_maps_udp, MAVLINK_COMM_2, mav_gs_recv_cb, NULL,
				  (tl_t*)&mav_maps_tl);

	mav_link_init(&mav_link_bb_udp, MAVLINK_COMM_2, mav_gs_recv_cb, NULL,
				  (tl_t*)&mav_bb_tl);

#if MISSION_PLANNER_UDP_TRANSPORT_LAYER

	mav_link_init(&mav_link_gs_mp_udp, MAVLINK_COMM_3, mav_gs_mp_recv_cb, NULL,
				  (tl_t*)&mav_gs_mp_tl);

#endif

#if MISSION_PLANNER_UART_TRANSPORT_LAYER

	mav_link_init(&mav_link_gs_mp_uart, MAVLINK_COMM_3, mav_gs_mp_recv_cb, NULL,
				  (tl_t*)&mav_gs_mp_uart_tl);

#endif

	/* connect MAVLink links to MAVLink handles */
	mav_link(&mav_gw_sky_handle, &mav_link_gw_sky_udp);

	// mav_link(&mav_gw_sky_handle, &mav_link_gw_sky_uart);

	mav_link(&mav_heli_handle, &mav_link_heli_udp);

	mav_link(&mav_motor_handle, &mav_link_motor_udp);

	mav_link(&mav_fg_handle, &mav_link_fg_udp);

	mav_link(&mav_maps_handle, &mav_link_maps_udp);

#if MISSION_PLANNER_UDP_TRANSPORT_LAYER

	mav_link(&mav_gs_mp_handle, &mav_link_gs_mp_udp);

#endif

#if MISSION_PLANNER_UART_TRANSPORT_LAYER

	mav_link(&mav_gs_mp_handle, &mav_link_gs_mp_uart);

#endif

	/* configure signing */
	mav_sign(&mav_gw_sky_handle, mav_key, NULL);

	/* track gateway sky handle messages */
	for (uint8_t i = 0; i < MAV_GW_SKY_TRACK_COUNT; i++) {

		mav_track_t* track;

		track = (mav_track_t*)&mav_gw_sky_tracks[i];

		mav_track(&mav_gw_sky_handle, track->msgid, track->sysid,
				  track->compid);
	}

	/* track heli handle messages */
	for (uint8_t i = 0; i < MAV_HELI_TRACK_COUNT; i++) {

		mav_track_t* track;

		track = (mav_track_t*)&mav_heli_tracks[i];

		mav_track(&mav_heli_handle, track->msgid, track->sysid, track->compid);
	}

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

	/* track Mission Planner messages */
	for (uint8_t i = 0; i < MAV_MISSION_PLANNER_TRACK_COUNT; i++) {

		mav_track_t* track;

		track = (mav_track_t*)&mav_gs_mp_tracks[i];

		mav_track(&mav_gs_mp_handle, track->msgid, track->sysid, track->compid);
	}

#endif

	/* create MAVLink heartbeat timer */
	timer_mav_hb = xTimerCreate("tim_mav_hb", pdMS_TO_TICKS(MAV_HB_PERIOD_MS),
								pdTRUE, NULL, timer_mav_hb_cb);

	/* start heartbeat timer */
	xTimerStart(timer_mav_hb, 0);

	for (;;) {

		queue_member = xQueueSelectFromSet(queueset_mav, portMAX_DELAY);

		if (queue_member == queue_mav_hb) {

			mavlink_heartbeat_t data;

			mavlink_heartbeat_t data_gs_mp;

			xQueueReceive(queue_mav_hb, &data, portMAX_DELAY);

			xSemaphoreTake(mutex_mav, portMAX_DELAY);

			memcpy(&data_gs_mp, &data, sizeof(data));

			mavlink_msg_heartbeat_encode_chan(dev_mav_sysid, dev_mav_compid,
											  MAVLINK_COMM_0, &msg, &data);

			mav_send(&mav_gw_sky_handle, &msg);

			mavlink_status_t* status =
				mavlink_get_channel_status(MAVLINK_COMM_3);
			status->signing = NULL;

			mavlink_msg_heartbeat_encode_chan(dev_mav_sysid, dev_mav_compid,
											  MAVLINK_COMM_3, &msg_gs_mp,
											  &data_gs_mp);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

			mav_send(&mav_gs_mp_handle, &msg_gs_mp);
			mav_send(&mav_heli_handle, &msg);

#endif

			xSemaphoreGive(mutex_mav);
		}
	}
}

static void mav_gw_sky_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg)
{

	mavlink_message_t tx_msg;

	mavlink_message_t tx_msg_mp;

	xSemaphoreTake(mutex_mav, portMAX_DELAY);

	switch (msg->msgid) {

	case MAVLINK_MSG_ID_HEARTBEAT: {

		mavlink_heartbeat_t data;

		mavlink_msg_heartbeat_decode(msg, &data);

		if (msg->sysid == gw_sky_mav_sysid &&
			msg->compid == gw_sky_mav_compid &&
			data.type == MAV_TYPE_ONBOARD_CONTROLLER) {

			mavlink_msg_heartbeat_encode(gw_sky_mav_sysid, gw_sky_mav_compid,
										 &tx_msg, &data);

			HAL_GPIO_SetPinValue(
				GPIO_HAL_INSTANCE_3, 25,
				!HAL_GPIO_GetPinValue(GPIO_HAL_INSTANCE_3, 25));

			mav_send(&mav_heli_handle, &tx_msg);

		} else if (msg->sysid == act_master_mav_sysid &&
				   msg->compid == act_master_mav_compid) {

			mav_send(&mav_heli_handle, &tx_msg);
		} else if (msg->sysid == bb_mav_sysid && msg->compid == bb_mav_compid &&
				   data.type == MAV_TYPE_LOG) {

			mavlink_msg_heartbeat_encode(bb_mav_sysid, bb_mav_compid, &tx_msg,
										 &data);

			mav_send(&mav_heli_handle, &tx_msg);
		}

		break;
	}
	case MAVLINK_MSG_ID_COMMAND_ACK: {

		mavlink_command_ack_t data;

		mavlink_msg_command_ack_decode(msg, &data);

		mavlink_msg_command_ack_encode(dev_mav_sysid, dev_mav_compid, &tx_msg,
									   &data);

		if (data.target_system == heli_mav_sysid &&
			data.target_component == heli_mav_compid) {

			mav_send(&mav_heli_handle, &tx_msg);
		}

		mavlink_msg_command_ack_encode_chan(dev_mav_sysid, dev_mav_compid,
											MAVLINK_COMM_3, &tx_msg, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_LISUM_POWER_HORNET_ACT_DATA: {
		/* route lisum power hornet actuator data */
		mavlink_lisum_power_hornet_act_data_t data;

		mavlink_msg_lisum_power_hornet_act_data_decode(msg, &data);

		mavlink_msg_lisum_power_hornet_act_data_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_lisum_power_hornet_act_data_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_LISUM_MANUAL_CTRL_HORNET: {
		/* route lisum manual control hornet */
		mavlink_lisum_manual_ctrl_hornet_t data;

		mavlink_msg_lisum_manual_ctrl_hornet_decode(msg, &data);

		mavlink_msg_lisum_manual_ctrl_hornet_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_lisum_manual_ctrl_hornet_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_SCALED_IMU: {
		/* route scaled imu data */
		mavlink_scaled_imu_t data;

		mavlink_msg_scaled_imu_decode(msg, &data);

		mavlink_msg_scaled_imu_encode(dev_mav_sysid, dev_mav_compid, &tx_msg,
									  &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_scaled_imu_encode_chan(dev_mav_sysid, dev_mav_compid,
										   MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_SCALED_IMU2: {
		/* route scaled imu2 data */
		mavlink_scaled_imu2_t data;

		mavlink_msg_scaled_imu2_decode(msg, &data);

		mavlink_msg_scaled_imu2_encode(dev_mav_sysid, dev_mav_compid, &tx_msg,
									   &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_scaled_imu2_encode_chan(dev_mav_sysid, dev_mav_compid,
											MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_SCALED_PRESSURE: {
		/* route scaled pressure */
		mavlink_scaled_pressure_t data;

		mavlink_msg_scaled_pressure_decode(msg, &data);

		mavlink_msg_scaled_pressure_encode(dev_mav_sysid, dev_mav_compid,
										   &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_scaled_pressure_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_ALTITUDE: {
		/* route altitude */
		mavlink_altitude_t data;

		mavlink_msg_altitude_decode(msg, &data);

		mavlink_msg_altitude_encode(dev_mav_sysid, dev_mav_compid, &tx_msg,
									&data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_altitude_encode_chan(dev_mav_sysid, dev_mav_compid,
										 MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_ATTITUDE: {
		/* route attitude */
		mavlink_attitude_t data;

		mavlink_msg_attitude_decode(msg, &data);

		mavlink_msg_attitude_encode(dev_mav_sysid, dev_mav_compid, &tx_msg,
									&data);

		mav_send(&mav_heli_handle, &tx_msg);
		mav_send(&mav_fg_handle, &tx_msg);
		mav_send(&mav_maps_handle, &tx_msg);

		mavlink_msg_attitude_encode_chan(dev_mav_sysid, dev_mav_compid,
										 MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_LISUM_GNSS_RECV_DATA: {
		/* route lisum gnss recv data */
		mavlink_lisum_gnss_recv_data_t data;

		mavlink_msg_lisum_gnss_recv_data_decode(msg, &data);

		mavlink_msg_lisum_gnss_recv_data_encode(dev_mav_sysid, dev_mav_compid,
												&tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);
		mav_send(&mav_fg_handle, &tx_msg);
		mav_send(&mav_maps_handle, &tx_msg);

		mavlink_msg_lisum_gnss_recv_data_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_LISUM_SENSOR_AIRSPEED_DATA: {
		/* route lisum sensor airspeed data */
		mavlink_lisum_sensor_airspeed_data_t data;

		mavlink_msg_lisum_sensor_airspeed_data_decode(msg, &data);

		mavlink_msg_lisum_sensor_airspeed_data_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_lisum_sensor_airspeed_data_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_BATTERY_STATUS: {
		/* route battery status */
		mavlink_battery_status_t data;

		mavlink_msg_battery_status_decode(msg, &data);

		mavlink_msg_battery_status_encode(dev_mav_sysid, dev_mav_compid,
										  &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_battery_status_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_LISUM_POWER_MOTOR_SCALED_DATA: {
		/* route lisum power motor scaled data */
		mavlink_lisum_power_motor_scaled_data_t data;

		mavlink_msg_lisum_power_motor_scaled_data_decode(msg, &data);

		mavlink_msg_lisum_power_motor_scaled_data_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);
		mav_send(&mav_motor_handle, &tx_msg);

		mavlink_msg_lisum_power_motor_scaled_data_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}
	case MAVLINK_MSG_ID_COMPONENT_INFORMATION_BASIC: {
		mavlink_component_information_basic_t data;

		mavlink_msg_component_information_basic_decode(msg, &data);

		mavlink_msg_component_information_basic_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}
	case MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL: {
		mavlink_file_transfer_protocol_t data;

		mavlink_msg_file_transfer_protocol_decode(msg, &data);

		mavlink_msg_file_transfer_protocol_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		mavlink_msg_file_transfer_protocol_encode_chan(
			dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_3, &tx_msg_mp, &data);

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

		mav_send(&mav_gs_mp_handle, &tx_msg_mp);

#endif

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_STATUS_1: {
				
		mavlink_lisum_vesc_status_1_t data;

		mavlink_msg_lisum_vesc_status_1_decode(msg, &data);

		mavlink_msg_lisum_vesc_status_1_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_STATUS_2: {
				
		mavlink_lisum_vesc_status_2_t data;

		mavlink_msg_lisum_vesc_status_2_decode(msg, &data);

		mavlink_msg_lisum_vesc_status_2_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_STATUS_3: {
				
		mavlink_lisum_vesc_status_3_t data;

		mavlink_msg_lisum_vesc_status_3_decode(msg, &data);

		mavlink_msg_lisum_vesc_status_3_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_STATUS_4: {
				
		mavlink_lisum_vesc_status_4_t data;

		mavlink_msg_lisum_vesc_status_4_decode(msg, &data);

		mavlink_msg_lisum_vesc_status_4_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_STATUS_5: {
				
		mavlink_lisum_vesc_status_5_t data;

		mavlink_msg_lisum_vesc_status_5_decode(msg, &data);

		mavlink_msg_lisum_vesc_status_5_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_STATUS_6: {
				
		mavlink_lisum_vesc_status_6_t data;

		mavlink_msg_lisum_vesc_status_6_decode(msg, &data);

		mavlink_msg_lisum_vesc_status_6_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}

		case MAVLINK_MSG_ID_LISUM_VESC_PING: {
				
		mavlink_lisum_vesc_ping_t data;

		mavlink_msg_lisum_vesc_ping_decode(msg, &data);

		mavlink_msg_lisum_vesc_ping_encode(
			dev_mav_sysid, dev_mav_compid, &tx_msg, &data);

		mav_send(&mav_heli_handle, &tx_msg);

		break;
	}

	default:
		break;
	}

	xSemaphoreGive(mutex_mav);
}

static void mav_gs_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg)
{
	mavlink_message_t tx_msg;

	xSemaphoreTake(mutex_mav, portMAX_DELAY);

	switch (msg->msgid) {

	case MAVLINK_MSG_ID_HEARTBEAT: {

		mavlink_heartbeat_t data;

		mavlink_msg_heartbeat_decode(msg, &data);

		break;
	}
	case MAVLINK_MSG_ID_LISUM_MANUAL_CTRL_HORNET: {
		/* route lisum manual control hornet */
		mavlink_lisum_manual_ctrl_hornet_t data;

		mavlink_msg_lisum_manual_ctrl_hornet_decode(msg, &data);

		mavlink_msg_lisum_manual_ctrl_hornet_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}
	case MAVLINK_MSG_ID_COMMAND_LONG: {
		/* route command long */
		mavlink_command_long_t data;

		mavlink_msg_command_long_decode(msg, &data);

		if (data.command == MAV_CMD_REQUEST_MESSAGE) {

			if ((uint16_t)data.param1 ==
				MAVLINK_MSG_ID_COMPONENT_INFORMATION_BASIC) {

				mavlink_message_t reply_msg;

				mavlink_component_information_basic_t reply_info;

				reply_info = make_comp_info_basic();

				mavlink_msg_component_information_basic_encode_chan(
					dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_0, &reply_msg,
					&reply_info);

				mav_send(&mav_heli_handle, &reply_msg);

			} else if ((uint16_t)data.param1 ==
					   MAVLINK_MSG_ID_AUTOPILOT_VERSION) {

				mavlink_message_t reply_msg_heli;

				mavlink_autopilot_version_t reply_info;

				reply_info = make_autopilot_version();

				mavlink_msg_autopilot_version_encode_chan(
					dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_0,
					&reply_msg_heli, &reply_info);

				mav_send(&mav_heli_handle, &reply_msg_heli);
			}
		}

		else if (data.command == MAV_CMD_DO_SET_MODE) {
			if (data.param1 == MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {

				mavlink_message_t tx_msg;

				mavlink_msg_autopilot_version_encode_chan(
					dev_mav_sysid, dev_mav_compid, MAVLINK_COMM_0, &tx_msg,
					&data);

				mav_send(&mav_gw_sky_handle, &tx_msg);
			}
		}

		mavlink_message_t tx_msg;

		mavlink_msg_command_long_encode_chan(msg->sysid, msg->compid,
											 MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}
	case MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL: {
		mavlink_file_transfer_protocol_t data;

		mavlink_msg_file_transfer_protocol_decode(msg, &data);

		mavlink_msg_file_transfer_protocol_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}
	case MAVLINK_MSG_ID_NAMED_VALUE_FLOAT: {
		mavlink_named_value_float_t data;

		mavlink_msg_named_value_float_decode(msg, &data);

		mavlink_msg_named_value_float_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);
		break;
	}
	case MAVLINK_MSG_ID_LISUM_VESC_SET_DUTY: {
				
		mavlink_lisum_vesc_set_duty_t data;

		mavlink_msg_lisum_vesc_set_duty_decode(msg, &data);

		mavlink_msg_lisum_vesc_set_duty_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_SET_CURRENT: {
				
		mavlink_lisum_vesc_set_current_t data;

		mavlink_msg_lisum_vesc_set_current_decode(msg, &data);

		mavlink_msg_lisum_vesc_set_current_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_SET_CURRENT_BRAKE: {
				
		mavlink_lisum_vesc_set_current_brake_t data;

		mavlink_msg_lisum_vesc_set_current_brake_decode(msg, &data);

		mavlink_msg_lisum_vesc_set_current_brake_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_SET_RPM: {
				
		mavlink_lisum_vesc_set_rpm_t data;

		mavlink_msg_lisum_vesc_set_rpm_decode(msg, &data);

		mavlink_msg_lisum_vesc_set_rpm_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_LISUM_VESC_SET_POS: {
				
		mavlink_lisum_vesc_set_pos_t data;

		mavlink_msg_lisum_vesc_set_pos_decode(msg, &data);

		mavlink_msg_lisum_vesc_set_pos_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}

		case MAVLINK_MSG_ID_LISUM_VESC_PING: {
				
		mavlink_lisum_vesc_ping_t data;

		mavlink_msg_lisum_vesc_ping_decode(msg, &data);

		mavlink_msg_lisum_vesc_ping_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}

	default:
		break;
	}

	xSemaphoreGive(mutex_mav);
}

#if MISSION_PLANNER_UART_TRANSPORT_LAYER || MISSION_PLANNER_UDP_TRANSPORT_LAYER

static void mav_gs_mp_recv_cb(mav_t* mav, mavlink_message_t* msg, void* arg)
{
	mavlink_message_t tx_msg;

	xSemaphoreTake(mutex_mav, portMAX_DELAY);

	switch (msg->msgid) {

	case MAVLINK_MSG_ID_HEARTBEAT: {

		break;
	}
	case MAVLINK_MSG_ID_RADIO_STATUS: {

		break;
	}
	case MAVLINK_MSG_ID_REQUEST_DATA_STREAM: {

		break;
	}
	case MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL: {
		mavlink_file_transfer_protocol_t data;

		mavlink_msg_file_transfer_protocol_decode(msg, &data);

		mavlink_msg_file_transfer_protocol_encode(msg->sysid, msg->compid,
												  &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);

		break;
	}
	case MAVLINK_MSG_ID_COMMAND_LONG: {

		mavlink_command_long_t data;

		mavlink_msg_command_long_decode(msg, &data);

		if (data.command == MAV_CMD_REQUEST_MESSAGE) {

			mavlink_msg_command_long_encode(msg->sysid, msg->compid, &tx_msg,
											&data);

			mav_send(&mav_gw_sky_handle, &tx_msg);
		}

		break;
	}
	case MAVLINK_MSG_ID_PARAM_REQUEST_LIST: {

		mavlink_message_t tx_msg;

		const char* param_id = "FTP_AVAILABLE";
		float param_value = 1.0f;
		uint16_t param_count = 1;
		uint16_t param_index = 0;
		uint8_t param_type = MAV_PARAM_TYPE_UINT8;

		mavlink_msg_param_value_pack(dev_mav_sysid, dev_mav_compid, &tx_msg,
									 param_id, param_value, param_type,
									 param_count, param_index);

		mav_send(&mav_gs_mp_handle, &tx_msg);

		break;
	}

	case MAVLINK_MSG_ID_NAMED_VALUE_FLOAT: {
		mavlink_named_value_float_t data;

		mavlink_msg_named_value_float_decode(msg, &data);

		mavlink_msg_named_value_float_encode_chan(
			msg->sysid, msg->compid, MAVLINK_COMM_0, &tx_msg, &data);

		mav_send(&mav_gw_sky_handle, &tx_msg);
		break;
	}
	default:
		break;
	}

	xSemaphoreGive(mutex_mav);
}
#endif

static void timer_mav_hb_cb(TimerHandle_t xTimer)
{
	mavlink_heartbeat_t mav_hb = {.type = MAV_TYPE_GENERIC,
								  .autopilot = MAV_AUTOPILOT_GENERIC,
								  .base_mode = 0,
								  .custom_mode = 0,
								  .system_status = MAV_STATE_ACTIVE};

	xQueueSendToBack(queue_mav_hb, &mav_hb, 0);
}