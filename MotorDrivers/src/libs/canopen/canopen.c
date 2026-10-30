/**
 * @file    canopen.c
 * @brief   CANopen Library.
 * @version 1.1.0
 * @date    06.03.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>

#include "canopen.h"
#include "canopen_def.h"

#include "util.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief CANopen SDO abort code */
typedef enum canopen_sdo_abort_code_e {
	CANOPEN_SDO_ABORT_CODE_GENERAL_ERR = 0x08000000
} canopen_sdo_abort_code_e;

/*! @brief CANopen NMT data. */
typedef struct __attribute__((__packed__)) canopen_nmt_data_t {
	uint8_t cs;		 //!< Command specifier
	uint8_t node_id; //!< Server node id
} canopen_nmt_data_t;

/*! @brief SDO abort flags */
typedef struct __attribute__((__packed__)) canopen_sdo_abort_flags_t {
	unsigned res : 5; //!< Reserved
	unsigned cs : 3;  //!< Client command specifier
} canopen_sdo_abort_flags_t;

/*! @brief SDO abort */
typedef struct __attribute__((__packed__)) canopen_sdo_abort_t {
	canopen_sdo_abort_flags_t flags; //!< Flags
	uint16_t idx;					 //!< Index
	uint8_t subidx;					 //!< Subindex
	uint32_t code;					 //!< Abort code
} canopen_sdo_abort_t;

/*! @brief SDO download initiate request flags */
typedef struct __attribute__((
	__packed__)) canopen_sdo_download_init_req_flags_t {
	unsigned s : 1;	  //!< Size indicator
	unsigned e : 1;	  //!< Transfer type, 0 - normal, 1 - expedited
	unsigned n : 2;	  //!< Number of unused bytes in data section
	unsigned res : 1; //!< Reserved
	unsigned ccs : 3; //!< Client command specifier
} canopen_sdo_download_init_req_flags_t;

/*! @brief SDO download initiate request */
typedef struct __attribute__((__packed__)) canopen_sdo_download_init_req_t {
	canopen_sdo_download_init_req_flags_t flags;  //!< Flags
	uint16_t idx;								  //!< Index
	uint8_t subidx;								  //!< Subindex
	uint8_t data[CANOPEN_DEF_SDO_INIT_DATA_SIZE]; //!< Data
} canopen_sdo_download_init_req_t;

/*! @brief SDO download initiate response flags */
typedef struct __attribute__((
	__packed__)) canopen_sdo_download_init_resp_flags_t {
	unsigned res : 5; //!< Reserved
	unsigned scs : 3; //!< Server command specifier
} canopen_sdo_download_init_resp_flags_t;

/*! @brief SDO download initiate response */
typedef struct __attribute__((__packed__)) canopen_sdo_download_init_resp_t {
	canopen_sdo_download_init_resp_flags_t flags; //!< Flags
	uint16_t idx;								  //!< Index
	uint8_t subidx;								  //!< Subindex
	uint8_t res[4];								  //!< Reserved
} canopen_sdo_download_init_resp_t;

/*! @brief SDO download segment request flags */
typedef struct __attribute__((
	__packed__)) canopen_sdo_download_seg_req_flags_t {
	unsigned c : 1;	  //!< Indicates are there more segments to be downloaded
	unsigned n : 3;	  //!< Number of unused bytes in data section
	unsigned t : 1;	  //!< Toggle bit
	unsigned ccs : 3; //!< Client command specifier
} canopen_sdo_download_seg_req_flags_t;

/*! @brief SDO download segment request */
typedef struct __attribute__((__packed__)) canopen_sdo_download_seg_req_t {
	canopen_sdo_download_seg_req_flags_t flags;	 //!< Flags
	uint8_t data[CANOPEN_DEF_SDO_SEG_DATA_SIZE]; //!< Data
} canopen_sdo_download_seg_req_t;

/*! @brief SDO download segment response flags */
typedef struct __attribute__((
	__packed__)) canopen_sdo_download_seg_resp_flags_t {
	unsigned res : 4; //!< Reserved
	unsigned t : 1;	  //!< Toggle bit
	unsigned scs : 3; //!< Server command specifier
} canopen_sdo_download_seg_resp_flags_t;

