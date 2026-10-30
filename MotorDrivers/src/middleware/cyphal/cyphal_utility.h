/**
 * @file    cyphal_utility.h
 * @brief   Cyphal/libcanard utility API: deterministic memory, TX/RX helpers
 * @version 1.0.0
 * @date    20.11.2025
 *
 * High-level helpers for using libcanard in hard real-time:
 * - Deterministic, fixed-size block allocators for libcanard instance and TX
 * queue
 * - Generic publish helpers for messages and services
 * - Bounded TX queue drain helpers (frame- or time-limited)
 * - RX polling helper that invokes a user callback per accepted transfer
 *
 * All functions are designed to be allocation-free at runtime except the
 * deterministic pools set up during initialization.
 */

#ifndef CYPHAL_UTILITY_H
#define CYPHAL_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "canard.h"
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * Configurable pool parameters (override before including if needed).
 * Choose sizes based on the largest expected allocation performed by
 * libcanard on your workload and the maximum amount of concurrent work.
 */
#ifndef CYPAHL_INSTANCE_BLOCK_SIZE
/** Size in bytes of a single instance allocator block. */
/* Must be >= maksimalnom RX extent-u (npr. GetInfo ~313B, FTP ~251B). */
#define CYPAHL_INSTANCE_BLOCK_SIZE 380
#endif
#ifndef CYPAHL_INSTANCE_BLOCK_CAPACITY
/** Number of blocks available to the instance allocator. */
#define CYPAHL_INSTANCE_BLOCK_CAPACITY 32
#endif
#ifndef CYPAHL_TXQ_BLOCK_SIZE
/** Size in bytes of a single TX-queue allocator block (CAN MTU payload). */
#define CYPAHL_TXQ_BLOCK_SIZE 64
#endif
#ifndef CYPAHL_TXQ_BLOCK_CAPACITY
/** Number of blocks available to the TX-queue allocator. */
#define CYPAHL_TXQ_BLOCK_CAPACITY 64
#endif

/** Number of node IDs stored in `cyphal_nodes_ids_t`. */
#define CYPHAL_NODES_IDX_MAX 7

#define CYPHAL_INS_ID 9
#define CYPHAL_BLACK_BOX_ID 20
#define CYPHAL_GW_GND_ID 6
#define CYPHAL_GW_SKY_ID 8
#define CYPHAL_POWER_MANAGEMENT_ID 12
#define CYPHAL_ACTUATOR_MASTER_ID 17
#define CYPHAL_MOTOR_DRIVERS_ID 15

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/** Global memory resources for libcanard core and TX queue. */
extern const struct CanardMemoryResource cyphal_instance_memory;
extern const struct CanardMemoryResource cyphal_txq_memory;

/**
 * Small container for whitelisting source node IDs on RX.
 * When passed to `cyphal_process()`, only transfers whose
 * `remote_node_id` matches one of the entries will be delivered.
 */
typedef struct {
	uint8_t uavcan_node_id[CYPHAL_NODES_IDX_MAX];
} cyphal_nodes_ids_t;

/** Default node-ID list provided by the implementation (local + one remote).
 *  Applications may ignore this and provide their own list.
 */
extern const uint8_t CYPHAL_NODES_ID_ARRAY[CYPHAL_NODES_IDX_MAX];

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/** @internal libcanard instance allocator (fixed-size blocks). */
static void* instance_allocate(void* const user_reference, const size_t size);
/** @internal libcanard instance deallocator. */
static void instance_deallocate(void* const user_reference, const size_t size, void* const pointer);
/** @internal libcanard TX-queue allocator (fixed-size blocks). */
static void* txq_allocate(void* const user_reference, const size_t size);
/** @internal libcanard TX-queue deallocator. */
static void txq_deallocate(void* const user_reference, const size_t size, void* const pointer);

/**
 * Initialize deterministic memory pools for libcanard (instance + TX queue).
 * Must be called before `canardInit()` and `canardTxInit()`.
 */
void cyphal_pool_init(void);

/**
 * Publish a subject (message) transfer.
 *
 * - Manages `transfer_id` (increments modulo 32 when a non-NULL pointer is
 * provided; otherwise a static fallback).
 * - Uses `timeout_usec` to set the canard deadline.
 *
 * @param ins         Libcanard instance.
 * @param txq         TX queue.
 * @param subject_id  Subject (port) ID.
 * @param priority    Transfer priority.
 * @param payload     Pointer to serialized payload buffer.
 * @param payload_size Payload size in bytes.
 * @param tid         Optional pointer to per-subject transfer-ID state.
 * @param timeout_usec Relative deadline for libcanard in microseconds.
 * @return >=0 on success (number of frames enqueued), negative on error.
 */
