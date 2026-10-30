/**
 * @file    ubx_parser.c
 * @brief   UBX parser.
 * @version	1.0.0
 * @date    02.04.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ubx_parser.h"
#include "ubx_def.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define UBX_NONDATA_BYTES sizeof(ubx_preamble_t) + sizeof(ubx_checksum_t)

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void ubx_pack_byte(ubx_t *inst, const uint8_t recv_byte);

static void ubx_crc_init(ubx_checksum_t *checksum, const uint8_t byte);

static void ubx_crc_add(ubx_checksum_t *checksum, const uint8_t byte);

/*******************************************************************************
 * Code
 ******************************************************************************/

void ubx_init(ubx_t *inst)
{
	inst->state = UBX_PARSER_STATE_HEADER1;
	inst->idx = 0U;
	inst->length = 0U;
	inst->recv_checksum = 0U;
	inst->packet_class = UBX_PACKET_CLASS_INVALID;
	inst->packet_id = UBX_PACKET_ID_INVALID;
	inst->checksum.ck_a = 0U;
	inst->checksum.ck_b = 0U;
#ifdef UBX_DBG
	inst->pass_cnt = 0U;
	inst->drop_cnt = 0U;
#endif
}

uint32_t ubx_parse(ubx_t *inst, const uint8_t recv_byte)
{
	uint32_t ret = 1;

	switch (inst->state) {
	case UBX_PARSER_STATE_HEADER1: /* get header1 */
		if (recv_byte == UBX_DEF_HEADER1) {
			inst->packet_class = UBX_PACKET_CLASS_INVALID;
			inst->packet_id = UBX_PACKET_ID_INVALID;
			inst->idx = 0U;
			ubx_pack_byte(inst, recv_byte);
			inst->state = UBX_PARSER_STATE_HEADER2;
		}

		break;
	case UBX_PARSER_STATE_HEADER2: /* get header 2 */
		if (recv_byte == UBX_DEF_HEADER2) {
			inst->state = UBX_PARSER_STATE_CLASS;
			ubx_pack_byte(inst, recv_byte);
		} else {
			inst->state = UBX_PARSER_STATE_HEADER1;
		}

		break;
	case UBX_PARSER_STATE_CLASS: /* get packet class */
		inst->packet_class = recv_byte;
		ubx_crc_init(&inst->checksum, recv_byte);
		ubx_pack_byte(inst, recv_byte);
		inst->state = UBX_PARSER_STATE_ID;

		break;
	case UBX_PARSER_STATE_ID: /* get packet ID */
		inst->packet_id = recv_byte;
		ubx_crc_add(&inst->checksum, recv_byte);
		ubx_pack_byte(inst, recv_byte);
		inst->state = UBX_PARSER_STATE_LENGTH1;

		break;
	case UBX_PARSER_STATE_LENGTH1: /* get length1 */
		inst->length = recv_byte;
		ubx_crc_add(&inst->checksum, recv_byte);
		ubx_pack_byte(inst, recv_byte);
		inst->state = UBX_PARSER_STATE_LENGTH2;

		break;
	case UBX_PARSER_STATE_LENGTH2: /* get length2 */
		inst->length |= ((uint16_t)recv_byte << 8);
		/* If length arrives corrupted, don't write the payload */
		if (inst->length <= (sizeof(inst->arr) - UBX_NONDATA_BYTES)) {
			ubx_crc_add(&inst->checksum, recv_byte);
			ubx_pack_byte(inst, recv_byte);
			inst->state = UBX_PARSER_STATE_PAYLOAD;
		} else {
			inst->state = UBX_PARSER_STATE_HEADER1;
		}

		break;
	case UBX_PARSER_STATE_PAYLOAD: /* get payload */
		ubx_crc_add(&inst->checksum, recv_byte);
		ubx_pack_byte(inst, recv_byte);

		if (inst->idx >= (inst->length + sizeof(ubx_preamble_t))) {
			inst->state = UBX_PARSER_STATE_CHECKSUM1;
		}

		break;
	case UBX_PARSER_STATE_CHECKSUM1: /* het checksum1 */
		inst->recv_checksum = recv_byte;
		inst->state = UBX_PARSER_STATE_CHECKSUM2;
		ubx_pack_byte(inst, recv_byte);

		break;
	case UBX_PARSER_STATE_CHECKSUM2: /* get checksum2 */
		inst->recv_checksum |= ((uint16_t)recv_byte << 8);
		ubx_pack_byte(inst, recv_byte);

		uint16_t actual_checksum =
			(inst->checksum.ck_a | ((uint16_t)inst->checksum.ck_b << 8));

		if (inst->recv_checksum == actual_checksum) {
			ret = 0;
#ifdef UBX_DBG
			inst->pass_cnt++;
#endif
		} else {
			ret = -1;
#ifdef UBX_DBG
			inst->drop_cnt++;
#endif
		}
		inst->state = UBX_PARSER_STATE_HEADER1;

		break;
	default:
		/* A fault occured somewhere */
		inst->state = UBX_PARSER_STATE_HEADER1;
		break;
	}

	return ret;
}

/****************************** static functions ******************************/

static void ubx_pack_byte(ubx_t *inst, const uint8_t recv_byte)
{
	inst->arr[inst->idx] = recv_byte;
	inst->idx++;
}

static void ubx_crc_init(ubx_checksum_t *checksum, const uint8_t byte)
{
	checksum->ck_a = byte;
	checksum->ck_b = byte;
}

static void ubx_crc_add(ubx_checksum_t *checksum, const uint8_t byte)
{
	checksum->ck_a = checksum->ck_a + byte;
	checksum->ck_b = checksum->ck_b + checksum->ck_a;
}

/********************************* End Of File ********************************/