/*! @brief SDO download segment response */
typedef struct __attribute__((__packed__)) canopen_sdo_download_seg_resp_t {
	canopen_sdo_download_seg_resp_flags_t flags; //!< Flags
	uint8_t res[7];								 //!< Reserved
} canopen_sdo_download_seg_resp_t;

/*! @brief SDO upload initiate request flags */
typedef struct __attribute__((__packed__)) canopen_sdo_upload_init_req_flags_t {
	unsigned res : 5; //!< Reserved
	unsigned ccs : 3; //!< Client command specifier
} canopen_sdo_upload_init_req_flags_t;

/*! @brief SDO upload initiate request */
typedef struct __attribute__((__packed__)) canopen_sdo_upload_init_req_t {
	canopen_sdo_upload_init_req_flags_t flags; //!< Flags
	uint16_t idx;							   //!< Index
	uint8_t subidx;							   //!< Subindex
	uint8_t res[4];							   //!< Reserved
} canopen_sdo_upload_init_req_t;

/*! @brief SDO upload initiate response flags */
typedef struct __attribute__((
	__packed__)) canopen_sdo_upload_init_resp_flags_t {
	unsigned s : 1;	  //!< Size indicator
	unsigned e : 1;	  //!< Transfer type, 0 - normal, 1 - expedited
	unsigned n : 2;	  //!< Number of unused bytes in data section
	unsigned res : 1; //!< Reserved
	unsigned scs : 3; //!< Server command specifier
} canopen_sdo_upload_init_resp_flags_t;

/*! @brief SDO upload initiate response */
typedef struct __attribute__((__packed__)) canopen_sdo_upload_init_resp_t {
	canopen_sdo_upload_init_resp_flags_t flags;	  //!< Flags
	uint16_t idx;								  //!< Index
	uint8_t subidx;								  //!< Subindex
	uint8_t data[CANOPEN_DEF_SDO_INIT_DATA_SIZE]; //!< Data
} canopen_sdo_upload_init_resp_t;

/*! @brief SDO upload segment request flags */
typedef struct __attribute__((__packed__)) canopen_sdo_upload_seg_req_flags_t {
	unsigned res : 4; //!< Reserved
	unsigned t : 1;	  //!< Toggle bit
	unsigned ccs : 3; //!< Client command specifier
} canopen_sdo_upload_seg_req_flags_t;

/*! @brief SDO upload segment request */
typedef struct __attribute__((__packed__)) canopen_sdo_upload_seg_req_t {
	canopen_sdo_upload_seg_req_flags_t flags; //!< Flags
	uint8_t res[7];							  //!< Reserved
} canopen_sdo_upload_seg_req_t;

/*! @brief SDO upload segment response flags */
typedef struct __attribute__((__packed__)) canopen_sdo_upload_seg_resp_flags_t {
	unsigned c : 1;	  //!< Indicates are there more segments to be downloaded
	unsigned n : 3;	  //!< Number of unused bytes in data section
	unsigned t : 1;	  //!< Toggle bit
	unsigned scs : 3; //!< Server command specifier
} canopen_sdo_upload_seg_resp_flags_t;

/*! @brief SDO upload segment response */
typedef struct __attribute__((__packed__)) canopen_sdo_upload_seg_resp_t {
	canopen_sdo_upload_seg_resp_flags_t flags;	 //!< Flags
	uint8_t data[CANOPEN_DEF_SDO_SEG_DATA_SIZE]; //!< Data
} canopen_sdo_upload_seg_resp_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* CAN Callback, read incoming packet and notify upper layer. */
void canopen_can_callback(void *usr_arg, uint32_t intStatus);

/* Send PDOnRx message. */
static lStatus_t canopen_pdorx(canopen_handle_t *handle,
							   canopen_cob_type_e cob_type, uint8_t node_id,
							   void *data, uint8_t size);

/* Poll SDO Rx message. */
static lStatus_t canopen_sdotx_poll(canopen_handle_t *handle, uint8_t node_id,
									void *rx, uint32_t timeout_ms);

