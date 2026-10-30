/**
 * @file    bms.c
 * @brief   Daly BMS driver.
 * @version	1.0.0
 * @date    18.05.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "bms.h"
#include "bms_def.h"
#include <string.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define BMS_STATUS_RESPONSE_EXTRACT_FIELD(response, index)                     \
	((uint16_t)((response[index] << 8) + response[index + 1]))

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* unpack received BMS status */
static void bms_status_unpack(bms_t *bms);

/* calculate MODBUS CRC16 */
static uint16_t bms_crc(uint8_t *data, uint16_t length);

/*******************************************************************************
 * Code
 ******************************************************************************/

void bms_init(bms_t *bms)
{
	memset(&bms->status, 0x00, sizeof(bms->status));
	bms->state = BMS_PARSER_STATE_ADDR;
	bms->idx = 0;
	bms->len = 0;
	bms->data_idx = 0;

#ifdef BMS_DBG
	bms->drop_cnt = 0;
	bms->pass_cnt = 0;
#endif
}

uint32_t bms_parse(bms_t *bms, uint8_t data)
{
	uint32_t ret = 1;

	switch (bms->state) {
	case BMS_PARSER_STATE_ADDR:
		if (data == BMS_DEF_SLAVE_ADDR) {
			bms->state = BMS_PARSER_STATE_CMD;
			bms->arr[bms->idx] = data;
			bms->idx++;
		} else {
			bms->idx = 0;
			ret = -1;
		}

		break;
	case BMS_PARSER_STATE_CMD:
		if (data == BMS_DEF_READ_CMD) {
			bms->state = BMS_PARSER_STATE_LEN;
			bms->arr[bms->idx] = data;
			bms->idx++;
		} else {
			bms->state = BMS_PARSER_STATE_ADDR;
			bms->idx = 0;
			ret = -1;
		}

		break;
	case BMS_PARSER_STATE_LEN:
		if (data == 2 * BMS_STATUS_REG_COUNT) {
			bms->len = data;
			bms->data_idx = 0;

			bms->state = BMS_PARSER_STATE_DATA;
			bms->arr[bms->idx] = data;
			bms->idx++;
		} else {
			bms->state = BMS_PARSER_STATE_ADDR;
			bms->idx = 0;
			ret = -1;
		}

		break;
	case BMS_PARSER_STATE_DATA:
		bms->data_idx++;

		if (bms->data_idx < bms->len)
			bms->state = BMS_PARSER_STATE_DATA;
		else
			bms->state = BMS_PARSER_STATE_CRC1;

		bms->arr[bms->idx] = data;
		bms->idx++;

		break;
	case BMS_PARSER_STATE_CRC1:
		bms->state = BMS_PARSER_STATE_CRC2;
		bms->arr[bms->idx] = data;
		bms->idx++;

		break;
	case BMS_PARSER_STATE_CRC2:
		bms->state = BMS_PARSER_STATE_ADDR;
		bms->arr[bms->idx] = data;
		bms->idx = 0;

		uint16_t crc = 0x00;
		uint16_t rx_crc = 0x00;

		crc = bms_crc(bms->arr, BMS_STATUS_RESPONSE_LEN - 2);
		rx_crc = (uint16_t)bms->arr[BMS_STATUS_RESPONSE_LEN - 1] << 8;
		rx_crc |= (uint16_t)bms->arr[BMS_STATUS_RESPONSE_LEN - 2];

		if (rx_crc == crc) {
			bms_status_unpack(bms);
			ret = 0;
#ifdef BMS_DBG
			bms->pass_cnt++;
#endif
		} else {
			ret = -1;
#ifdef BMS_DBG
			bms->drop_cnt++;
#endif
		}

		break;
	default:
		bms->state = BMS_PARSER_STATE_ADDR;
		bms->idx = 0;
		ret = -1;

		break;
	}

	return ret;
}

void bms_status_req_pack(bms_t *bms, uint8_t *data)
{
	uint16_t checksum;

	data[0] = BMS_DEF_SLAVE_ADDR;
	data[1] = BMS_DEF_READ_CMD;
	data[2] = BMS_DEF_REG_ADDR_BAT_1_VOLT >> 8;
	data[3] = (uint8_t)(BMS_DEF_REG_ADDR_BAT_1_VOLT & 0xFF);
	data[4] = BMS_STATUS_REG_COUNT >> 8;
	data[5] = BMS_STATUS_REG_COUNT & 0xFF;
	checksum = bms_crc(data, 6);
	data[6] = (uint8_t)(checksum & 0xFF);
	data[7] = (uint8_t)(checksum >> 8);
}

/****************************** static functions ******************************/

