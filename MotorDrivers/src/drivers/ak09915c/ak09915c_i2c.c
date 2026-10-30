/**
 * @file    ak09915c_i2c.c
 * @brief   AK09915C Magnetometer driver I2C Physical layer.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ak09915c_i2c.h"

#include "ak09915c_def.h"

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

void ak09915c_i2c_init(void *interface, i2c_hal_instance_t instance,
					   uint8_t cad0, uint8_t cad1)
{
	ak09915c_i2c_interface_t *i2c;

	i2c = (ak09915c_i2c_interface_t *)interface;

	i2c->instance = instance;
	i2c->slave_addr = (AK09915C_DEF_SLAVE_ADDR | (cad1 << 1) | cad0);
	i2c->interface.write = ak09915c_i2c_write;
	i2c->interface.read = ak09915c_i2c_read;
}

lStatus_t ak09915c_i2c_write(void *interface, uint8_t addr, uint8_t val)
{
	ak09915c_i2c_interface_t *i2c;
	lStatus_t status = lStatus_Fail;
	uint8_t data[2] = {addr, val};

	i2c = (ak09915c_i2c_interface_t *)interface;

	status = HAL_I2C_MasterTransmit(i2c->instance, i2c->slave_addr, data,
									sizeof(data), AK09915C_I2C_TIMEOUT_MS);
	if (status != lStatus_Success)
		return status;

	return lStatus_Success;
}

lStatus_t ak09915c_i2c_read(void *interface, uint8_t addr, uint8_t *val,
							uint32_t cnt)
{
	ak09915c_i2c_interface_t *i2c;
	lStatus_t status = lStatus_Fail;

	i2c = (ak09915c_i2c_interface_t *)interface;

	status = HAL_I2C_MasterTransmit(i2c->instance, i2c->slave_addr, &addr,
									sizeof(addr), AK09915C_I2C_TIMEOUT_MS);
	if (status != lStatus_Success)
		return status;

	status = HAL_I2C_MasterReceive(i2c->instance, i2c->slave_addr, val, cnt,
								   AK09915C_I2C_TIMEOUT_MS);
	if (status != lStatus_Success)
		return status;

	return lStatus_Success;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/