/* SDO abort transfer */
static lStatus_t canopen_sdo_abort(canopen_handle_t *handle, uint8_t node_id,
								   canopen_sdo_abort_t *tx);

/* SDO download initiate */
static lStatus_t canopen_sdo_download_init(canopen_handle_t *handle,
										   uint8_t node_id,
										   canopen_sdo_download_init_req_t *tx,
										   uint32_t timeout_ms);

/* SDO download segment */
static lStatus_t canopen_sdo_download_seg(canopen_handle_t *handle,
										  uint8_t node_id,
										  canopen_sdo_download_seg_req_t *tx,
										  uint32_t timeout_ms);

/* SDO upload initiate */
static lStatus_t canopen_sdo_upload_init(canopen_handle_t *handle,
										 uint8_t node_id,
										 canopen_sdo_upload_init_req_t *tx,
										 canopen_sdo_upload_init_resp_t *rx,
										 uint32_t timeout_ms);

/* SDO upload segment */
static lStatus_t canopen_sdo_upload_seg(canopen_handle_t *handle,
										uint8_t node_id,
										canopen_sdo_upload_seg_req_t *tx,
										canopen_sdo_upload_seg_resp_t *rx,
										uint32_t timeout_ms);

/*******************************************************************************
 * Code
 ******************************************************************************/

void canopen_init(canopen_handle_t *handle, can_hal_instance_t instance,
				  uint8_t node_id)
{
	if (node_id > CANOPEN_DEF_MAX_NODE_ID)
		return;

	handle->instance = instance;
	handle->node_id = node_id;
	handle->cb = NULL;
	handle->arg = NULL;

	HAL_CAN_IntCmd(handle->instance, CAN_HAL_INT_RI, lFunctionalState_Enable);

	HAL_CAN_EnableInterrupt(handle->instance, canopen_can_callback, handle);
}

void canopen_callback_config(canopen_handle_t *handle, canopen_callback_t cb,
							 void *arg)
{
	handle->cb = cb;
	handle->arg = arg;
}

lStatus_t canopen_nmt(canopen_handle_t *handle, uint8_t node_id,
					  canopen_nmt_cs_e cs)
{
	if (node_id > CANOPEN_DEF_MAX_NODE_ID)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};
	canopen_nmt_data_t req = {.cs = cs, .node_id = node_id};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_NMT;
	msg.length = CANOPEN_DEF_NMT_MSG_SIZE;
	memcpy(msg.data, &req, sizeof(req));

	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);

	return ret;
}

lStatus_t canopen_sync(canopen_handle_t *handle)
{
	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_SYNC;
	msg.length = CANOPEN_DEF_SYNC_MSG_SIZE;

	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);

	return ret;
}

lStatus_t canopen_pdo1rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size)
{
	return canopen_pdorx(handle, CANOPEN_COB_TYPE_PDO1_RX, node_id, data, size);
}

lStatus_t canopen_pdo2rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size)
{
	return canopen_pdorx(handle, CANOPEN_COB_TYPE_PDO2_RX, node_id, data, size);
}

lStatus_t canopen_pdo3rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size)
{
	return canopen_pdorx(handle, CANOPEN_COB_TYPE_PDO3_RX, node_id, data, size);
}

lStatus_t canopen_pdo4rx(canopen_handle_t *handle, uint8_t node_id, void *data,
						 uint8_t size)
{
	return canopen_pdorx(handle, CANOPEN_COB_TYPE_PDO4_RX, node_id, data, size);
}

