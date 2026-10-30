/**
 * @file    motor.c
 * @brief   EDePro turbomotor driver.
 * @version	1.0.0
 * @date    18.05.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>

#include "motor.h"
#include "motor_def.h"

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

/* unpack received status */
static void motor_status_unpack(motor_t *motor);

/* calculate CRC16 */
static uint16_t motor_crc(uint8_t *data, uint16_t length);

/*******************************************************************************
 * Code
 ******************************************************************************/

void motor_init(motor_t *motor)
{
	memset(&motor->status, 0x00, sizeof(motor->status));
	memset(motor->arr, 0x00, sizeof(motor->arr));
	motor->idx = 0;
	motor->state = MOTOR_PARSER_STATE_HEADER1;

#ifdef MOTOR_DBG
	motor->pass_cnt = 0;
	motor->drop_cnt = 0;
#endif
}

uint32_t motor_parse(motor_t *motor, uint8_t byte)
{
	uint32_t ret = 1;

	switch (motor->state) {
	case MOTOR_PARSER_STATE_HEADER1:
		if (byte == MOTOR_DEF_STATUS_HEADER1) {
			motor->state = MOTOR_PARSER_STATE_HEADER2;
			motor->arr[motor->idx] = byte;
			motor->idx++;
		} else {
			motor->idx = 0;
			ret = -1;
		}

		break;
	case MOTOR_PARSER_STATE_HEADER2:
		if (byte == MOTOR_DEF_STATUS_HEADER2) {
			motor->state = MOTOR_PARSER_STATE_DATA;
			motor->arr[motor->idx] = byte;
			motor->idx++;
		} else {
			motor->state = MOTOR_PARSER_STATE_HEADER1;
			motor->idx = 0;
			ret = -1;
		}

		break;
	case MOTOR_PARSER_STATE_DATA:
		motor->arr[motor->idx] = byte;
		motor->idx++;
		if (motor->idx == MOTOR_STATUS_LEN - 2)
			motor->state = MOTOR_PARSER_STATE_CRC1;

		break;
	case MOTOR_PARSER_STATE_CRC1:
		motor->state = MOTOR_PARSER_STATE_CRC2;
		motor->arr[motor->idx] = byte;
		motor->idx++;

		break;
	case MOTOR_PARSER_STATE_CRC2:
		motor->state = MOTOR_PARSER_STATE_HEADER1;
		motor->arr[motor->idx] = byte;
		motor->idx = 0;

		uint16_t crc;
		uint16_t rx_crc;

		crc = motor_crc(motor->arr, MOTOR_STATUS_LEN - 2);
		rx_crc = (uint16_t)motor->arr[MOTOR_STATUS_LEN - 1] << 8;
		rx_crc |= (uint16_t)motor->arr[MOTOR_STATUS_LEN - 2];

		if (rx_crc == crc) {
			motor_status_unpack(motor);
			ret = 0;
#ifdef MOTOR_DBG
			motor->pass_cnt++;
#endif
		} else {
			ret = -1;
#ifdef MOTOR_DBG
			motor->drop_cnt++;
#endif
		}

		break;
	default:
		motor->state = MOTOR_PARSER_STATE_HEADER1;
		motor->idx = 0;
		ret = -1;

		break;
	}

	return ret;
}

/****************************** static functions ******************************/

static void motor_status_unpack(motor_t *motor)
{
	memcpy(&motor->status, motor->arr, MOTOR_STATUS_LEN);
}

static uint16_t motor_crc(uint8_t *data, uint16_t length)
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