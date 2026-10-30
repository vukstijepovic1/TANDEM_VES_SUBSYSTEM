/**
 * @file    ak09915c_i2c.h
 * @brief   AK09915C Magnetometer driver I2C Physical layer.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

#ifndef AK09915C_I2C_H
#define AK09915C_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"
#include "i2c.h"

#include "ak09915c_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef AK09915C_I2C_TIMEOUT_MS
#define AK09915C_I2C_TIMEOUT_MS 10
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief AK09915C I2C communication interface. */
typedef struct ak09915c_i2c_interface_t {
	ak09915c_interface_t interface; // Communication interface.

	i2c_hal_instance_t instance; // I2C Instance.
	uint8_t slave_addr;			 // I2C 7bit slave address.
} ak09915c_i2c_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize AK09915C I2C communication interface.
 *
 * @param[in] interface AK09915C I2C interface.
 * @param[in] instance  I2C Instance.
 * @param[in] cad2      AK09915C CAD2 pin state (used to determine slave addr).
 * @param[in] cad1      AK09915C CAD1 pin state (used to determine slave addr).
 * @return None.
 */
void ak09915c_i2c_init(void *interface, i2c_hal_instance_t instance,
					   uint8_t cad0, uint8_t cad1);

/**
 * @brief Write into AK09915C register.
 *
 * @param[in] interface AK09915C I2C interface.
 * @param[in] addr      Register address.
 * @param[in] val       Register value.
 * @return Write status.
 */
lStatus_t ak09915c_i2c_write(void *interface, uint8_t addr, uint8_t val);

/**
 * @brief Read AK09915C registers.
 *
 * @param[in] interface AK09915C I2C interface.
 * @param[in] addr      Register address.
 * @param[out] val      Register data.
 * @param[in] cnt       Register cnt.
 * @return Read status.
 */
lStatus_t ak09915c_i2c_read(void *interface, uint8_t addr, uint8_t *val,
							uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* AK09915C_I2C_H */