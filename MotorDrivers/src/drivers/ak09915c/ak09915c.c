/**
 * @file    ak09915c.c
 * @brief   AK09915C Magnetometer driver.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ak09915c.h"

#include "ak09915c_def.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

// Check DRDY bit of Status 1 Register.
#define AK09915C_CHECK_DRDY_FLAG(x) ((x >> 0) & 0x01)
// Check DOR bit of Status 1 Register.
#define AK09915C_CHECK_DOR_FLAG(x) ((x >> 1) & 0x01)

// Check INV bit of Status 2 Register.
#define AK09915C_CHECK_INV_FLAG(x) ((x >> 2) & 0x01)
// Check HOFL bit of Status 2 Register.
#define AK09915C_CHECK_HOFL_FLAG(x) ((x >> 3) & 0x01)
// Check if operation mode is continuos.
#define AK09915C_IS_MODE_CONTINUOUS(x)                                         \
	(x == AK09915C_OP_MODE_CONTINUOUS1 || x == AK09915C_OP_MODE_CONTINUOUS2 ||             \
	 x == AK09915C_OP_MODE_CONTINUOUS3 || x == AK09915C_OP_MODE_CONTINUOUS4 ||             \
	 x == AK09915C_OP_MODE_CONTINUOUS5 || x == AK09915C_OP_MODE_CONTINUOUS6)

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Write value into AK09915C register. */
static lStatus_t ak09915c_write(ak09915c_t *dev, uint8_t addr, uint8_t val);

/* Read value from AK09915C register. */
static lStatus_t ak09915c_read(ak09915c_t *dev, uint8_t addr, uint8_t *val,
                               uint32_t cnt);

/* Check values in AK09915C Company ID and Device ID registers. */
static lStatus_t ak09915c_who_am_i(ak09915c_t *dev);

/* Soft reset AK09915C. */
static lStatus_t ak09915c_soft_reset(ak09915c_t *dev);

/* Set AK09915C mode of operation. */
static lStatus_t ak09915c_set_mode(ak09915c_t *dev, ak09915c_op_mode_t mode);

/* Get AK09915C mode of operation. */
static lStatus_t ak09915c_get_mode(ak09915c_t *dev, ak09915c_op_mode_t *mode);

/* Set AK09915C Sensor drive setting. */
static lStatus_t ak09915c_set_sdr(ak09915c_t *dev, ak09915c_sdr_t sdr);

/* Set AK09915C noise supression filter setting. */
static lStatus_t ak09915c_set_nsf(ak09915c_t *dev, lFunctionalState_t nsf);

/* Fetch sample. */
static lStatus_t ak09915c_sample_fetch(ak09915c_t *dev, int16_t *x, int16_t *y,
                                       int16_t *z);

/*******************************************************************************
 * Code
 ******************************************************************************/

lStatus_t ak09915c_init(ak09915c_t *dev, ak09915c_interface_t *interface,
						ak09915c_cfg_t *cfg)
{
	lStatus_t status = lStatus_Fail;

	dev->interface = interface;

	// Check if AK0915C is available.
	status = ak09915c_who_am_i(dev);
	if (status != lStatus_Success)
		return status;

	// Perform Soft Reset.
	status = ak09915c_soft_reset(dev);
	if (status != lStatus_Success)
		return status;

	// Set sensor Sensor Drive.
	status = ak09915c_set_sdr(dev, cfg->sdr);
	if (status != lStatus_Success)
		return status;

	// Set sensor noise supression filter.
	status = ak09915c_set_nsf(dev, cfg->noise_filt);
	if (status != lStatus_Success)
		return status;

	// Set sensor operation mode.
	status = ak09915c_set_mode(dev, cfg->op_mode);
	if (status != lStatus_Success)
		return status;

	return lStatus_Success;
}

uint8_t ak09915c_drdy(ak09915c_t *dev)
{
	lStatus_t status = lStatus_Fail;
	uint8_t st1;

	// Read Status 1 register.
	status = ak09915c_read(dev, AK09915C_DEF_REG_ST1, &st1, sizeof(st1));
	if (status == lStatus_Success && AK09915C_CHECK_DRDY_FLAG(st1))
		return 1;
	else
		return 0;
}

lStatus_t ak09915c_sample_get(ak09915c_t *dev, float *x, float *y, float *z)
{
	lStatus_t status = lStatus_Fail;
	int16_t raw_x, raw_y, raw_z;

    status = ak09915c_sample_fetch(dev, &raw_x, &raw_y, &raw_z);
    if (status != lStatus_Success)
        return status;

	*x = raw_x * AK09915C_DEF_SENSITIVITY;
	*y = raw_y * AK09915C_DEF_SENSITIVITY;
	*z = raw_z * AK09915C_DEF_SENSITIVITY;

	return lStatus_Success;
}

/****************************** static functions ******************************/

static lStatus_t ak09915c_write(ak09915c_t *dev, uint8_t addr, uint8_t val)
{
	return dev->interface->write(dev->interface, addr, val);
}

static lStatus_t ak09915c_read(ak09915c_t *dev, uint8_t addr, uint8_t *val,
                               uint32_t cnt)
{
	return dev->interface->read(dev->interface, addr, val, cnt);
}

