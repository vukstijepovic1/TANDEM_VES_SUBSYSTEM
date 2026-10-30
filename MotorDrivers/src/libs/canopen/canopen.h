/**
 * @file    canopen.h
 * @brief   CANopen Library.
 * @version 1.1.0
 * @date    06.03.2025
 * @author  LisumLab
 */

#ifndef CANOPEN_H
#define CANOPEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "can.h"
#include "common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* CAN Communication Timeout [us] */
#ifndef CANOPEN_CAN_TIMEOUT_US
#define CANOPEN_CAN_TIMEOUT_US 600
#endif

// Broadcast node id
#define CANOPEN_NODE_ID_BROADCAST 0x00U

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief COB Type */
typedef enum canopen_cob_type_e {
	CANOPEN_COB_TYPE_NMT = 0x000,		  //!< COB Type NMT
	CANOPEN_COB_TYPE_SYNC = 0x080,		  //!< COB Type Sync
	CANOPEN_COB_TYPE_EMG = 0x080,		  //!< COB Type EMG
	CANOPEN_COB_TYPE_TMS = 0x100,		  //!< COB Type TMS
	CANOPEN_COB_TYPE_PDO1_TX = 0x180,	  //!< COB Type PDO1 Tx
	CANOPEN_COB_TYPE_PDO1_RX = 0x200,	  //!< COB Type PDO1 Rx
	CANOPEN_COB_TYPE_PDO2_TX = 0x280,	  //!< COB Type PDO2 Tx
	CANOPEN_COB_TYPE_PDO2_RX = 0x300,	  //!< COB Type PDO2 Rx
	CANOPEN_COB_TYPE_PDO3_TX = 0x380,	  //!< COB Type PDO3 Tx
	CANOPEN_COB_TYPE_PDO3_RX = 0x400,	  //!< COB Type PDO3 Rx
	CANOPEN_COB_TYPE_PDO4_TX = 0x480,	  //!< COB Type PDO4 Tx
	CANOPEN_COB_TYPE_PDO4_RX = 0x500,	  //!< COB Type PDO4 Rx
	CANOPEN_COB_TYPE_SDO_TX = 0x580,	  //!< COB Type SDO Tx
	CANOPEN_COB_TYPE_SDO_RX = 0x600,	  //!< COB Type SDO Rx
	CANOPEN_COB_TYPE_NMT_ERR_CTRL = 0x700 //!< COB Type NMT error control
} canopen_cob_type_e;

/*! @brief NMT Message Command Specifier */
typedef enum canopen_nmt_cs_e {
	//!< Start Remote Node Protocol
	CANOPEN_NMT_CS_START_REMOTE_NODE = 0x01,
	//!< Enter Stopped Protocol
	CANOPEN_NMT_CS_ENTER_STOPPED = 0x02,
	//!< Enter Pre-Operational Protocol
	CANOPEN_NMT_CS_ENTER_PRE_OPERATIONAL = 0x80,
	//!< Enter Reset Application Protocol
	CANOPEN_NMT_CS_ENTER_RESET_APP = 0x81,
	//!< Enter Reset Communication Protocol
	CANOPEN_NMT_CS_ENTER_RESET_COMM = 0x82
} canopen_nmt_cs_e;

typedef enum canopen_nmt_slave_state_e {
	//!< NMT Slave is in Boot state
	CANOPEN_NMT_SLAVE_STATE_BOOT = 0x00,
	//!< NMT Slave is in Stopped state
	CANOPEN_NMT_SLAVE_STATE_STOPPED = 0x04,
	//!< NMT Slave is in Operational state
	CANOPEN_NMT_SLAVE_STATE_OPERATIONAL = 0x05,
	//!< NMT Slave is in Pre-Operational state
	CANOPEN_NMT_SLAVE_STATE_PRE_OPERATIONAL = 0x7F
} canopen_nmt_slave_state_e;

/*! @brief CANopen Object Dictionary Entry */
typedef struct canopen_od_entry_t {
	uint16_t idx;		//!< Index
	uint8_t subidx;		//!< Subindex
	uint32_t data_size; //!< Data Size [bytes]
} canopen_od_entry_t;

typedef void canopen_callback_t(void *user_arg, canopen_cob_type_e type,
								uint8_t node_id, uint8_t *data,
								uint8_t data_size);

