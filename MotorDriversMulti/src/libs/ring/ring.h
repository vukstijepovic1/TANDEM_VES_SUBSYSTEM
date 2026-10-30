/**
 * @file    ring.h
 * @brief   Ring buffer library.
 * @author  LisumLab
 */

#ifndef RING_H
#define RING_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief Ring buffer config flags.*/
typedef enum ring_flag_t {
	/* enforce minimal block size for all ring buffer operations */
	RING_FLAG_BLOCK = 0x00000001,
	/* allow to partially push/pop data to/from ring buffer */
	RING_FLAG_PART_PUSH = 0x0000002,
	RING_FLAG_PART_POP = 0x00000004
} ring_flag_t;

/*! @brief Ring buffer. */
typedef struct ring_t {
	uint8_t *buffer; // Pointer to buffer allocated for ring buffer.
	uint32_t size;	 // Ring buffer size in bytes.

	uint32_t block; // Ring Buffer Block Size.
	uint32_t flags; // Ring Buffer Flags.

	volatile uint32_t head; // Ring Buffer Head.
	volatile uint32_t tail; // Ring Buffer Tail.
} ring_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize ring buffer.
 *
 * @param[in] ring      Ring buffer.
 * @param[in] buffer    Array which will be used as ring buffer.
 * @param[in] size      Array size.
 * @param[in] block     Data block size.
 *                      @note   Operations (push, pop, peek etc.) are over
 *                              multiples of block size.
 * @param[in] flags Ring buffer flags (ring_flag_t)
 * @return None.
 */
void ring_init(ring_t *ring, uint8_t *buffer, uint32_t size, uint32_t block,
			   uint32_t flags);

/**
 * @brief Retrieve number of used bytes in ring buffer.
 *
 * @param[in] ring Ring buffer.
 * @return Number of used bytes in ring buffer
 */
uint32_t ring_used(ring_t *ring);

/**
 * @brief Retrieve number of free bytes in ring buffer.
 *
 * @param[in] ring  Ring buffer.
 * @return Number of free bytes in ring buffer
 */
uint32_t ring_free(ring_t *ring);

/**
 * @brief Write data to ring buffer.
 *
 * @param[in] ring  Ring buffer.
 * @param[in] data  Write data.
 * @param[in] count Number of data blocks.
 *                  Number of write bytes = count * block_size
 * @return Number written bytes.
 */
uint32_t ring_push(ring_t *ring, const uint8_t *data, uint32_t count);

/**
 * @brief Read data from ring buffer.
 *
 * @param[in] ring Ring buffer.
 * @param[in] data Read data.
 * @param[in] count Number of data blocks.
 *                  Number of read bytes = count * block_size
 * @return Number read bytes.
 */
uint32_t ring_pop(ring_t *ring, uint8_t *data, uint32_t count);

/**
 * @brief Read data from ring buffer without updating index.
 *
 * @param[in] ring  Ring buffer.
 * @param[in] data  Read data.
 * @param[in] count Number of data blocks.
 *                  Number of read bytes = count * block_size
 * @return Number read bytes.
 */
uint32_t ring_peek(ring_t *ring, uint8_t *data, uint32_t count);

/**
 * @brief Read data from ring buffer without copying data.
 *
 * @param[in] ring  Ring buffer.
 * @param[in] count Number of data blocks.
 *                  Number of skipped bytes = count * block_size
 * @return Number of skipped bytes.
 */
uint32_t ring_skip(ring_t *ring, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* RING_H */