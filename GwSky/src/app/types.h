/**
 * @file    types.h
 * @brief   Project common defines and types.
 * @version 1.0.0
 * @date    09.04.2025
 * @author  LisumLab
 */

#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
/* External hardware drivers */
#include "cyphal_mavlink_publishers.h"
#include "cyphal_reg_publishers.h"
#include "cyphal_uavcan_publishers.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Lib */
#include "can.h"
#include "canard.h"
#include "cyphal_utility.h"
#include "mavlink/messages_cyphal_uavcan/common/AdsbVehicle_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/AisVessel_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/Attitude_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/AutopilotVersion_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/CommandLong_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/FileTransferProtocol_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/NamedValueFloat_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/ScaledImu_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumGnssRecvData_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescSetDuty_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescSetCurrent_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescSetCurrentBrake_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescSetRpm_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescSetPos_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescStatus1_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescStatus2_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescStatus3_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescStatus4_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescStatus5_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescStatus6_1_0.h"
#include "mavlink/messages_cyphal_uavcan/lisum/LisumVescPing_1_0.h"
#include "reg/udral/physics/thermodynamics/PressureTempVarTs_0_1.h"
#include "uavcan/si/sample/length/Scalar_1_0.h"
#include "uavcan/si/unit/duration/Scalar_1_0.h"
#include "uavcan/si/unit/pressure/Scalar_1_0.h"
#include "uavcan/si/unit/temperature/Scalar_1_0.h"
#include "uavcan/uavcan/node/GetInfo_1_0.h"
#include "uavcan/uavcan/node/Heartbeat_1_0.h"
#include "uavcan/uavcan/node/Mode_1_0.h"
#include "uavcan/uavcan/primitive/array/Integer8_1_0.h"



/*******************************************************************************
 * Defines
 ******************************************************************************/

/* blinky period [ms] */
#define BLINKY_PERIOD_MS 500

/* Telemetry UART instance */
#define TELEMETRY_UART_INSTANCE UART_HAL_INSTANCE_3
/* Telemetry UART baud rate */
#define TELEMETRY_UART_BAUD_RATE 230400
/* Telemetry UART receive queue length in bytes */
#define TELEMETRY_RX_QUEUE_LEN 1024
/* Telemetry UART transmit queue length in bytes */
#define TELEMETRY_TX_QUEUE_LEN 1024

/* DATA FOR ARDUPILOT VERSION MESSAGE */
#define PAYLOAD_LENGTH 8

/* Read from src/middleware/FreeRTOS/manifest.yml */
#define OS_CUSTOM_VERSION OS_CUSTOM_HASH

/* BlackBox Board Version */
#define BOARD_TYPE 8	 // Read ethernet port marked by marker
#define BOARD_REVISION 2 // This is Hardware Version V0.2

/* Vendor and product IDs*/
#define VENDOR_ID 0x4254 // 'B' 'T' Inicialize of company
#define PRODUCT_ID 8	 // Number of model

#define VENDOR_NAME "BetaTehPro"
#define MODEL_NAME "HW_GW_SKY"
#define SOFTWARE_VERSION FLIGHT_CUSTOM_VERSION
#define HARDWARE_VERSION "DEV-Board V0.2"
#define SERIAL_NUMBER "S_N: 008"

/* Cyphal timeouts [us] */
#define CYPHAL_CRITICAL_TIMEOUT 10000U
#define CYPHAL_MEDIUM_TIMEOUT 50000U
#define CYPHAL_LOW_TIMEOUT 1000000U

/* Mutex for Cyphal TX queuesss */
extern SemaphoreHandle_t tx_queue_mutex;

/* Mutex macros for Cyphal TX queue */
#define TX_QUEUE_MUTEX_TAKE                                                    \
	if (xSemaphoreTake(tx_queue_mutex, portMAX_DELAY) == pdTRUE)
#define TX_QUEUE_MUTEX_GIVE xSemaphoreGive(tx_queue_mutex)


#define MAVLINK_OR_CYPHAL 0 /* 1 is for MAVLink & UDP, 0 is for Cyphal CAN Bus */

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

typedef struct cyphal_subscription_messages_t {
	const enum CanardTransferKind kind;
	const CanardPortID port_id;
	const size_t serialization_buffer_size;
	struct CanardRxSubscription* subscription;
} cyphal_subscription_messages_t;

typedef enum{
	NODE_MODE_OPERATIONAL = uavcan_node_Mode_1_0_OPERATIONAL,
	NODE_MODE_INITIALIZATION = uavcan_node_Mode_1_0_INITIALIZATION,
	NODE_MODE_MAINTENANCE = uavcan_node_Mode_1_0_MAINTENANCE,
	NODE_MODE_SOFTWARE_UPDATE = uavcan_node_Mode_1_0_SOFTWARE_UPDATE,
	NODE_MODE_IDLE = 4,
	NODE_MODE_CRITICAL_FAILURE = 5
}node_mode_state_t;


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* TYPES_H */