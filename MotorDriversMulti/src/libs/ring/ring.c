/**
 * @file    ring.c
 * @brief   Ring buffer library.
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>

#include "ring.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void ring_init(ring_t *ring, uint8_t *buffer, uint32_t size, uint32_t block,
			   uint32_t flags)
{
	ring->buffer = buffer;
	ring->size = size;

	ring->block = block;
	ring->flags = flags;

	ring->head = 0;
	ring->tail = 0;
}

uint32_t ring_used(ring_t *ring)
{
	if (ring->head >= ring->tail)
		return ring->head - ring->tail;
	else
		return ring->size - (ring->tail - ring->head);
}

uint32_t ring_free(ring_t *ring) { return ring->size - 1 - ring_used(ring); }

uint32_t ring_push(ring_t *ring, const uint8_t *data, uint32_t count)
{
	uint32_t free;

	/* force push in blocks of specified size if RING_FLAG_BLOCK is set */
	if (ring->flags & RING_FLAG_BLOCK && count % ring->block != 0)
		return 0;

	free = ring_free(ring);
	if (free < count) {
		/* partial push is not allowed */
		if (!(ring->flags & RING_FLAG_PART_PUSH))
			return 0;

		/* partial push is allowed */
		if (ring->flags & RING_FLAG_BLOCK)
			count = (free / ring->block) * ring->block;
		else
			count = free;
	}

	/* write data in one or two linear parts */
	if (count <= ring->size - ring->head) {
		memcpy(&ring->buffer[ring->head], &data[0], count);
	} else {
		memcpy(&ring->buffer[ring->head], &data[0], ring->size - ring->head);
		memcpy(&ring->buffer[0], &data[ring->size - ring->head],
			   count - (ring->size - ring->head));
	}

	/* prevent compiler reordering */
	__asm__ __volatile__("" ::: "memory");

	/* update index */
	ring->head = (ring->head + count) % ring->size;

	/* return number of pushed bytes */
	return count;
}

uint32_t ring_pop(ring_t *ring, uint8_t *data, uint32_t count)
{
	uint32_t used;

	/* force pop in blocks of specified size if RING_FLAG_BLOCK is set */
	if (ring->flags & RING_FLAG_BLOCK && count % ring->block != 0)
		return 0;

	used = ring_used(ring);
	if (used < count) {
		/* partial pop is not allowed */
		if (!(ring->flags & RING_FLAG_PART_POP))
			return 0;

		/* partial pop is allowed */
		if (ring->flags & RING_FLAG_BLOCK)
			count = (used / ring->block) * ring->block;
		else
			count = used;
	}

	/* read data from one or two linear parts */
	if (count <= ring->size - ring->tail) {
		memcpy(&data[0], &ring->buffer[ring->tail], count);
	} else {
		memcpy(&data[0], &ring->buffer[ring->tail], ring->size - ring->tail);
		memcpy(&data[ring->size - ring->tail], &ring->buffer[0],
			   count - (ring->size - ring->tail));
	}

	/* prevent compiler reordering */
	__asm__ __volatile__("" ::: "memory");

	/* update index */
	ring->tail = (ring->tail + count) % ring->size;

	/* return number of poped bytes */
	return count;
}

uint32_t ring_peek(ring_t *ring, uint8_t *data, uint32_t count)
{
	uint32_t used;

	/* force peek in blocks of specified size if RING_FLAG_BLOCK is set */
	if (ring->flags & RING_FLAG_BLOCK && count % ring->block != 0)
		return 0;

	used = ring_used(ring);
	if (used < count) {
		/* partial peek is not allowed */
		if (!(ring->flags & RING_FLAG_PART_POP))
			return 0;

		/* partial peek is allowed */
		if (ring->flags & RING_FLAG_BLOCK)
			count = (used / ring->block) * ring->block;
		else
			count = used;
	}

	/* read data from one or two linear parts */
	if (count <= ring->size - ring->tail) {
		memcpy(&data[0], &ring->buffer[ring->tail], count);
	} else {
		memcpy(&data[0], &ring->buffer[ring->tail], ring->size - ring->tail);
		memcpy(&data[ring->size - ring->tail], &ring->buffer[0],
			   count - (ring->size - ring->tail));
	}

	/* return number of peeked bytes */
	return count;
}

uint32_t ring_skip(ring_t *ring, uint32_t count)
{
	uint32_t used;

	/* force skip in blocks of specified size if RING_FLAG_BLOCK is set */
	if (ring->flags & RING_FLAG_BLOCK && count % ring->block != 0)
		return 0;

	used = ring_used(ring);
	if (used < count) {
		/* partial peek is not allowed */
		if (!(ring->flags & RING_FLAG_PART_POP))
			return 0;

		/* partial peek is allowed */
		if (ring->flags & RING_FLAG_BLOCK)
			count = (used / ring->block) * ring->block;
		else
			count = used;
	}

	/* prevent compiler reordering */
	__asm__ __volatile__("" ::: "memory");

	/* update index */
	ring->tail = (ring->tail + count) % ring->size;

	/* return number of skiped bytes */
	return count;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/