int cyphal_pub_message(struct CanardInstance* ins, struct CanardTxQueue* txq,
					   CanardPortID subject_id, enum CanardPriority priority, const void* payload,
					   size_t payload_size, CanardTransferID* tid, CanardMicrosecond timeout_usec);

/**
 * Publish a subject (message) transfer with an explicit remote node ID.
 * NOTE: Cyphal messages are broadcast; setting remote_node_id is non-standard
 * and may be ignored by other stacks. Provided to support targeted filtering
 * in mixed systems.
 */
int cyphal_pub_message_to(struct CanardInstance* ins, struct CanardTxQueue* txq,
						  CanardPortID subject_id, enum CanardPriority priority,
						  CanardNodeID dst_node_id, const void* payload, size_t payload_size,
						  CanardTransferID* tid, CanardMicrosecond timeout_usec);

/**
 * Publish a service request/response.
 *
 * @param ins          Libcanard instance.
 * @param txq          TX queue.
 * @param kind         CanardTransferKindRequest or CanardTransferKindResponse.
 * @param service_id   Service (port) ID.
 * @param dst_node_id  Destination node-ID (0..127).
 * @param priority     Transfer priority.
 * @param payload      Pointer to serialized payload buffer.
 * @param payload_size Payload size in bytes.
 * @param tid          Optional pointer to per-service transfer-ID state.
 * @param timeout_usec Relative deadline in microseconds.
 * @return >=0 on success (frames enqueued), negative on error.
 */
int cyphal_pub_service(struct CanardInstance* ins, struct CanardTxQueue* txq,
					   enum CanardTransferKind kind, CanardPortID service_id,
					   CanardNodeID dst_node_id, enum CanardPriority priority, const void* payload,
					   size_t payload_size, CanardTransferID* tid, CanardMicrosecond timeout_usec);

/**
 * Flush pending frames from the TX queue using a user-provided frame handler.
 *
 * @param ins        Libcanard instance.
 * @param txq        TX queue.
 * @param user_reference Opaque pointer passed to `on_frame`.
 * @param on_frame   Callback invoked per frame; should return 1 when sent, 0 to
 * retry later.
 * @param max_frames Maximum number of frames to attempt.
 * @param max_flush_time_usec Optional soft time budget (0 to ignore).
 * @return Number of frames handled, or negative on error.
 */
int cyphal_flush_tx(struct CanardInstance* ins, struct CanardTxQueue* txq, void* user_reference,
					int8_t (*on_frame)(void*, CanardMicrosecond, struct CanardMutableFrame*),
					uint32_t max_frames, CanardMicrosecond max_flush_time_usec);

/**
 * Publish then flush frames immediately (bounded by frames/time).
 * Combines `cyphal_pub_message()` and `cyphal_flush_tx()`.
 *
 * @return Number of frames sent, or negative on error.
 */
int cyphal_pub_and_flush(struct CanardInstance* ins, struct CanardTxQueue* txq,
						 CanardPortID subject_id, enum CanardPriority priority, const void* payload,
						 size_t payload_size, CanardTransferID* tid, CanardMicrosecond timeout_usec,
						 void* user_reference,
						 int8_t (*on_frame)(void*, CanardMicrosecond, struct CanardMutableFrame*),
						 uint32_t max_frames, CanardMicrosecond max_flush_time_usec);

/** Default CAN frame TX handler using HAL (extended ID). */
int8_t cyphal_default_tx_handler(void* user_ref, CanardMicrosecond deadline_usec,
								 struct CanardMutableFrame* frame);

/**
 * Drain a bounded number of frames from the TX queue using the default HAL
 * handler.
 *
 * @param max_frames Upper limit on frames processed this call.
 * @return Number of frames sent, negative on error.
 */
int cyphal_drain_tx_queue(struct CanardInstance* ins, struct CanardTxQueue* txq,
						  uint32_t max_frames);

/**
 * RX process: poll CAN, feed libcanard, and invoke a user callback for each
 * accepted, fully reassembled transfer.
 *
 * Filtering: if `nodes_ids` is non-NULL, only transfers whose
 * `remote_node_id` matches one of the entries are delivered; others are
 * dropped. If NULL, all transfers for which there is an active subscription are
 * delivered.
 *
 * @param ins         Libcanard instance.
 * @param on_transfer User callback invoked per accepted transfer (non-NULL
 * recommended).
 * @param max_frames  Maximum number of CAN frames to consume from the HAL in
 * this call.
 * @param nodes_ids   Optional whitelist of allowed source node IDs.
 * @return Number of transfers delivered to the callback, negative on error.
 */
int cyphal_process(struct CanardInstance* ins,
				   void (*on_transfer)(const struct CanardRxTransfer* tr), uint32_t max_frames,
				   cyphal_nodes_ids_t* nodes_ids);

#ifdef __cplusplus
}
#endif

#endif /* CYPHAL_UTILITY_H */