/**
 * @file    ms5607_spi.h
 * @brief   MS5607 Barometer driver SPI Physical layer.
 * @version 1.1.0
 * @date    29.01.2025
 * @author  LisumLab
 */

#ifndef MS5607_SPI_H
#define MS5607_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"
#include "spi.h"

#include "ms5607_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef MS5607_SPI_TIMEOUT_MS
#define MS5607_SPI_TIMEOUT_MS 10
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief MS5607 SPI communication interface. */
typedef struct ms5607_spi_interface_t {
	ms5607_interface_t interface; // Communication interface.

	spi_hal_instance_t instance; // SPI Instance.
	spi_hal_slave_id_t id;		 // SPI Slave ID.
} ms5607_spi_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize MS5607 SPI communication interface.
 *
 * @param[in] interface MS5607 SPI interface.
 * @param[in] instance  SPI Instance.
 * @param[in] id        SPI Slave ID.
 * @return None.
 */
void ms5607_spi_init(void *interface, spi_hal_instance_t instance,
					 spi_hal_slave_id_t id);

/**
 * @brief Send command to MS5607.
 *
 * @param[in] interface MS5607 SPI interface.
 * @param[in] cmd       Command.
 * @return Operation status.
 */
lStatus_t ms5607_spi_send_cmd(void *interface, uint8_t cmd);

/**
 * @brief Read MS5607 register.
 *
 * @param[in] interface MS5607 SPI interface.
 * @param[in] addr      Register address.
 * @param[out] val      Register data.
 * @param[in] cnt       Register cnt.
 * @return Operation status.
 */
lStatus_t ms5607_spi_read(void *interface, uint8_t addr, uint8_t *val,
						  uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* MS5607_SPI_H */