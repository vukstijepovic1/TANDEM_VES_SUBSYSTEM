/**
 * @file    ms5607.c
 * @brief   MS5607 Barometer driver.
 * @version 1.1.0
 * @date    29.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>

#include "util.h"

#include "ms5607.h"
#include "ms5607_def.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Soft reset MS5607. */
static lStatus_t ms5607_soft_reset(ms5607_t *dev);

/* Check CRC4, please refer to TE Connectivity Application Note AN520 */
static uint8_t ms5607_crc4(uint16_t *n_prom);

/* Read MS5607 calibration values. */
static lStatus_t ms5607_read_prom(ms5607_t *dev);

/* Convert Raw Temperature to Temperature [cdegC]. */
static int32_t ms5607_calc_temp(ms5607_t *dev, uint32_t raw_temp);

/* Convert Raw Pressure to Pressure [Pa]. */
static int32_t ms5607_calc_press(ms5607_t *dev, uint32_t raw_press);

/*******************************************************************************
 * Code
 ******************************************************************************/

lStatus_t ms5607_init(ms5607_t *dev, ms5607_interface_t *interface)
{
	if (dev == NULL || interface == NULL)
		return lStatus_Fail;

	lStatus_t status = lStatus_Fail;

	dev->interface = interface;

	// Reset device.
	status = ms5607_soft_reset(dev);

	if (status == lStatus_Success) {
		// Try reading PROM up to five times
		uint8_t cnt = 0;
		do {
			status = ms5607_read_prom(dev);
		} while (status != lStatus_Success && cnt++ < 5);
	}

	return status;
}

lStatus_t ms5607_measure(ms5607_t *dev, ms5607_meas_t meas, ms5607_osr_t osr,
						 uint32_t *t_us)
{
	lStatus_t status = lStatus_Fail;
	uint32_t conv_period = 0;
	uint8_t cmd = 0;

	if (dev == NULL)
		return lStatus_Fail;

	if (meas != MS5607_MEAS_PRESSURE && meas != MS5607_MEAS_TEMPERATURE)
		return lStatus_Fail;

	switch (osr) {
	case MS5607_OSR_256:
		conv_period = MS5607_CONV_256_TIME_US;

		if (meas == MS5607_MEAS_PRESSURE)
			cmd = MS5607_DEF_CMD_CONV_D1_256;
		else
			cmd = MS5607_DEF_CMD_CONV_D2_256;

		break;
	case MS5607_OSR_512:
		conv_period = MS5607_CONV_512_TIME_US;

		if (meas == MS5607_MEAS_PRESSURE)
			cmd = MS5607_DEF_CMD_CONV_D1_512;
		else
			cmd = MS5607_DEF_CMD_CONV_D2_512;

		break;
	case MS5607_OSR_1024:
		conv_period = MS5607_CONV_1024_TIME_US;

		if (meas == MS5607_MEAS_PRESSURE)
			cmd = MS5607_DEF_CMD_CONV_D1_1024;
		else
			cmd = MS5607_DEF_CMD_CONV_D2_1024;

		break;
	case MS5607_OSR_2048:
		conv_period = MS5607_CONV_2048_TIME_US;

		if (meas == MS5607_MEAS_PRESSURE)
			cmd = MS5607_DEF_CMD_CONV_D1_2048;
		else
			cmd = MS5607_DEF_CMD_CONV_D2_2048;

		break;
	case MS5607_OSR_4096:
		conv_period = MS5607_CONV_4096_TIME_US;

		if (meas == MS5607_MEAS_PRESSURE)
			cmd = MS5607_DEF_CMD_CONV_D1_4096;
		else
			cmd = MS5607_DEF_CMD_CONV_D2_4096;

		break;
	default:
		conv_period = 0;
	}

	if (conv_period != 0) {
		status = dev->interface->send_cmd(dev->interface, cmd);

		if (status == lStatus_Success) {
			dev->curr_meas = meas;
			*t_us = conv_period;
		}
	}

	return status;
}

lStatus_t ms5607_collect(ms5607_t *dev, ms5607_meas_t meas, float *val)
{
	lStatus_t status = lStatus_Fail;
	uint32_t raw_val = 0;
	uint8_t cmd = 0;
	uint8_t resp[3] = {0};

	if (dev == NULL)
		return lStatus_Fail;

	if (meas != MS5607_MEAS_PRESSURE && meas != MS5607_MEAS_TEMPERATURE)
		return lStatus_Fail;

	// Check if ADC conversion is same as requested measurement.
	if (dev->curr_meas != meas)
		return lStatus_Fail;

	cmd = MS5607_DEF_CMD_ADC_READ;

	status = dev->interface->read(dev->interface, cmd, resp, sizeof(resp));

	if (status == lStatus_Success) {
		raw_val = (uint32_t)(((uint32_t)resp[0] << 16) |
							 ((uint32_t)resp[1] << 8) | (uint32_t)resp[2]);

		/* calc real value */
		if (raw_val != 0) {
			if (meas == MS5607_MEAS_PRESSURE) {
				// Convert Pressure to bar
				*val = ms5607_calc_press(dev, raw_val);
			} else {
				// Convert Temperature to degC
				*val = ms5607_calc_temp(dev, raw_val) * .01;
			}

			dev->curr_meas = MS5607_MEAS_NONE;
		} else {
			status = lStatus_Fail;
		}
	}

	return status;
}

