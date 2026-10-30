/**
 * @file    xbus.c
 * @brief   XBUS parser.
 * @date    12.09.2024.
 * @version 1.0.0
 *
 * @details Full support for XBUS protocol isn't implemented, this version
 *          of parser focuses on MTData2 message which contains sensor data.
 *
 *          MTData2 message contains multiple data packets which user can
 *          track via subscription. Device MTData2 Data Packet output
 *          configuration is done using MT Manager Application, and isn't
 *          possible using this library for now.
 *
 *          Rest of XBUS Messages are silently dropped, same goes for
 *          MTData2 Data Packets which are not tracked.
 *
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>

#include "xbus.h"

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

/* Pack byte into internal buffer. */
static inline void xbus_pack_byte(xbus_t *xbus, uint8_t byte);

/* Reset XBUS Parser. */
static inline void xbus_reset(xbus_t *xbus);

/* Check if Message Checksum is Valid. */
static uint8_t xbus_crc(xbus_t *xbus);

/* Handle received message. */
static void xbus_handle_msg(xbus_t *xbus);

/* Invert bytes from start to end indices of array. */
static void xbus_swap(uint8_t *arr, uint16_t len, uint16_t start, uint16_t end);

/* Find Subscription. */
static uint8_t xbus_find_sub(xbus_t *xbus, uint16_t xdi);

/*******************************************************************************
 * Code
 ******************************************************************************/

void xbus_init(xbus_t *xbus)
{
	assert(xbus != NULL);

	xbus_reset(xbus);

	memset(xbus->sub, 0x00, sizeof(xbus->sub));
	memset(xbus->sub_cb, 0x00, sizeof(xbus->sub_cb));
	xbus->sub_cnt = 0x00;

#ifdef XBUS_COM_DBG
	xbus->drop_cnt = 0x00;
	xbus->pass_cnt = 0x00;
#endif
}

uint16_t xbus_sub(xbus_t *xbus, xbus_xdi_t xdi, uint8_t xdi_flags,
				  xbus_sub_callback cb)
{
	assert(xbus->sub_cnt <= XBUS_MAX_SUB_CNT);

	xbus->sub[xbus->sub_cnt] = xdi;
	xbus->sub_cb[xbus->sub_cnt] = cb;

	return xbus->sub_cnt++;
}

uint32_t xbus_parse(xbus_t *xbus, uint8_t byte)
{
	uint32_t ret = 1;

	switch (xbus->state) {
	case XBUS_STATE_PREAMBLE:
		if (byte == XBUS_DEF_PREAMBLE_BYTE) {
			xbus_pack_byte(xbus, byte);
			xbus->state = XBUS_STATE_BID;
		}

		break;
	case XBUS_STATE_BID:
		xbus_pack_byte(xbus, byte);
		xbus->state = XBUS_STATE_MID;

		if (byte != XBUS_DEF_BID_FIRST_DEV && byte != XBUS_DEF_BID_MASTER_DEV) {
			xbus_reset(xbus);
			ret = -1;

#ifdef XBUS_COM_DBG
			xbus->drop_cnt++;
#endif
		}

		break;
	case XBUS_STATE_MID:
		xbus_pack_byte(xbus, byte);
		xbus->state = XBUS_STATE_LEN;

		if (byte != XBUS_DEF_MTDATA2_MID) {
			xbus_reset(xbus);
			ret = -1;
		}

		break;
	case XBUS_STATE_LEN:
		xbus_pack_byte(xbus, byte);
		xbus->state = XBUS_STATE_DATA;

		xbus->len = byte;

		if (xbus->len > XBUS_DEF_MAX_STD_DATA_SIZE) {
			xbus_reset(xbus);
			ret = -1;

#ifdef XBUS_COM_DBG
			xbus->drop_cnt++;
#endif
		}

		break;
	case XBUS_STATE_DATA:
		xbus_pack_byte(xbus, byte);

		if ((xbus->idx - XBUS_DEF_HEADER_SIZE) >= xbus->len)
			xbus->state = XBUS_STATE_CRC;

		break;
	case XBUS_STATE_CRC:
		xbus_pack_byte(xbus, byte);

		if (xbus_crc(xbus) == 0x00) {
			/* packet is valid */
			xbus_handle_msg(xbus);
			ret = 0;

#ifdef XBUS_COM_DBG
			xbus->pass_cnt++;
#endif
		} else {
			ret = -1;

#ifdef XBUS_COM_DBG
			xbus->drop_cnt++;
#endif
		}

		xbus_reset(xbus);

		break;
	default:
		break;
	}

	return ret;
}

/****************************** static functions ******************************/

static inline void xbus_pack_byte(xbus_t *xbus, uint8_t byte)
{
	xbus->data[xbus->idx] = byte;
	xbus->idx++;
}

static inline void xbus_reset(xbus_t *xbus)
{
	xbus->idx = 0;
	xbus->state = XBUS_STATE_PREAMBLE;
	xbus->len = 0;
}

static uint8_t xbus_crc(xbus_t *xbus)
{
	uint8_t crc = 0;

	for (uint16_t i = 1; i < xbus->idx; i++)
		crc += *(xbus->data + i);

	return crc;
}

static void xbus_handle_msg(xbus_t *xbus)
{
	/* parse indivudal data packets and perform callback */
	for (uint16_t idx = XBUS_DEF_HEADER_SIZE; idx < (xbus->idx - 1);) {
		uint16_t pckt_id = 0;

		uint16_t start_idx = idx;

		xbus_swap(xbus->data, sizeof(xbus->data), idx, idx + 1);
		memcpy(&pckt_id, xbus->data + idx, sizeof(pckt_id));
		idx += 2;

		uint8_t pckt_size = *(xbus->data + idx);
		idx++;

		xbus_swap(xbus->data, sizeof(xbus->data), idx, idx + pckt_size - 1);

		memcpy(&xbus->packet, xbus->data + start_idx, pckt_size + 3);
		idx += pckt_size;

		/* Callbacks */
		uint8_t sub_id = xbus_find_sub(xbus, pckt_id);

		if (sub_id != 0xFF && xbus->sub_cb[sub_id] != NULL) {
			xbus_xdi_t xdi = pckt_id & 0xFFF0U;
			uint8_t flags = pckt_id & 0x000FU;

			xbus->sub_cb[sub_id](xbus, &xbus->packet, xdi, flags);
		}
	}
}

static void xbus_swap(uint8_t *arr, uint16_t len, uint16_t start, uint16_t end)
{
	if (start < end && end < len) {
		uint16_t i = start;
		uint16_t j = end;

		while (i < j) {
			uint8_t pom = arr[i];
			arr[i] = arr[j];
			arr[j] = pom;

			i++;
			j--;
		}
	}
}

static uint8_t xbus_find_sub(xbus_t *xbus, uint16_t xdi)
{
	uint8_t ret = 0xFF;

	for (uint8_t i = 0; i < xbus->sub_cnt; i++) {
		if (xbus->sub[i] == xdi) {
			ret = i;
			break;
		}
	}

	return ret;
}

/********************************* End Of File ********************************/