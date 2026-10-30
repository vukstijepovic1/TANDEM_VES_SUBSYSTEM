/**
 * @file    cyphal_utility.c
 * @brief   Cyphal utility functions and definitions
 * @version 1.0.0
 * @date    20.11.2025
 * @author  BetaTehPro
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "cyphal_utility.h"
#include "can.h"
#include "timer.h"



#include <stdbool.h>
#include <string.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

const struct CanardMemoryResource cyphal_instance_memory = {
	.user_reference = NULL,
	.deallocate = instance_deallocate,
	.allocate = instance_allocate,
};

const struct CanardMemoryResource cyphal_txq_memory = {
	.user_reference = NULL,
	.deallocate = txq_deallocate,
	.allocate = txq_allocate,
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Default list of known nodes (referenced via extern in header) */
const uint8_t CYPHAL_NODES_ID_ARRAY[CYPHAL_NODES_IDX_MAX] = {
	CYPHAL_INS_ID,	  CYPHAL_BLACK_BOX_ID,		  CYPHAL_GW_GND_ID,
	CYPHAL_GW_SKY_ID, CYPHAL_POWER_MANAGEMENT_ID, CYPHAL_ACTUATOR_MASTER_ID};

/* Instance memory: used by libcanard for TX items, RX payloads, RX sessions */
static __attribute__((section("AHBSRAM0"), aligned(8))) uint8_t
	cyphal_instance_pool[CYPAHL_INSTANCE_BLOCK_CAPACITY][CYPAHL_INSTANCE_BLOCK_SIZE];

static uint16_t cyphal_instance_free[CYPAHL_INSTANCE_BLOCK_CAPACITY];
static int16_t cyphal_instance_top = -1;

/* TX queue payload memory: used by canardTxQueue for frame payloads (<=8) */
static __attribute__((section("AHBSRAM0"), aligned(8))) uint8_t
	cyphal_txq_pool[CYPAHL_TXQ_BLOCK_CAPACITY][CYPAHL_TXQ_BLOCK_SIZE];

static uint16_t cyphal_txq_free[CYPAHL_TXQ_BLOCK_CAPACITY];
static int16_t cyphal_txq_top = -1;

/*******************************************************************************
 * Code
 ******************************************************************************/

void cyphal_pool_init(void)
{
	/* Fill free stacks LIFO */
	cyphal_instance_top = -1;

	for (uint16_t i = 0; i < CYPAHL_INSTANCE_BLOCK_CAPACITY; i++) {
		cyphal_instance_free[i] = i;
	}

	cyphal_instance_top = CYPAHL_INSTANCE_BLOCK_CAPACITY - 1;

	cyphal_txq_top = -1;

	for (uint16_t i = 0; i < CYPAHL_TXQ_BLOCK_CAPACITY; i++) {
		cyphal_txq_free[i] = i;
	}

	cyphal_txq_top = CYPAHL_TXQ_BLOCK_CAPACITY - 1;
}

static void* instance_allocate(void* const user_reference, const size_t size)
{
	(void)user_reference;
	if (size > CYPAHL_INSTANCE_BLOCK_SIZE) {
		return NULL;
	}
	if (cyphal_instance_top < 0) {
		return NULL;
	}
	uint16_t idx = (uint16_t)cyphal_instance_free[cyphal_instance_top--];
	return (void*)cyphal_instance_pool[idx];
}

static void instance_deallocate(void* const user_reference, const size_t size, void* const pointer)
{
	(void)user_reference;
	(void)size;
	if (pointer == NULL)
		return;
	uintptr_t base = (uintptr_t)&cyphal_instance_pool[0][0];
	uintptr_t end = (uintptr_t)&cyphal_instance_pool[0][0] +
					(uintptr_t)(CYPAHL_INSTANCE_BLOCK_CAPACITY * CYPAHL_INSTANCE_BLOCK_SIZE);
	uintptr_t p = (uintptr_t)pointer;
	if (p < base || p >= end) {
		return; /* Not ours */
	}
	uint32_t offset = (uint32_t)(p - base);
	uint16_t idx = (uint16_t)(offset / CYPAHL_INSTANCE_BLOCK_SIZE);
	if (cyphal_instance_top < (int16_t)(CYPAHL_INSTANCE_BLOCK_CAPACITY - 1)) {
		cyphal_instance_free[++cyphal_instance_top] = idx;
	}
}

static void* txq_allocate(void* const user_reference, const size_t size)
{
	(void)user_reference;

	if (size > CYPAHL_TXQ_BLOCK_SIZE) {
		return NULL;
	}
	if (cyphal_txq_top < 0) {
		return NULL;
	}
	uint16_t idx = (uint16_t)cyphal_txq_free[cyphal_txq_top--];
	return (void*)cyphal_txq_pool[idx];
}

