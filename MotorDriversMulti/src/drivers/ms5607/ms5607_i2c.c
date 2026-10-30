/**
 * @file    ms5607_i2c.c
 * @brief   MS5607 Barometer driver I2C Physical layer.
 * @version 1.1.0
 * @date    29.01.2025
 * @author 	LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ms5607_i2c.h"
#include "ms5607_def.h"

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

void ms5607_i2c_init(void *interface, i2c_hal_instance_t instance, uint8_t csb)
{
	ms5607_i2c_interface_t *i2c;

	i2c = (ms5607_i2c_interface_t *)interface;

	i2c->instance = instance;
	i2c->slave_addr = MS5607_DEF_SLAVE_ADDR | !csb;
	i2c->interface.send_cmd = ms5607_i2c_send_cmd;
	i2c->interface.read = ms5607_i2c_read;
}

lStatus_t ms5607_i2c_send_cmd(void *interface, uint8_t cmd)
{
	ms5607_i2c_interface_t *i2c;
	lStatus_t status = lStatus_Fail;

	i2c = (ms5607_i2c_interface_t *)interface;

	status = HAL_I2C_MasterTransmit(i2c->instance, i2c->slave_addr, &cmd,
									sizeof(cmd), MS5607_I2C_TIMEOUT_MS);

	return status;
}

lStatus_t ms5607_i2c_read(void *interface, uint8_t addr, uint8_t *val,
						  uint32_t cnt)
{
	ms5607_i2c_interface_t *i2c;
	lStatus_t status = lStatus_Fail;

	i2c = (ms5607_i2c_interface_t *)interface;

	status = HAL_I2C_MasterTransmit(i2c->instance, i2c->slave_addr, &addr,
									sizeof(addr), MS5607_I2C_TIMEOUT_MS);

	if (status == lStatus_Success) {
		status = HAL_I2C_MasterReceive(i2c->instance, i2c->slave_addr, val, cnt,
									   MS5607_I2C_TIMEOUT_MS);
	}

	return status;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/