/**
 * @file    ak09915c_spi.c
 * @brief   AK09915C Magnetometer driver SPI Physical layer.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ak09915c_spi.h"

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

void ak09915c_spi_init(void *interface, spi_hal_instance_t instance,
					   spi_hal_slave_id_t id)
{
	ak09915c_spi_interface_t *spi;

	spi = (ak09915c_spi_interface_t *)interface;

	spi->instance = instance;
	spi->id = id;
	spi->interface.write = ak09915c_spi_write;
	spi->interface.read = ak09915c_spi_read;
}

lStatus_t ak09915c_spi_write(void *interface, uint8_t addr, uint8_t val)
{
	ak09915c_spi_interface_t *spi;
	lStatus_t status = lStatus_Fail;
	uint8_t data[2] = {addr & 0x7F, val};

	spi = (ak09915c_spi_interface_t *)interface;

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_RESET);
	status = HAL_SPI_Transmit(spi->instance, data, sizeof(data),
							  AK09915C_SPI_TIMEOUT_MS);
	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_SET);

	return status;
}

lStatus_t ak09915c_spi_read(void *interface, uint8_t addr, uint8_t *val,
							uint32_t cnt)
{
	ak09915c_spi_interface_t *spi;
	lStatus_t status = lStatus_Fail;
	uint8_t cmd = addr | (1 << 7);

	spi = (ak09915c_spi_interface_t *)interface;

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_RESET);

	status = HAL_SPI_Transmit(spi->instance, &cmd, sizeof(cmd),
							  AK09915C_SPI_TIMEOUT_MS);
	if (status == lStatus_Success) {
		status =
			HAL_SPI_Receive(spi->instance, val, cnt, AK09915C_SPI_TIMEOUT_MS);
	}

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_SET);

	return status;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/