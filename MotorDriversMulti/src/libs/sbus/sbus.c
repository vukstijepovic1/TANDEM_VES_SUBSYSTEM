/**
 * @file    sbus.c
 * @brief   SBUS Library.
 * @version 1.0.0
 * @date    22.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "sbus.h"

#include <string.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define SBUS_PACKET_TIMEOUT_US (SBUS_PACKET_TIMEOUT_MS * 1000)

#define SBUS_GET_BIT(x, offset) ((x & (1 << offset)) >> offset)

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

void sbus_init(sbus_t *sbus, uint64_t (*time_us)())
{
	sbus->i = 0;
	memset(&sbus->data, 0x00, sizeof(sbus->data));
	sbus->time_us = time_us;
	sbus->pckt_start_time = 0;
#ifdef SBUS_DBG
	sbus->drop_cnt = 0;
	sbus->pass_cnt = 0;
#endif
}

int32_t sbus_parse(sbus_t *sbus, uint8_t byte)
{
	int ret = 0;

	switch (sbus->parser_state) {
	case SBUS_PARSER_STATE_HEADER:
		if (byte == SBUS_PACKET_HEADER) {
			sbus->arr[sbus->i] = byte;
			sbus->i++;
			sbus->parser_state = SBUS_PARSER_STATE_DATA;
			sbus->pckt_start_time = sbus->time_us();
		}

		break;
	case SBUS_PARSER_STATE_DATA:
		if (sbus->time_us() - sbus->pckt_start_time >= SBUS_PACKET_TIMEOUT_US) {
			sbus->i = 0;
			sbus->parser_state = SBUS_PARSER_STATE_HEADER;
			ret = -1;
#ifdef SBUS_DBG
			sbus->drop_cnt++;
#endif
			break;
		}

		sbus->arr[sbus->i] = byte;
		sbus->i++;

		if (sbus->i == SBUS_PACKET_SIZE - 1)
			sbus->parser_state = SBUS_PARSER_STATE_FOOTER;

		break;
	case SBUS_PARSER_STATE_FOOTER:
		if (sbus->time_us() - sbus->pckt_start_time >= SBUS_PACKET_TIMEOUT_US) {
			sbus->i = 0;
			sbus->parser_state = SBUS_PARSER_STATE_HEADER;
			ret = -1;
#ifdef SBUS_DBG
			sbus->drop_cnt++;
#endif
			break;
		}

		sbus->arr[sbus->i] = byte;

		if (byte == SBUS_PACKET_FOOTER) {
			uint32_t val = 0x00;
			uint8_t off = 0;
			uint8_t ch_cnt = 0;

			for (uint32_t i = 1; i < (SBUS_PACKET_SIZE - 2); i++) {
				val |= (sbus->arr[i] << off);
				off += 8;

				while (off >= 11) {
					sbus->data.pwm_ch[ch_cnt++] = val & SBUS_PWM_CHANNEL_MASK;
					val >>= 11;
					off -= 11;
				}
			}

			sbus->data.digi_ch[0] =
				SBUS_GET_BIT(sbus->arr[23], SBUS_CH_17_OFFSET);
			sbus->data.digi_ch[1] =
				SBUS_GET_BIT(sbus->arr[23], SBUS_CH_18_OFFSET);
			sbus->data.frame_lost =
				SBUS_GET_BIT(sbus->arr[23], SBUS_FRAME_LOST_OFFSET);
			sbus->data.failsafe =
				SBUS_GET_BIT(sbus->arr[23], SBUS_FAILSAFE_OFFSET);

			ret = 1;
#ifdef SBUS_DBG
			sbus->pass_cnt++;
#endif
		} else {
			ret = -1;
#ifdef SBUS_DBG
			sbus->drop_cnt++;
#endif
		}

		sbus->i = 0;
		sbus->parser_state = SBUS_PARSER_STATE_HEADER;

		break;
	default:
		break;
	}

	return ret;
}

void sbus_pack(uint8_t *arr, sbus_data_t *data)
{
	uint32_t val = 0;
	uint8_t off = 0;

	memset(arr, 0x00, SBUS_PACKET_SIZE);

	arr[0] = SBUS_PACKET_HEADER;
	for (uint32_t i = 0, cnt = 1; i < SBUS_PWM_CHANNEL_CNT; i++) {
		val |= ((data->pwm_ch[i] & SBUS_PWM_CHANNEL_MASK) << off);
		off += 11;

		while (off >= 8) {
			arr[cnt] = val & 0xFFU;
			cnt++;

			val >>= 8;
			off -= 8;
		}
	}
	arr[23] = data->digi_ch[0] << SBUS_CH_17_OFFSET |
			  data->digi_ch[1] << SBUS_CH_18_OFFSET |
			  data->frame_lost << SBUS_FRAME_LOST_OFFSET |
			  data->failsafe << SBUS_FAILSAFE_OFFSET;
	arr[24] = SBUS_PACKET_FOOTER;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/