lStatus_t canopen_sdo_download(canopen_handle_t *handle, uint8_t node_id,
							   const canopen_od_entry_t *mtdt, void *data,
							   uint32_t timeout_ms)
{
	if (node_id > CANOPEN_DEF_MAX_NODE_ID)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	canopen_sdo_download_init_req_t init_tran = {0};
	uint32_t tx_cnt = 0;
	uint8_t *data_ptr;

	data_ptr = (uint8_t *)data;

	/* SDO download initiate */
	if (mtdt->data_size <= CANOPEN_DEF_SDO_INIT_DATA_SIZE) {
		init_tran.flags.ccs = CANOPEN_DEF_SDO_CCS_DOWNLOAD_INIT_REQ;
		init_tran.flags.n = CANOPEN_DEF_SDO_INIT_DATA_SIZE - mtdt->data_size;
		init_tran.flags.e = 1;
		init_tran.flags.s = 1;
		init_tran.idx = mtdt->idx;
		init_tran.subidx = mtdt->subidx;
		memcpy(init_tran.data, data_ptr, mtdt->data_size);
	} else {
		init_tran.flags.ccs = CANOPEN_DEF_SDO_CCS_DOWNLOAD_INIT_REQ;
		init_tran.flags.n =
			CANOPEN_DEF_SDO_INIT_DATA_SIZE - sizeof(mtdt->data_size);
		init_tran.flags.e = 0;
		init_tran.flags.s = 1;
		init_tran.idx = mtdt->idx;
		init_tran.subidx = mtdt->subidx;
		memcpy(init_tran.data, &mtdt->data_size, sizeof(mtdt->data_size));
	}

	ret = canopen_sdo_download_init(handle, node_id, &init_tran, timeout_ms);
	if (ret != lStatus_Success)
		return ret;

	/* check if transfer is expedited, return status if it is */
	if (mtdt->data_size <= CANOPEN_DEF_SDO_INIT_DATA_SIZE)
		return lStatus_Success;

	/* handle data segments */
	uint8_t t = 0;

	do {
		uint8_t c = 0;
		uint8_t size = 0;

		/* check if this fragment is the last one  */
		c = (mtdt->data_size - tx_cnt <= CANOPEN_DEF_SDO_SEG_DATA_SIZE) ? 1 : 0;
		/* calculate fragment data size */
		size = c ? (mtdt->data_size - tx_cnt) : (CANOPEN_DEF_SDO_SEG_DATA_SIZE);

		/* SDO segment download */
		canopen_sdo_download_seg_req_t seg_req = {0};

		seg_req.flags.ccs = CANOPEN_DEF_SDO_CCS_DOWNLOAD_SEG_REQ;
		seg_req.flags.t = t;
		seg_req.flags.n = (CANOPEN_DEF_SDO_SEG_DATA_SIZE - size);
		seg_req.flags.c = c;
		memcpy(seg_req.data, data_ptr, size);

		ret = canopen_sdo_download_seg(handle, node_id, &seg_req, timeout_ms);
		if (ret != lStatus_Success)
			return ret;

		tx_cnt += size;
		data_ptr += size;
		t = !t;
	} while (tx_cnt < mtdt->data_size);

	if (tx_cnt < mtdt->data_size)
		return lStatus_Fail;

	return lStatus_Success;
}

