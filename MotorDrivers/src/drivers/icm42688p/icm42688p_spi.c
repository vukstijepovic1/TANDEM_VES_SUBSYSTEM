/**
 * @file    icm42688p_spi.c
 * @brief   ICM42688P Accelerometer & Gyroscope Driver SPI Physical Layer.
 * @version	1.0.0
 * @date    23.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "icm42688p_spi.h"

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

void icm42688p_spi_init(void *interface, spi_hal_instance_t instance,
						spi_hal_slave_id_t id)
{
	icm42688p_spi_interface_t *spi;

	spi = (icm42688p_spi_interface_t *)interface;

	spi->instance = instance;
	spi->id = id;
	spi->interface.write = icm42688p_spi_write;
	spi->interface.read = icm42688p_spi_read;
}

lStatus_t icm42688p_spi_write(void *interface, uint8_t addr, uint8_t data)
{
	icm42688p_spi_interface_t *spi;
	lStatus_t status = lStatus_Fail;
	uint8_t cmd[2] = {(0 << 7) | addr, data};

	spi = (icm42688p_spi_interface_t *)interface;

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_RESET);
	status = HAL_SPI_Transmit(spi->instance, cmd, sizeof(cmd),
							  ICM42688P_SPI_TIMEOUT_MS);
	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_SET);

	return status;
}

lStatus_t icm42688p_spi_read(void *interface, uint8_t addr, uint8_t *val,
							 uint32_t cnt)
{
	icm42688p_spi_interface_t *spi;
	lStatus_t status = lStatus_Fail;
	uint8_t cmd = (1 << 7) | addr;

	spi = (icm42688p_spi_interface_t *)interface;

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_RESET);

	status = HAL_SPI_Transmit(spi->instance, &cmd, sizeof(cmd),
							  ICM42688P_SPI_TIMEOUT_MS);
	if (status == lStatus_Success) {
		status =
			HAL_SPI_Receive(spi->instance, val, cnt, ICM42688P_SPI_TIMEOUT_MS);
	}

	HAL_SPI_SlavePinState(spi->instance, spi->id, SPI_HAL_SLAVE_PIN_SET);

	return status;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/