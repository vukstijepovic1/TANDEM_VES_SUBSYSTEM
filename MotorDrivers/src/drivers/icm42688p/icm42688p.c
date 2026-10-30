/**
 * @file    icm42688p.c
 * @brief   ICM42688P Accelerometer & Gyroscope Driver.
 * @version	1.0.0
 * @date    23.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "icm42688p.h"
#include "icm42688p_def.h"

#include <string.h>

#include "util.h"

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

/* Read data from ICM42688P registers. */
static lStatus_t icm42688p_read(icm42688p_t *dev, uint16_t reg, uint8_t *vals,
								uint8_t cnt);

/* Write data into ICM42688P register. */
static lStatus_t icm42688p_write(icm42688p_t *dev, uint16_t reg, uint8_t data);

/* Change ICM42688P bank. */
static lStatus_t icm42688p_change_bank(icm42688p_t *dev, uint8_t bank);

/* Check ICM42688P Who Am I register. */
static lStatus_t icm42688p_who_am_i(icm42688p_t *dev);

/* ICM42688P Software Reset. */
static lStatus_t icm42688p_reset(icm42688p_t *dev);

/*******************************************************************************
 * Code
 ******************************************************************************/

lStatus_t icm42688p_init(icm42688p_t *dev, icm42688p_interface_t *interface,
						 icm42688p_cfg_t *cfg)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cmd;

	dev->interface = interface;
	dev->bank = -1;

	/* Get Gyroscope sensitivity. */
	switch (cfg->gyro_fs) {
	case ICM42688P_GYRO_FS_SEL_2000:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_2000;
		break;
	case ICM42688P_GYRO_FS_SEL_1000:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_1000;
		break;
	case ICM42688P_GYRO_FS_SEL_500:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_500;
		break;
	case ICM42688P_GYRO_FS_SEL_250:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_250;
		break;
	case ICM42688P_GYRO_FS_SEL_125:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_125;
		break;
	case ICM42688P_GYRO_FS_SEL_62_5:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_62_5;
		break;
	case ICM42688P_GYRO_FS_SEL_31_25:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_31_25;
		break;
	case ICM42688P_GYRO_FS_SEL_15_625:
		dev->gyro_sens = ICM42688P_DEF_GYRO_SENS_FS_SEL_15_625;
		break;
	default:
		return lStatus_Fail;
	}

	/* Get Accelerometer sensitivity. */
	switch (cfg->accel_fs) {
	case ICM42688P_ACCEL_FS_SEL_16:
		dev->accel_sens = ICM42688P_DEF_ACCEL_SENS_FS_SEL_16;
		break;
	case ICM42688P_ACCEL_FS_SEL_8:
		dev->accel_sens = ICM42688P_DEF_ACCEL_SENS_FS_SEL_8;
		break;
	case ICM42688P_ACCEL_FS_SEL_4:
		dev->accel_sens = ICM42688P_DEF_ACCEL_SENS_FS_SEL_4;
		break;
	case ICM42688P_ACCEL_FS_SEL_2:
		dev->accel_sens = ICM42688P_DEF_ACCEL_SENS_FS_SEL_2;
		break;
	default:
		return lStatus_Fail;
	}

	/* Check Who Am I register. */
	status = icm42688p_who_am_i(dev);
	if (status != lStatus_Success)
		return status;

	/* Perform Soft Reset. */
	status = icm42688p_reset(dev);
	if (status != lStatus_Success)
		return status;

	/* Set SPI Mode. */
	cmd = cfg->spi_mode << 4;
	status = icm42688p_write(dev, ICM42688P_DEF_DEVICE_CONFIG_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/*
	 * Sense registers and FIFO buffer will hold last valid sample, FIFO count
	 * is repoted in bytes, while FIFO count and Sensor data is Little Endian.
	 */
	cmd = (0x01U << 7) | 0x03U;
	status = icm42688p_write(dev, ICM42688P_DEF_INTF_CONFIG0_REG, cmd);
	if (status != lStatus_Success)
		return status;

	HAL_DelayMS(ICM42688P_DEF_GYRO_TRANSITION_PERIOD_MS);

	/*
	 * Enable temperature sensor, place gyro and accel in Low Noise (LN) mode.
	 */
	cmd = (0x03U << 2) | 0x03U;
	status = icm42688p_write(dev, ICM42688P_DEF_PWR_MGMT0_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/*
	 * Set gyroscope full scale (FS) & output data rate (ODR).
	 */
	cmd = (cfg->gyro_fs << 5) | cfg->gyro_odr;
	status = icm42688p_write(dev, ICM42688P_DEF_GYRO_CONFIG0_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/*
	 * Set accelerometer full scale (FS) & output data rate (ODR)
	 */
	cmd = (cfg->accel_fs << 5) | cfg->accel_odr;
	status = icm42688p_write(dev, ICM42688P_DEF_ACCEL_CONFIG0_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/*
	 * Enable timestamp, disable Timestamp FSYNC feature, Timestamp field will
	 * contain measurement of time since last occurance of ODR, Timestamp
	 * resolution is 1us.
	 */
	cmd = 0x35;
	status = icm42688p_write(dev, ICM42688P_DEF_TMST_CONFIG_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/*
	 * Gyroscope and accelerometer enable/disable.
	 */
	uint8_t gyro_dis;
	uint8_t accel_dis;

	gyro_dis = (cfg->gyro_odr == ICM42688P_GYRO_ODR_0) ? 0x07 : 0x00;
	accel_dis = (cfg->accel_odr == ICM42688P_ACCEL_ODR_0) ? 0x07 : 0x00;

	cmd = 0x80 | (gyro_dis << 3) | accel_dis;
	status = icm42688p_write(dev, ICM42688P_DEF_SENSOR_CONFIG0_REG, cmd);
	if (status != lStatus_Success)
		return status;

	return lStatus_Success;
}

lStatus_t icm42688p_int_cfg(icm42688p_t *dev, icm42688p_int_cfg_t *pin1,
							icm42688p_int_cfg_t *pin2)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cmd;

	/*
	 * Set INT Pulse duration to 8us, disable de-asserts, set int async to 0.
	 */
	cmd = 0x60;
	status = icm42688p_write(dev, ICM42688P_DEF_INT_CONFIG1_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/*
	 * Configure interrupt.
	 */
	cmd = (pin2->int_mode << 5) | (pin2->int_drive << 4) |
		  (pin2->int_polarity << 3) | (pin1->int_mode << 2) |
		  (pin1->int_drive << 1) | pin1->int_polarity;
	status = icm42688p_write(dev, ICM42688P_DEF_INT_CONFIG_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/*
	 * INT Clear on FIFO 1B ready, clear on sensor register read.
	 */
	cmd = 0x2A;
	status = icm42688p_write(dev, ICM42688P_DEF_INT_CONFIG0_REG, cmd);
	if (status != lStatus_Success)
		return status;

	/* Set INT1 Source. */
	if (pin1->int_type != ICM42688P_INT_TYPE_DISABLE) {
		cmd = (1 << pin1->int_type);
		status = icm42688p_write(dev, ICM42688P_DEF_INT_SOURCE0_REG, cmd);
		if (status != lStatus_Success)
			return status;
	}

	/* Set INT2 Source. */
	if (pin2->int_type != ICM42688P_INT_TYPE_DISABLE) {
		cmd = (1 << pin2->int_type);
		status = icm42688p_write(dev, ICM42688P_DEF_INT_SOURCE3_REG, cmd);
		if (status != lStatus_Success)
			return status;
	}

	return lStatus_Success;
}

lStatus_t icm42688p_temp_get(icm42688p_t *dev, float *temp)
{
	lStatus_t status = lStatus_Fail;
	int16_t val;

	status = icm42688p_read(dev, ICM42688P_DEF_TEMP_DATA1_REG, (uint8_t *)&val,
							sizeof(val));
	if (status == lStatus_Success) {
		*temp = (val * ICM42688P_DEF_TEMP_FACTOR_16_BITS) +
				ICM42688P_DEF_TEMP_OFFSET;
	}

	return status;
}

lStatus_t icm42688p_gyro_get(icm42688p_t *dev, float *x, float *y, float *z)
{
	lStatus_t status = lStatus_Fail;
	int16_t values[3];

	status = icm42688p_read(dev, ICM42688P_DEF_GYRO_DATA_X1_REG,
							(uint8_t *)values, sizeof(values));

	if (status == lStatus_Success) {
		*x = values[0] * dev->gyro_sens;
		*y = values[1] * dev->gyro_sens;
		*z = values[2] * dev->gyro_sens;
	}

	return status;
}

lStatus_t icm42688p_accel_get(icm42688p_t *dev, float *x, float *y, float *z)
{
	lStatus_t status = lStatus_Fail;
	int16_t values[3];

	status = icm42688p_read(dev, ICM42688P_DEF_ACCEL_DATA_X1_REG,
							(uint8_t *)values, sizeof(values));

	if (status == lStatus_Success) {
		*x = values[0] * dev->accel_sens;
		*y = values[1] * dev->accel_sens;
		*z = values[2] * dev->accel_sens;
	}

	return status;
}

lStatus_t icm42688p_int_status(icm42688p_t *dev, icm42688p_int_status_t *status)
{
	return icm42688p_read(dev, ICM42688P_DEF_INT_STATUS_REG, (uint8_t *)status,
						  1);
}

/****************************** static functions ******************************/

static lStatus_t icm42688p_read(icm42688p_t *dev, uint16_t reg, uint8_t *vals,
								uint8_t cnt)
{
	uint8_t addr = ICM42688P_DEF_GET_ADDR(reg);
	uint8_t bank = ICM42688P_DEF_GET_BANK(reg);

	if (bank != dev->bank) {
		if (lStatus_Success != icm42688p_change_bank(dev, bank))
			return lStatus_Fail;

		dev->bank = bank;
	}

	return dev->interface->read(dev->interface, reg, vals, cnt);
}

static lStatus_t icm42688p_write(icm42688p_t *dev, uint16_t reg, uint8_t data)
{
	uint8_t addr = ICM42688P_DEF_GET_ADDR(reg);
	uint8_t bank = ICM42688P_DEF_GET_BANK(reg);

	if (bank != dev->bank) {
		if (lStatus_Success != icm42688p_change_bank(dev, bank))
			return lStatus_Fail;

		dev->bank = bank;
	}

	return dev->interface->write(dev->interface, addr, data);
}

static lStatus_t icm42688p_change_bank(icm42688p_t *dev, uint8_t bank)
{
	uint8_t addr = ICM42688P_DEF_GET_ADDR(ICM42688P_DEF_REG_BANK_SEL_REG);

	bank &= 0b00000111;

	return dev->interface->write(dev->interface, addr, bank);
}

static lStatus_t icm42688p_who_am_i(icm42688p_t *dev)
{
	lStatus_t status = lStatus_Fail;
	uint8_t who_am_i;

	status = icm42688p_read(dev, ICM42688P_DEF_WHO_AM_I_REG, &who_am_i,
							sizeof(who_am_i));
	if (status != lStatus_Success || who_am_i != ICM42688P_DEF_WHO_AM_I)
		return lStatus_Fail;

	return lStatus_Success;
}

static lStatus_t icm42688p_reset(icm42688p_t *dev)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cmd;

	cmd = 0x01; // soft reset
	status = icm42688p_write(dev, ICM42688P_DEF_DEVICE_CONFIG_REG, cmd);
	if (status != lStatus_Success)
		return status;

	HAL_DelayMS(ICM42688P_DEF_SOFT_RESET_PERIOD_MS);

	return lStatus_Success;
}

/********************************* End Of File ********************************/