lStatus_t canopen_sdo_upload(canopen_handle_t *handle, uint8_t node_id,
							 const canopen_od_entry_t *mtdt, void *data,
							 uint32_t timeout_ms)
{
	if (node_id > CANOPEN_DEF_MAX_NODE_ID)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	canopen_sdo_upload_init_req_t init_req = {0};
	canopen_sdo_upload_init_resp_t init_resp = {0};
	uint32_t rx_size = 0;
	uint32_t rx_cnt = 0;
	uint8_t data_size;
	uint8_t *data_ptr;

	data_ptr = (uint8_t *)data;

	/* SDO upload initiate */
	init_req.flags.ccs = CANOPEN_DEF_SDO_CCS_UPLOAD_INIT_REQ;
	init_req.idx = mtdt->idx;
	init_req.subidx = mtdt->subidx;

	ret = canopen_sdo_upload_init(handle, node_id, &init_req, &init_resp,
								  timeout_ms);
	if (ret != lStatus_Success)
		return ret;

	data_size = CANOPEN_DEF_SDO_INIT_DATA_SIZE - init_resp.flags.n;

	/* check if transfer is expedited */
	if (init_resp.flags.e) {
		/* if transfer is expedited, extract data and return success */
		memcpy(data_ptr, init_resp.data, data_size);

		return lStatus_Success;
	} else {
		/* if transfer is normal, extract data size */
		memcpy(&rx_size, init_resp.data, data_size);

		/* check if data size is valid */
		if (rx_size > mtdt->data_size) {
			/* SDO abort transfer */
			canopen_sdo_abort_t abort_req = {0};

			abort_req.flags.cs = CANOPEN_DEF_SDO_CS_ABORT_TRANSFER;
			abort_req.idx = mtdt->idx;
			abort_req.subidx = mtdt->subidx;
			abort_req.code = CANOPEN_SDO_ABORT_CODE_GENERAL_ERR;

			canopen_sdo_abort(handle, node_id, &abort_req);
			return lStatus_Fail;
		}
	}

	/* handle SDO upload segments */
	uint8_t t = 0;

	do {
		canopen_sdo_upload_seg_req_t seg_req = {0};
		canopen_sdo_upload_seg_resp_t seg_resp = {0};

		seg_req.flags.ccs = CANOPEN_DEF_SDO_CCS_UPLOAD_SEG_REQ;
		seg_req.flags.t = t;

		/* SDO upload segment */
		ret = canopen_sdo_upload_seg(handle, node_id, &seg_req, &seg_resp,
									 timeout_ms);
		if (ret != lStatus_Success)
			return ret;

		data_size = CANOPEN_DEF_SDO_SEG_DATA_SIZE - seg_resp.flags.n;

		/* check for overflow */
		if ((rx_cnt + data_size) > mtdt->data_size) {
			/* SDO abort transfer */
			canopen_sdo_abort_t abort_req = {0};

			abort_req.flags.cs = CANOPEN_DEF_SDO_CS_ABORT_TRANSFER;
			abort_req.idx = mtdt->idx;
			abort_req.subidx = mtdt->subidx;
			abort_req.code = CANOPEN_SDO_ABORT_CODE_GENERAL_ERR;

			canopen_sdo_abort(handle, node_id, &abort_req);

			return lStatus_Fail;
		}

		/* extract data and move data pointer */
		memcpy(data_ptr, seg_resp.data, data_size);
		rx_cnt += data_size;
		data_ptr += data_size;

		/* check if this is the last segment */
		if (seg_resp.flags.c)
			break;

		/* toggle bit */
		t = !t;
	} while (rx_cnt < mtdt->data_size);

	/* check receive size */
	if (rx_cnt < mtdt->data_size)
		return lStatus_Fail;

	return lStatus_Success;
}

lStatus_t canopen_heartbeat(canopen_handle_t *handle,
							canopen_nmt_slave_state_e state)
{
	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_NMT_ERR_CTRL | handle->node_id;
	msg.length = CANOPEN_DEF_ERROR_MSG_SIZE;
	msg.data[0] = state;

	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);

	return ret;
}

/******************************* User Callbacks *******************************/

/* CAN Callback, read incoming packet and notify upper layer. */
void canopen_can_callback(void *usr_arg, uint32_t intStatus)
{
	canopen_handle_t *handle;
	can_hal_msg_t msg;

	handle = (canopen_handle_t *)usr_arg;

	if (intStatus & CAN_HAL_INT_MASK_RI) {
		HAL_CAN_ReceiveMessage(handle->instance, &msg, 0);

		canopen_cob_type_e type;
		uint8_t node_id;

		type = msg.messageId & CANOPEN_DEF_FCN_CODE_MASK;
		node_id = msg.messageId & CANOPEN_DEF_NODE_ID_MASK;

		if (handle->cb != NULL)
			handle->cb(handle->arg, type, node_id, msg.data, msg.length);
	}
}

/****************************** static functions ******************************/

static lStatus_t canopen_pdorx(canopen_handle_t *handle,
							   canopen_cob_type_e cob_type, uint8_t node_id,
							   void *data, uint8_t size)
{
	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};

	if (node_id > CANOPEN_DEF_MAX_NODE_ID || size > sizeof(msg.data))
		return lStatus_Fail;

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = cob_type | node_id;
	msg.length = size;
	memcpy(msg.data, data, size);

	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);

	return ret;
}