static lStatus_t ak09915c_who_am_i(ak09915c_t *dev)
{
	lStatus_t status = lStatus_Fail;
	uint8_t company_id = 0;
	uint8_t dev_id = 0;

	// Read Company ID.
	status = ak09915c_read(dev, AK09915C_DEF_REG_WIA1, &company_id,
						   sizeof(company_id));
	if (status != lStatus_Success)
		return status;

	// Read Device ID.
	status = ak09915c_read(dev, AK09915C_DEF_REG_WIA2, &dev_id, sizeof(dev_id));
	if (status != lStatus_Success)
		return status;

	// Check Company and Device ID.
	if (company_id == AK09915C_DEF_COMPANY_ID &&
		dev_id == AK09915C_DEF_DEVICE_ID)
		return lStatus_Success;
	else
		return lStatus_Fail;
}

static lStatus_t ak09915c_soft_reset(ak09915c_t *dev)
{
	lStatus_t status = lStatus_Fail;

	// Request AK09915C Soft Reset.
	status = ak09915c_write(dev, AK09915C_DEF_REG_CNTL3, 0x01);
	if (status != lStatus_Success)
		return status;

	HAL_DelayUS(AK09915C_DEF_TWAIT_US);

	return lStatus_Success;
}

static lStatus_t ak09915c_set_mode(ak09915c_t *dev, ak09915c_op_mode_t mode)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cntl2;

	// Get content of CNTL2 Register.
	status = ak09915c_read(dev, AK09915C_DEF_REG_CNTL2, &cntl2, sizeof(cntl2));
	if (status != lStatus_Success)
		return status;

	// Go to Power Down operational mode first.
	cntl2 &= 0xE0;

	HAL_DelayUS(AK09915C_DEF_TWAIT_US);

    if (mode == AK09915C_OP_MODE_PWR_DOWN)
        return lStatus_Success;

	status = ak09915c_write(dev, AK09915C_DEF_REG_CNTL2, cntl2);
	if (status != lStatus_Success)
		return status;

	cntl2 |= mode;

	// Enter requested operational mode.
	status = ak09915c_write(dev, AK09915C_DEF_REG_CNTL2, cntl2);
	if (status != lStatus_Success)
		return status;

	return lStatus_Success;
}

static lStatus_t ak09915c_get_mode(ak09915c_t *dev, ak09915c_op_mode_t *mode)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cntl2;

	// Get content of CNTL2 Register.
	status = ak09915c_read(dev, AK09915C_DEF_REG_CNTL2, &cntl2, sizeof(cntl2));
	if (status != lStatus_Success)
		return status;

	*mode = cntl2 & 0x1F;

	return lStatus_Success;
}

static lStatus_t ak09915c_set_sdr(ak09915c_t *dev, ak09915c_sdr_t sdr)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cntl2;

	HAL_DelayUS(AK09915C_DEF_TWAIT_US);

	// Get content of CNTL2 Register.
	status = ak09915c_read(dev, AK09915C_DEF_REG_CNTL2, &cntl2, sizeof(cntl2));
	if (status != lStatus_Success)
		return status;

	cntl2 &= !(1 << 6);
	cntl2 |= (sdr << 6);

	// Write new value of CNTL2 Register.
	status = ak09915c_write(dev, AK09915C_DEF_REG_CNTL2, cntl2);
	if (status != lStatus_Success)
		return status;

	return lStatus_Success;
}

static lStatus_t ak09915c_set_nsf(ak09915c_t *dev, lFunctionalState_t nsf)
{
	lStatus_t status = lStatus_Fail;
	uint8_t cntl1;

	// Get content of CNTL1 Register.
	status = ak09915c_read(dev, AK09915C_DEF_REG_CNTL1, &cntl1, sizeof(cntl1));
	if (status != lStatus_Success)
		return status;

	cntl1 &= !(1 << 5);
	cntl1 |= (nsf << 5);

	// Write new value of CNTL1 Register.
	status = ak09915c_write(dev, AK09915C_DEF_REG_CNTL1, cntl1);
	if (status != lStatus_Success)
		return status;

	return lStatus_Success;
}

static lStatus_t ak09915c_sample_fetch(ak09915c_t *dev, int16_t *x, int16_t *y,
                                       int16_t *z)
{
	lStatus_t status = lStatus_Fail;
	uint8_t raw_val[6];
	uint8_t st1 = 0;
	uint8_t st2 = 0;

	// Read Status 1 Register.
	status = ak09915c_read(dev, AK09915C_DEF_REG_ST1, &st1, sizeof(st1));
	if (status != lStatus_Success)
		return status;

	// Read data sample.
	status = ak09915c_read(dev, AK09915C_DEF_REG_HXL, raw_val, sizeof(raw_val));
	if (status != lStatus_Success)
		return status;

	// Read Status 2 Register.
	status = ak09915c_read(dev, AK09915C_DEF_REG_ST2, &st2, sizeof(st2));
	if (status != lStatus_Success || AK09915C_CHECK_INV_FLAG(st2) ||
		AK09915C_CHECK_HOFL_FLAG(st2) || !AK09915C_CHECK_DRDY_FLAG(st1))
		return lStatus_Fail;

	// Convert data.
	int16_t tmp;

	*x = ((int16_t)raw_val[1] << 8 | raw_val[0]);
	*y = ((int16_t)raw_val[3] << 8 | raw_val[2]);
	*z = ((int16_t)raw_val[5] << 8 | raw_val[4]);

	return lStatus_Success;
}

/********************************* End Of File ********************************/