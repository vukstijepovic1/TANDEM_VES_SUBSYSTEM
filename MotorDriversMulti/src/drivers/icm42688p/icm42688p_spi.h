/**
 * @file    icm42688p_spi.h
 * @brief   ICM42688P Accelerometer & Gyroscope Driver SPI Physical Layer.
 * @version	1.0.0
 * @date    23.01.2025
 * @author  LisumLab
 */

#ifndef ICM42688P_SPI_H
#define ICM42688P_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "spi.h"

#include "icm42688p_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef ICM42688P_SPI_TIMEOUT_MS
#define ICM42688P_SPI_TIMEOUT_MS 10
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief ICM42688P SPI communication interface. */
typedef struct icm42688p_spi_interface_t {
	icm42688p_interface_t interface; // Communication interface.

	spi_hal_instance_t instance; // SPI Instance.
	spi_hal_slave_id_t id;		 // SPI Slave ID.
} icm42688p_spi_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize ICM42688P SPI communication interface.
 *
 * @param[in] interface ICM42688P SPI interface.
 * @param[in] instance  SPI Instance.
 * @param[in] id        SPI Slave ID.
 * @return None.
 */
void icm42688p_spi_init(void *interface, spi_hal_instance_t instance,
						spi_hal_slave_id_t id);

/**
 * @brief Write data into ICM42688P Register
 *
 * @param[in] interface ICM42688P SPI interface.
 * @param[in] addr      Register Address.
 * @param[in] data      Data to be written into register.
 * @return Operation status.
 */
lStatus_t icm42688p_spi_write(void *interface, uint8_t addr, uint8_t data);

/**
 * @brief Read data from ICM42688P Register
 *
 * @param[in] interface ICM42688P SPI interface.
 * @param[in] addr      Register address.
 * @param[out] val      Register data.
 * @param[in] cnt       Register cnt.
 * @return Operation status.
 */
lStatus_t icm42688p_spi_read(void *interface, uint8_t addr, uint8_t *val,
							 uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* ICM42688P_SPI_H */