/**
 * @file    ms5607_spi.c
 * @brief   MS5607 Barometer driver SPI Physical layer.
 * @version 1.1.0
 * @date    29.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ms5607_spi.h"

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

void ms5607_spi_init(void *interface, spi_hal_instance_t instance,
					 spi_hal_slave_id_t id)
{
	ms5607_spi_interface_t *spi;

	spi = (ms5607_spi_interface_t *)interface;

	spi->instance = instance;
	spi->id = id;
	spi->interface.send_cmd = ms5607_spi_send_cmd;
	spi->interface.read = ms5607_spi_read;
}

lStatus_t ms5607_spi_send_cmd(void *interface, uint8_t cmd)
{
	ms5607_spi_interface_t *spi;
	lStatus_t status = lStatus_Fail;

	spi = (ms5607_spi_interface_t *)interface;

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_RESET);
	status = HAL_SPI_Transmit(spi->instance, &cmd, sizeof(cmd),
							  MS5607_SPI_TIMEOUT_MS);
	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_SET);

	return status;
}

lStatus_t ms5607_spi_read(void *interface, uint8_t addr, uint8_t *val,
						  uint32_t cnt)
{
	ms5607_spi_interface_t *spi;
	lStatus_t status = lStatus_Fail;

	spi = (ms5607_spi_interface_t *)interface;

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_RESET);

	status = HAL_SPI_Transmit(spi->instance, &addr, sizeof(addr),
							  MS5607_SPI_TIMEOUT_MS);
	if (status == lStatus_Success) {
		status =
			HAL_SPI_Receive(spi->instance, val, cnt, MS5607_SPI_TIMEOUT_MS);
	}

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_SET);

	return status;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/