static void txq_deallocate(void* const user_reference, const size_t size, void* const pointer)
{
	(void)user_reference;
	(void)size;
	if (pointer == NULL)
		return;
	uintptr_t base = (uintptr_t)&cyphal_txq_pool[0][0];
	uintptr_t end = (uintptr_t)&cyphal_txq_pool[0][0] +
					(uintptr_t)(CYPAHL_TXQ_BLOCK_CAPACITY * CYPAHL_TXQ_BLOCK_SIZE);
	uintptr_t p = (uintptr_t)pointer;
	if (p < base || p >= end) {
		return; /* Not ours */
	}
	uint32_t offset = (uint32_t)(p - base);
	uint16_t idx = (uint16_t)(offset / CYPAHL_TXQ_BLOCK_SIZE);
	if (cyphal_txq_top < (int16_t)(CYPAHL_TXQ_BLOCK_CAPACITY - 1)) {
		cyphal_txq_free[++cyphal_txq_top] = idx;
	}
}

/* --- TX helper functions --- */

int8_t cyphal_default_tx_handler(void* user_ref, CanardMicrosecond deadline_usec,
								 struct CanardMutableFrame* frame)
{
	(void)user_ref;
	(void)deadline_usec;
	can_hal_msg_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.messageId = frame->extended_can_id;
	msg.idFormat = CAN_HAL_ID_FORMAT_EXT;
	msg.frameType = CAN_HAL_DATA_FRAME;
	msg.length = (uint8_t)frame->payload.size;
	memcpy(msg.data, frame->payload.data, msg.length);
	const lStatus_t st = HAL_CAN_SendMessage(CAN_HAL_INSTANCE_0, &msg, 0);
	if (st == lStatus_Success) {
		return 1; /* transmitted */
	}
	return 0; /* retry later */
}

int cyphal_drain_tx_queue(struct CanardInstance* ins, struct CanardTxQueue* txq,
						  uint32_t max_frames)
{
	if ((ins == NULL) || (txq == NULL)) {
		return -CANARD_ERROR_INVALID_ARGUMENT;
	}

	uint32_t sent = 0;

	while (sent < max_frames) {

		const CanardMicrosecond now = (CanardMicrosecond)HAL_GetTimeUS();

		const int8_t r = canardTxPoll(txq, ins, now, NULL, cyphal_default_tx_handler, NULL, NULL);

		if (r <= 0)
			break;

		sent += (uint32_t)r;
	}

	return (int)sent;
}

int cyphal_process(struct CanardInstance* ins,
				   void (*on_transfer)(const struct CanardRxTransfer* tr), uint32_t max_frames,
				   cyphal_nodes_ids_t* nodes_ids)
{
	if (ins == NULL) {
		return -CANARD_ERROR_INVALID_ARGUMENT;
	}

	uint32_t delivered = 0;
	uint32_t processed_frames = 0;

	while (processed_frames < max_frames) {
		can_hal_msg_t rx;

		/* Ako nema vise poruka u CAN FIFO – izlazi */
		if (HAL_CAN_ReceiveMessage(CAN_HAL_INSTANCE_0, &rx, 0) != lStatus_Success) {
			break;
		}

		processed_frames++;

		/* Samo extended CAN ID */
		if (rx.idFormat != CAN_HAL_ID_FORMAT_EXT) {
			continue;
		}

		struct CanardFrame fr = {.extended_can_id = rx.messageId,
								 .payload = {.size = rx.length, .data = rx.data}};

		const CanardMicrosecond ts = (CanardMicrosecond)HAL_GetTimeUS();

		struct CanardRxTransfer tr = {0};
		struct CanardRxSubscription* sub = NULL;

		const int8_t acc = canardRxAccept(ins, ts, &fr, 0, &tr, &sub);

		if (acc != 1) {
			continue;
		}

		/* Provera da li je node ID dozvoljen */
		bool allowed = false;
		if (nodes_ids != NULL) {
			for (uint8_t i = 0; i < CYPHAL_NODES_IDX_MAX; i++) {
				if (tr.metadata.remote_node_id == nodes_ids->uavcan_node_id[i]) {
					allowed = true;
					break;
				}
			}
		}

		if (!allowed) {
			ins->memory.deallocate(ins->memory.user_reference, tr.payload.allocated_size,
								   tr.payload.data);
			continue;
		}

		/* Preuzmi transfer */
		if (on_transfer) {
			on_transfer(&tr);
		}

		/* Oslobodi payload */
		ins->memory.deallocate(ins->memory.user_reference, tr.payload.allocated_size,
							   tr.payload.data);

		delivered++;
	}

	return (int)delivered;
}

int cyphal_pub_message(struct CanardInstance* ins, struct CanardTxQueue* txq,
					   CanardPortID subject_id, enum CanardPriority priority, const void* payload,
					   size_t payload_size, CanardTransferID* tid, CanardMicrosecond timeout_usec)
{
	if ((ins == NULL) || (txq == NULL))
		return -CANARD_ERROR_INVALID_ARGUMENT;
	if ((payload == NULL) && (payload_size != 0))
		return -CANARD_ERROR_INVALID_ARGUMENT;

	CanardTransferID id;
	static CanardTransferID fallback_tid = 0;
	if (tid != NULL) {
		id = *tid;
		*tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	} else {
		id = fallback_tid;
		fallback_tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	}

	const CanardMicrosecond now = (CanardMicrosecond)HAL_GetTimeUS();
	const CanardMicrosecond deadline = now + timeout_usec;

	const struct CanardTransferMetadata meta = {
		.priority = priority,
		.transfer_kind = CanardTransferKindMessage,
		.port_id = subject_id,
		.remote_node_id = CANARD_NODE_ID_UNSET,
		.transfer_id = id,
	};
	const struct CanardPayload pl = {.size = payload_size, .data = (void*)payload};
	return canardTxPush(txq, ins, deadline, &meta, pl, now, NULL);
}