static lStatus_t canopen_sdotx_poll(canopen_handle_t *handle, uint8_t node_id,
									void *rx, uint32_t timeout_ms)
{
	can_hal_msg_t msg = {0};
	lStatus_t ret = lStatus_Fail;
	uint64_t t0 = HAL_GetTimeUS();

	while (1) {
		/* receive message */
		ret = HAL_CAN_ReceiveMessage(handle->instance, &msg,
									 CANOPEN_CAN_TIMEOUT_US);

		/* check for timeout */
		if (HAL_GetTimeUS() - t0 >= timeout_ms * 1000)
			return lStatus_Timeout;

		if (ret != lStatus_Success)
			continue;

		/* check received type and node id */
		canopen_cob_type_e rx_type;
		uint8_t rx_node_id;

		rx_type = msg.messageId & CANOPEN_DEF_FCN_CODE_MASK;
		rx_node_id = msg.messageId & CANOPEN_DEF_NODE_ID_MASK;

		if (rx_type != CANOPEN_COB_TYPE_SDO_TX || rx_node_id != node_id)
			continue;

		memcpy(rx, msg.data, CANOPEN_DEF_SDO_MSG_SIZE);
		break;
	}

	return lStatus_Success;
}

static lStatus_t canopen_sdo_abort(canopen_handle_t *handle, uint8_t node_id,
								   canopen_sdo_abort_t *tx)
{
	if (tx->flags.cs != CANOPEN_DEF_SDO_CS_ABORT_TRANSFER)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_SDO_RX | node_id;
	msg.length = CANOPEN_DEF_SDO_MSG_SIZE;
	memcpy(msg.data, tx, sizeof(canopen_sdo_abort_t));

	/* send SDO abort */
	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);
	if (ret != lStatus_Success)
		return ret;

	return lStatus_Success;
}

static lStatus_t canopen_sdo_download_init(canopen_handle_t *handle,
										   uint8_t node_id,
										   canopen_sdo_download_init_req_t *tx,
										   uint32_t timeout_ms)
{
	if (tx->flags.ccs != CANOPEN_DEF_SDO_CCS_DOWNLOAD_INIT_REQ)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};
	canopen_sdo_download_init_resp_t sdo_resp = {0};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_SDO_RX | node_id;
	msg.length = CANOPEN_DEF_SDO_MSG_SIZE;
	memcpy(msg.data, tx, sizeof(canopen_sdo_download_init_req_t));

	/* disable interrupt */
	HAL_NVIC_DisableIRQ(CAN_IRQn);

	/* send SDO download initiate request */
	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);
	if (ret == lStatus_Success) {
		uint8_t sdo_data[CANOPEN_DEF_SDO_MSG_SIZE];

		/* poll SDO message from server */
		ret = canopen_sdotx_poll(handle, node_id, sdo_data, timeout_ms);
		if (ret == lStatus_Success) {
			memcpy(&sdo_resp, sdo_data,
				   sizeof(canopen_sdo_download_init_resp_t));

			/* check response */
			if (sdo_resp.flags.scs != CANOPEN_DEF_SDO_SCS_DOWNLOAD_INIT_RESP ||
				sdo_resp.idx != tx->idx || sdo_resp.subidx != tx->subidx)
				ret = lStatus_Fail;
		}
	}

	/* enable interrupt */
	HAL_NVIC_EnableIRQ(CAN_IRQn);

	return ret;
}

static lStatus_t canopen_sdo_download_seg(canopen_handle_t *handle,
										  uint8_t node_id,
										  canopen_sdo_download_seg_req_t *tx,
										  uint32_t timeout_ms)
{
	if (tx->flags.ccs != CANOPEN_DEF_SDO_CCS_DOWNLOAD_SEG_REQ)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};
	canopen_sdo_download_seg_resp_t seg_resp = {0};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_SDO_RX | node_id;
	msg.length = CANOPEN_DEF_SDO_MSG_SIZE;
	memcpy(msg.data, tx, sizeof(canopen_sdo_download_seg_req_t));

	/* disable interrupt */
	HAL_NVIC_DisableIRQ(CAN_IRQn);

	/* send SDO download segment request */
	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);
	if (ret == lStatus_Success) {
		uint8_t data[8];

		/* poll SDO message from server */
		ret = canopen_sdotx_poll(handle, node_id, data, timeout_ms);
		if (ret == lStatus_Success) {
			memcpy(&seg_resp, data, sizeof(canopen_sdo_download_seg_resp_t));

			/* check response */
			if (seg_resp.flags.scs != CANOPEN_DEF_SDO_SCS_DOWNLOAD_SEG_RESP ||
				seg_resp.flags.t != tx->flags.t)
				ret = lStatus_Fail;
		}
	}

	/* enable interrupt */
	HAL_NVIC_EnableIRQ(CAN_IRQn);

	return ret;
}