static void bms_status_unpack(bms_t *bms)
{
	int idx;
	int off = 3; // skip addr, cmd and length fields
	int start_addr = BMS_DEF_REG_ADDR_BAT_1_VOLT;

	/* get voltages */
	for (int i = 0; i < 14; i++) {
		idx = off + 2 * i;
		bms->status.cell_volt[i] =
			BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);
	}

	/* get temperature */
	idx = off + 2 * (BMS_DEF_REG_ADDR_TEMP - start_addr);
	bms->status.temp = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx) - 40;

	/* get sum voltage */
	idx = off + 2 * (BMS_DEF_REG_ADDR_SUM_VOLT - start_addr);
	bms->status.sum_volt = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get current */
	idx = off + 2 * (BMS_DEF_REG_ADDR_CURR - start_addr);
	bms->status.curr = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx) - 30000;

	/* get state of charge */
	idx = off + 2 * (BMS_DEF_REG_ADDR_STATE_OF_CHARGE_RO - start_addr);
	bms->status.soc = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get maximum voltage */
	idx = off + 2 * (BMS_DEF_REG_ADDR_MAX_VOLT - start_addr);
	bms->status.max_volt = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get minimum voltage */
	idx = off + 2 * (BMS_DEF_REG_ADDR_MIN_VOLT - start_addr);
	bms->status.min_volt = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get maximum monomer temperature */
	idx = off + 2 * (BMS_DEF_REG_ADDR_MAX_MONOMER_TEMP - start_addr);
	bms->status.max_temp =
		BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx) - 40;

	/* get minimum monomer temperature */
	idx = off + 2 * (BMS_DEF_REG_ADDR_MIN_MONOMER_TEMP - start_addr);
	bms->status.min_temp =
		BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx) - 40;

	/* get charge state */
	idx = off + 2 * (BMS_DEF_REG_ADDR_CHARGE_STATE - start_addr);
	bms->status.charge_state = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get remaining capacity */
	idx = off + 2 * (BMS_DEF_REG_ADDR_REMAINING_CAPACITY - start_addr);
	bms->status.rem_capacity = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get number of batteries */
	idx = off + 2 * (BMS_DEF_REG_ADDR_NUM_OF_BAT - start_addr);
	bms->status.cell_num = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get number of temperature sensors */
	idx = off + 2 * (BMS_DEF_REG_ADDR_NUM_OF_TEMP_SENSORS - start_addr);
	bms->status.temp_sensor_num =
		BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get number of cycles */
	idx = off + 2 * (BMS_DEF_REG_ADDR_CYCLES - start_addr);
	bms->status.cycles = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get equilibrium status */
	idx = off + 2 * (BMS_DEF_REG_ADDR_EQUILIBRIUM_STAT - start_addr);
	bms->status.eq_stat = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get charge MOSFET state */
	idx = off + 2 * (BMS_DEF_REG_ADDR_CHARGE_MOS_STAT - start_addr);
	bms->status.charge_mos_stat =
		BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get discharge MOSFET state */
	idx = off + 2 * (BMS_DEF_REG_ADDR_DISCHARGE_MOS_STAT - start_addr);
	bms->status.discharge_mos_stat =
		BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get average voltage */
	idx = off + 2 * (BMS_DEF_REG_ADDR_AVG_VOLT - start_addr);
	bms->status.avg_volt = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get diff voltage */
	idx = off + 2 * (BMS_DEF_REG_ADDR_DIFF_VOLT - start_addr);
	bms->status.diff_volt = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get status */
	idx = off + 2 * (BMS_DEF_REG_ADDR_POWER - start_addr);
	bms->status.power = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get fault status 1 */
	idx = off + 2 * (BMS_DEF_REG_ADDR_FAULT_STAT_1 - start_addr);
	bms->status.fault1 = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get fault status 2 */
	idx = off + 2 * (BMS_DEF_REG_ADDR_FAULT_STAT_2 - start_addr);
	bms->status.fault2 = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get fault status 3 */
	idx = off + 2 * (BMS_DEF_REG_ADDR_FAULT_STAT_3 - start_addr);
	bms->status.fault3 = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);

	/* get fault status 4 */
	idx = off + 2 * (BMS_DEF_REG_ADDR_FAULT_STAT_4 - start_addr);
	bms->status.fault4 = BMS_STATUS_RESPONSE_EXTRACT_FIELD(bms->arr, idx);
}

static uint16_t bms_crc(uint8_t *data, uint16_t length)
{
	uint16_t checksum = 0xFFFF;
	uint16_t c = 0xA001;
	uint16_t tmp = 0x0001;

	for (int i = 0; i < length; i++) {
		checksum = ((uint16_t)data[i]) ^ checksum;
		for (int j = 0; j < 8; j++) {
			if (tmp & checksum) {
				checksum = checksum >> 1;
				checksum = c ^ checksum;
			} else {
				checksum = checksum >> 1;
			}
		}
	}

	return checksum;
}

/********************************* End Of File ********************************/