/****************************** static functions ******************************/

static lStatus_t ms5607_soft_reset(ms5607_t *dev)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cmd = MS5607_DEF_CMD_RESET;

	status = dev->interface->send_cmd(dev->interface, cmd);

	if (status == lStatus_Success)
		HAL_DelayMS(MS5607_DEF_RESET_TIME_MS);

	return status;
}

static uint8_t ms5607_crc4(uint16_t *n_prom)
{
	int16_t cnt;	   // simple counter
	uint16_t n_rem;	   // crc reminder
	uint16_t crc_read; // original value of the crc
	uint8_t n_bit;

	n_rem = 0x00;

	// save the read CRC
	crc_read = n_prom[7];

	// CRC byte is replaced by 0
	n_prom[7] = (0xFF00 & (n_prom[7]));

	for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
	{
		// choose LSB or MSB
		if (cnt & 1)
			n_rem ^= (unsigned short)((n_prom[cnt >> 1]) & 0x00FF);
		else
			n_rem ^= (unsigned short)(n_prom[cnt >> 1] >> 8);

		for (n_bit = 8; n_bit > 0; n_bit--) {
			if (n_rem & (0x8000))
				n_rem = (n_rem << 1) ^ 0x3000;
			else
				n_rem = (n_rem << 1);
		}
	}

	// final 4-bit reminder is CRC code
	n_rem = (0x000F & (n_rem >> 12));

	// restore the crc_read to its original place
	n_prom[7] = crc_read;

	return ((crc_read & 0x000F) == (n_rem ^ 0x00));
}

static lStatus_t ms5607_read_prom(ms5607_t *dev)
{
	lStatus_t status = lStatus_Fail;

	/* read calibration data */
	for (uint32_t i = 0; i < 8; i++) {
		uint8_t resp[2] = {0};
		uint8_t cmd = 0;

		cmd = (uint8_t)(MS5607_DEF_CMD_PROM_START + i * 2);

		status = dev->interface->read(dev->interface, cmd, resp, sizeof(resp));

		if (status == lStatus_Success)
			dev->prom.c[i] = ((uint16_t)resp[0] << 8U) | resp[1];
		else
			return lStatus_Fail;
	}

	if (ms5607_crc4(dev->prom.c)) {
		return lStatus_Success;
	}

	return lStatus_Fail;
}

static int32_t ms5607_calc_temp(ms5607_t *dev, uint32_t raw_temp)
{
	// For more information please refer to MS5607-02BA03 datasheet pages 8-9
	int32_t dT, TEMP;
	int64_t OFF, SENS, T2, OFF2, SENS2;

	dT = (int32_t)raw_temp - (int32_t)(dev->prom.s.c5 << 8);
	TEMP = 2000 + ((int64_t)dT * (int64_t)dev->prom.s.c6 >> 23);

	OFF =
		((int64_t)dev->prom.s.c2 << 17) + (((int64_t)dev->prom.s.c4 * dT) >> 6);
	SENS =
		((int64_t)dev->prom.s.c1 << 16) + (((int64_t)dev->prom.s.c3 * dT) >> 7);

	// Second order temperature compensation
	if (TEMP < 2000) {
		T2 = ((int64_t)dT * (int64_t)dT) >> 31;
		int64_t f1 = ((int64_t)TEMP - 2000) * ((int64_t)TEMP - 2000);
		OFF2 = 61 * f1 >> 4;
		SENS2 = 2 * f1;

		if (TEMP < -1500) {
			int64_t f2 = ((int64_t)TEMP + 1500) * ((int64_t)TEMP + 1500);
			OFF2 += 15 * f2;
			SENS2 += 8 * f2;
		}
	} else {
		T2 = 0;
		OFF2 = 0;
		SENS2 = 0;
	}

	TEMP = TEMP - T2;
	dev->OFF = OFF - OFF2;
	dev->SENS = SENS - SENS2;

	return (int32_t)TEMP;
}

static int32_t ms5607_calc_press(ms5607_t *dev, uint32_t raw_press)
{
	// For more information please refer to MS5607-02BA03 datasheet pages 8-9
	int64_t P;

	P = (((raw_press * dev->SENS) >> 21) - dev->OFF) >> 15;

	return (int32_t)P;
}

/************************************* EOF ************************************/