static lStatus_t canopen_sdo_upload_init(canopen_handle_t *handle,
										 uint8_t node_id,
										 canopen_sdo_upload_init_req_t *tx,
										 canopen_sdo_upload_init_resp_t *rx,
										 uint32_t timeout_ms)
{
	if (tx->flags.ccs != CANOPEN_DEF_SDO_CCS_UPLOAD_INIT_REQ)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_SDO_RX | node_id;
	msg.length = CANOPEN_DEF_SDO_MSG_SIZE;
	memcpy(msg.data, tx, sizeof(canopen_sdo_upload_init_req_t));

	/* disable interrupt */
	HAL_NVIC_DisableIRQ(CAN_IRQn);

	/* send SDO upload initiate request */
	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);
	if (ret == lStatus_Success) {
		uint8_t sdo_data[CANOPEN_DEF_SDO_MSG_SIZE];

		/* poll SDO message from server */
		ret = canopen_sdotx_poll(handle, node_id, sdo_data, timeout_ms);
		if (ret == lStatus_Success) {
			memcpy(rx, sdo_data, sizeof(canopen_sdo_upload_init_resp_t));

			/* check response */
			if (rx->idx != tx->idx || rx->subidx != tx->subidx ||
				rx->flags.scs != CANOPEN_DEF_SDO_SCS_UPLOAD_INIT_RESP)
				ret = lStatus_Fail;

			/* check if data size is indicated */
			if (!rx->flags.s)
				ret = lStatus_Fail;
		}
	}

	/* enable interrupt */
	HAL_NVIC_EnableIRQ(CAN_IRQn);

	return ret;
}

static lStatus_t canopen_sdo_upload_seg(canopen_handle_t *handle,
										uint8_t node_id,
										canopen_sdo_upload_seg_req_t *tx,
										canopen_sdo_upload_seg_resp_t *rx,
										uint32_t timeout_ms)
{
	if (tx->flags.ccs != CANOPEN_DEF_SDO_CCS_UPLOAD_SEG_REQ)
		return lStatus_Fail;

	lStatus_t ret = lStatus_Fail;
	can_hal_msg_t msg = {0};

	msg.idFormat = CAN_HAL_ID_FORMAT_STD;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.messageId = CANOPEN_COB_TYPE_SDO_RX | node_id;
	msg.length = CANOPEN_DEF_SDO_MSG_SIZE;
	memcpy(msg.data, tx, sizeof(canopen_sdo_upload_seg_req_t));

	/* disable interrupt */
	HAL_NVIC_DisableIRQ(CAN_IRQn);

	/* send SDO upload segment request */
	ret = HAL_CAN_SendMessage(handle->instance, &msg, CANOPEN_CAN_TIMEOUT_US);
	if (ret == lStatus_Success) {
		uint8_t data[8];

		/* poll SDO message from server */
		ret = canopen_sdotx_poll(handle, node_id, data, timeout_ms);
		if (ret == lStatus_Success) {
			memcpy(rx, data, sizeof(canopen_sdo_upload_seg_resp_t));

			/* check response */
			if (rx->flags.scs != CANOPEN_DEF_SDO_SCS_UPLOAD_SEG_RESP ||
				rx->flags.t != tx->flags.t)
				ret = lStatus_Fail;
		}
	}

	/* enable interrupt */
	HAL_NVIC_EnableIRQ(CAN_IRQn);

	return ret;
}

/********************************* End Of File ********************************/