/*! @brief CANopen handle */
typedef struct canopen_handle_t {
	can_hal_instance_t instance; //!< CAN Instance.
	uint8_t node_id;

	canopen_callback_t *cb; //!< On receive callback.
	void *arg;				//!< User arg.
} canopen_handle_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize CANopen and configure CANx Receive Interrupt.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] instance  CAN Instance.
 * @param[in] node_id   Device Node ID.
 * @return None
 */
void canopen_init(canopen_handle_t *handle, can_hal_instance_t instance,
				  uint8_t node_id);

/**
 * @brief Initialize CANopen and configure CANx Receive Interrupt.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] cb        Callback function.
 * @param[in] arg       User arg.
 * @return None
 */
void canopen_callback_config(canopen_handle_t *handle, canopen_callback_t cb,
							 void *arg);

/**
 * @brief Send CANopen Network Management message.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] node_id   Target Node ID.
 * @param[in] cs        NMT Message Command Specifier.
 * @return Status
 */
lStatus_t canopen_nmt(canopen_handle_t *handle, uint8_t node_id,
					  canopen_nmt_cs_e cs);

/**
 * @brief Send Sync message.
 *
 * @param[in] handle    CANopen handle.
 * @return Status
 */
lStatus_t canopen_sync(canopen_handle_t *handle);

/**
 * @brief Send CANopen PDO1 (Process Data Object) RX message.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] node_id   Target Node ID.
 * @param[in] data      Transmit data.
 * @param[in] size      Transmit data size.
 * @return Status
 */
lStatus_t canopen_pdo1rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size);

/**
 * @brief Send CANopen PDO2 (Process Data Object) RX message.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] node_id   Target Node ID.
 * @param[in] data      Transmit data.
 * @param[in] size      Transmit data size.
 * @return Status
 */
lStatus_t canopen_pdo2rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size);

/**
 * @brief Send CANopen PDO3 (Process Data Object) RX message.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] node_id   Target Node ID.
 * @param[in] data      Transmit data.
 * @param[in] size      Transmit data size.
 * @return Status
 */
lStatus_t canopen_pdo3rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size);

/**
 * @brief Send CANopen PDO4 (Process Data Object) RX message.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] node_id   Target Node ID.
 * @param[in] data      Transmit data.
 * @param[in] size      Transmit data size.
 * @return Status
 */
lStatus_t canopen_pdo4rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size);

/**
 * @brief   Write Service Data Object (Protocol CANopen SDO Download)
 *          Client (CANopen master) requests SDO write from Server
 *          (CANopen slave).
 * @note    Current implementation supports only Expedited Transfers with
 *          set Size Indicator (Refer to CiA 301 V4.2 page 48).
 *
 * @param[in] handle        CANopen handle.
 * @param[in] node_id       Target Node ID.
 * @param[in] mtdt          Service Data Object Message metadata.
 * @param[in] data          Transmit data.
 * @param[in] timeout_ms    Response timeout [ms].
 * @return Status
 */
lStatus_t canopen_sdo_download(canopen_handle_t *handle, uint8_t node_id,
							   const canopen_od_entry_t *mtdt, void *data,
							   uint32_t timeout_ms);

/**
 * @brief   Read Service Data Object (CANopen SDO Upload).
 *          Client (CANopen master) requests SDO read from Server
 *          (CANopen slave).
 * @note    Current implementation supports only Expedited Transfers with
 *          set Size Indicator (Refer to CiA 301 V4.2 page 50).
 *
 * @param[in] handle        CANopen handle.
 * @param[in] node_id       Target Node ID.
 * @param[in] mtdt          Service Data Object Message metadata.
 * @param[in] data          Receive data.
 * @param[in] timeout_ms    Response timeout [ms].
 * @return Status
 */
lStatus_t canopen_sdo_upload(canopen_handle_t *handle, uint8_t node_id,
							 const canopen_od_entry_t *mtdt, void *data,
							 uint32_t timeout_ms);

/**
 * @brief Send CANopen Heartbeat.
 *
 * @param[in] handle    CANopen handle.
 * @param[in] state     Node state.
 * @return Status
 */
lStatus_t canopen_heartbeat(canopen_handle_t *handle,
							canopen_nmt_slave_state_e state);

#ifdef __cplusplus
}
#endif

#endif /* CANOPEN_H */