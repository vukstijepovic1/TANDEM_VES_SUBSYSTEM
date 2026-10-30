/**
 * @file    ak09915c_spi.h
 * @brief   AK09915C Magnetometer driver SPI Physical layer.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

#ifndef AK09915C_SPI_H
#define AK09915C_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"
#include "spi.h"

#include "ak09915c_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef AK09915C_SPI_TIMEOUT_MS
#define AK09915C_SPI_TIMEOUT_MS 10
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief AK09915C SPI communication interface. */
typedef struct ak09915c_spi_interface_t {
	ak09915c_interface_t interface; // Communication interface.

	spi_hal_instance_t instance; // SPI Instance.
	spi_hal_slave_id_t id;		 // SPI Slave ID.
} ak09915c_spi_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize AK09915C SPI communication interface.
 *
 * @param[in] interface AK09915C SPI interface.
 * @param[in] instance  SPI Instance.
 * @param[in] id        SPI Slave ID.
 * @return None.
 */
void ak09915c_spi_init(void *interface, spi_hal_instance_t instance,
					   spi_hal_slave_id_t id);

/**
 * @brief Write into AK09915C register.
 *
 * @param[in] interface AK09915C SPI interface.
 * @param[in] addr      Register address.
 * @param[in] val       Register value.
 * @return Write status.
 */
lStatus_t ak09915c_spi_write(void *interface, uint8_t addr, uint8_t val);

/**
 * @brief Read AK09915C register.
 *
 * @param[in] interface AK09915C SPI interface.
 * @param[in] addr      Register address.
 * @param[out] val      Register data.
 * @param[in] cnt       Register cnt.
 * @return Read status.
 */
lStatus_t ak09915c_spi_read(void *interface, uint8_t addr, uint8_t *val,
							uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* AK09915C_SPI_H */