int cyphal_pub_messages(struct CanardInstance* ins, struct CanardTxQueue* txq,
						CanardPortID subject_id, enum CanardPriority priority, const void* payload,
						size_t payload_size, CanardTransferID* tid, CanardMicrosecond timeout_usec)
{
	if ((ins == NULL) || (txq == NULL))
		return -CANARD_ERROR_INVALID_ARGUMENT;
	if ((payload == NULL) && (payload_size != 0))
		return -CANARD_ERROR_INVALID_ARGUMENT;

	CanardTransferID id;
	static CanardTransferID fallback_tid = 0;
	if (tid != NULL) {
		id = *tid;
		*tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	} else {
		id = fallback_tid;
		fallback_tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	}

	const CanardMicrosecond now = (CanardMicrosecond)HAL_GetTimeUS();
	const CanardMicrosecond deadline = now + timeout_usec;

	const struct CanardTransferMetadata meta = {
		.priority = priority,
		.transfer_kind = CanardTransferKindMessage,
		.port_id = subject_id,
		.remote_node_id = CANARD_NODE_ID_UNSET,
		.transfer_id = id,
	};
	const struct CanardPayload pl = {.size = payload_size, .data = (void*)payload};
	return canardTxPush(txq, ins, deadline, &meta, pl, now, NULL);
}

int cyphal_pub_message_to(struct CanardInstance* ins, struct CanardTxQueue* txq,
						  CanardPortID subject_id, enum CanardPriority priority,
						  CanardNodeID dst_node_id, const void* payload, size_t payload_size,
						  CanardTransferID* tid, CanardMicrosecond timeout_usec)
{
	if ((ins == NULL) || (txq == NULL))
		return -CANARD_ERROR_INVALID_ARGUMENT;
	if ((payload == NULL) && (payload_size != 0))
		return -CANARD_ERROR_INVALID_ARGUMENT;
	if (dst_node_id > CANARD_NODE_ID_MAX)
		return -CANARD_ERROR_INVALID_ARGUMENT;

	CanardTransferID id;
	static CanardTransferID fallback_tid = 0;
	if (tid != NULL) {
		id = *tid;
		*tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	} else {
		id = fallback_tid;
		fallback_tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	}

	const CanardMicrosecond now = (CanardMicrosecond)HAL_GetTimeUS();
	const CanardMicrosecond deadline = now + timeout_usec;

	const struct CanardTransferMetadata meta = {
		.priority = priority,
		.transfer_kind = CanardTransferKindRequest,
		.port_id = subject_id,
		.remote_node_id = dst_node_id,
		.transfer_id = id,
	};
	const struct CanardPayload pl = {.size = payload_size, .data = (void*)payload};
	return canardTxPush(txq, ins, deadline, &meta, pl, now, NULL);
}

int cyphal_pub_service(struct CanardInstance* ins, struct CanardTxQueue* txq,
					   enum CanardTransferKind kind, CanardPortID service_id,
					   CanardNodeID dst_node_id, enum CanardPriority priority, const void* payload,
					   size_t payload_size, CanardTransferID* tid, CanardMicrosecond timeout_usec)
{
	if ((ins == NULL) || (txq == NULL))
		return -CANARD_ERROR_INVALID_ARGUMENT;
	if (!((kind == CanardTransferKindRequest) || (kind == CanardTransferKindResponse)))
		return -CANARD_ERROR_INVALID_ARGUMENT;
	if (dst_node_id > CANARD_NODE_ID_MAX)
		return -CANARD_ERROR_INVALID_ARGUMENT;
	if ((payload == NULL) && (payload_size != 0))
		return -CANARD_ERROR_INVALID_ARGUMENT;

	CanardTransferID id;
	static CanardTransferID fallback_tid = 0;
	if (tid != NULL) {
		id = *tid;
		*tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	} else {
		id = fallback_tid;
		fallback_tid = (CanardTransferID)((id + 1U) & CANARD_TRANSFER_ID_MAX);
	}

	const CanardMicrosecond now = (CanardMicrosecond)HAL_GetTimeUS();
	const CanardMicrosecond deadline = now + timeout_usec;

	const struct CanardTransferMetadata meta = {
		.priority = priority,
		.transfer_kind = kind,
		.port_id = service_id,
		.remote_node_id = dst_node_id,
		.transfer_id = id,
	};
	const struct CanardPayload pl = {.size = payload_size, .data = (void*)payload};
	return canardTxPush(txq, ins, deadline, &meta, pl, now, NULL);
}
