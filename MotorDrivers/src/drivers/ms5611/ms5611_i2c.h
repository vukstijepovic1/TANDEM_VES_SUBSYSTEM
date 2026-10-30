/**
 * @file    ms5611_i2c.h
 * @brief   MS5611 Barometer driver I2C Physical layer.
 * @version	1.1.0
 * @date    28.01.2025
 * @author  LisumLab
 */

#ifndef MS5611_I2C_H
#define MS5611_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"
#include "i2c.h"

#include "ms5611_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef MS5611_I2C_TIMEOUT_MS
#define MS5611_I2C_TIMEOUT_MS 10
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief MS5611 I2C communication interface. */
typedef struct ms5611_i2c_interface_t {
	ms5611_interface_t interface; // Communication interface.

	i2c_hal_instance_t instance; // I2C Instance.
	uint8_t slave_addr;			 // I2C 7bit slave address.
} ms5611_i2c_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize MS5611 I2C communication interface.
 *
 * @param[in] interface MS5611 I2C interface.
 * @param[in] instance  I2C Instance.
 * @param[in] csb       MS5611 CSB pin status (used to determine slave addr).
 * @return None.
 */
void ms5611_i2c_init(void *interface, i2c_hal_instance_t instance, uint8_t csb);

/**
 * @brief Send command to MS5611.
 *
 * @param[in] interface MS5611 I2C interface.
 * @param[in] cmd       Command.
 * @return Operation status.
 */
lStatus_t ms5611_i2c_send_cmd(void *interface, uint8_t cmd);

/**
 * @brief Read MS5611 registers.
 *
 * @param[in] interface MS5611 I2C interface.
 * @param[in] addr      Register address.
 * @param[out] val      Register data.
 * @param[in] cnt       Register cnt.
 * @return Operation status.
 */
lStatus_t ms5611_i2c_read(void *interface, uint8_t addr, uint8_t *val,
						  uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* MS5611_I2C_H */