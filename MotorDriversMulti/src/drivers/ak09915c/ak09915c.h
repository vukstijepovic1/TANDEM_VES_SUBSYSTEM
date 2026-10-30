/**
 * @file    ak09915c.h
 * @brief   AK09915C Magnetometer driver.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

#ifndef AK09915C_H
#define AK09915C_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <assert.h>
#include <stdint.h>

#include "common.h"
#include "util.h"

#include "ak09915c_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief AK09915C Operational mode setting. */
typedef enum ak09915c_op_mode_t {
	AK09915C_OP_MODE_PWR_DOWN = 0,	   // No measurements are made
	AK09915C_OP_MODE_SINGLE_MEAS = 1,  // Single measurement is performed
	AK09915C_OP_MODE_CONTINUOUS1 = 2,  // Cyclic 10Hz measurements
	AK09915C_OP_MODE_CONTINUOUS2 = 4,  // Cyclic 20Hz measurements
	AK09915C_OP_MODE_CONTINUOUS3 = 6,  // Cyclic 50Hz measurements
	AK09915C_OP_MODE_CONTINUOUS4 = 8,  // Cyclic 100Hz measurements
	AK09915C_OP_MODE_CONTINUOUS5 = 10, // Cyclic 200Hz measurements
	AK09915C_OP_MODE_CONTINUOUS6 = 12, // Cyclic 1Hz measurements
	AK09915C_OP_MODE_ST = 16,		   // Self-Test mode
} ak09915c_op_mode_t;

/*! @brief AK09915C Sensor drive setting. */
typedef enum ak09915c_sdr_t {
	AK09915C_SDR_LOW_POWER = 0,
	AK09915C_SDR_LOW_NOISE = 1
} ak09915c_sdr_t;

/*! @brief AK09915C Configuration. */
typedef struct ak09915c_cfg_t {
	ak09915c_op_mode_t op_mode;	   // Operational mode.
	lFunctionalState_t noise_filt; // Noise filter configuration.
	ak09915c_sdr_t sdr;			   // Sensor drive.
} ak09915c_cfg_t;

/*! @brief AK09915C Device. */
typedef struct ak09915c_t {
	ak09915c_interface_t *interface; // AK09915C communication interface.
} ak09915c_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize AK09915C Magnetometer
 *
 * @param[in] dev       AK09915C device.
 * @param[in] interface Communication interface:
 *                          ak09915c_i2c or ak09915c_spi
 * @param[in] cfg       AK09915C device configuration.
 * @return Initialization status.
 */
lStatus_t ak09915c_init(ak09915c_t *dev, ak09915c_interface_t *interface,
						ak09915c_cfg_t *cfg);

/**
 * @brief Check if AK09915C Data is ready.
 *
 * @param[in] dev   AK09915C Device.
 * @return Data Status:
 *          0 - Data isn't ready.
 *          1 - Data is ready.
 */
uint8_t ak09915c_drdy(ak09915c_t *dev);

/**
 * @brief Get AK09915C sample.
 *
 * @param[in] dev   AK09915C Device.
 * @param[out] x    X axis magnetometer reading [uT].
 * @param[out] y    Y axis magnetometer reading [uT].
 * @param[out] z    Z axis magnetometer reading [uT].
 * @return Data read status.
 */
lStatus_t ak09915c_sample_get(ak09915c_t *dev, float *x, float *y, float *z);

#ifdef __cplusplus
}
#endif

#endif /* AK09915C_H */