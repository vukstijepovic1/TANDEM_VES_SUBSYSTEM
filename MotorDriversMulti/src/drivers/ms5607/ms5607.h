/**
 * @file    ms5607.h
 * @brief   MS5607 Barometer driver.
 * @version 1.1.0
 * @date    29.01.2025
 * @author  LisumLab
 */

#ifndef MS5607_H
#define MS5607_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

#include "ms5607_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief Measurement type enum. */
typedef enum ms5607_meas_t {
	MS5607_MEAS_NONE = 0,
	MS5607_MEAS_PRESSURE,
	MS5607_MEAS_TEMPERATURE
} ms5607_meas_t;

/*! @brief MS5607 Oversampling Ratio. */
typedef enum ms5607_osr_t {
	MS5607_OSR_256 = 0, //!< Oversampling Ratio 256
	MS5607_OSR_512,		//!< Oversampling Ratio 512
	MS5607_OSR_1024,	//!< Oversampling Ratio 1024
	MS5607_OSR_2048,	//!< Oversampling Ratio 2048
	MS5607_OSR_4096		//!< Oversampling Ratio 4096
} ms5607_osr_t;

/*! @brief MS5607 PROM. */
typedef struct ms5607_prom_t {
	uint16_t res; //!< 16 bit reserved for the manufacturer
	uint16_t c1;  //!< Pressure Sensitivity
	uint16_t c2;  //!< Pressure Offset
	uint16_t c3;  //!< Temperature Coefficient of Pressure Sensitivity
	uint16_t c4;  //!< Temperature Coefficient of Pressure Offset
	uint16_t c5;  //!< Reference Temperature
	uint16_t c6;  //!< Temperature Coefficient of the Temperature
	uint16_t crc; //!< Dummy bits + PROM CRC4
} ms5607_prom_t;

/*! @brief MS5607 PROM Union. */
typedef union ms5607_prom_u {
	ms5607_prom_t s;
	uint16_t c[8];
} ms5607_prom_u;

/*! @brief MS5607 Device. */
typedef struct ms5607_t {
	ms5607_interface_t *interface; //!< Communication interface.

	ms5607_prom_u prom;		 //!< PROM
	ms5607_meas_t curr_meas; //!< Ongoing conversion type.

	int64_t OFF;  //!< Data used for internal pressure calculation
	int64_t SENS; //!< Data used for internal pressure calculation

} ms5607_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize ms5607 pressure sensor.
 *
 * @param[in] dev        MS5607 device.
 * @param[in] interface  Communication interface:
 *                           ms5607_i2c_interface_t or ms5607_spi_interface_t
 * @return Initialization status.
 */
lStatus_t ms5607_init(ms5607_t *dev, ms5607_interface_t *interface);

/**
 * @brief Request start of pressure/temperature conversion (start measurement).
 *
 * @param[in] dev    MS5607 device.
 * @param[in] meas   Measurement type:
 *                       MS5607_MEAS_TEMPERATURE or MS5607_MEAS_PRESSURE.
 * @param[in] osr    Oversampling ratio.
 * @param[out] t_us  Conversion period.
 * @return Operation status.
 */
lStatus_t ms5607_measure(ms5607_t *dev, ms5607_meas_t meas, ms5607_osr_t osr,
						 uint32_t *t_us);

/**
 * @brief Read converted pressure/temperature (measured pressure/temperature).
 *
 * @param[in] dev    MS5607 device.
 * @param[in] meas   Measurement type:
 *						MS5607_MEAS_TEMPERATURE or MS5607_MEAS_PRESSURE.
 * @param[out] val   Measured value (pressure/temperature).
 * @return Operation status.
 */
lStatus_t ms5607_collect(ms5607_t *dev, ms5607_meas_t meas, float *val);

#ifdef __cplusplus
}
#endif

#endif /* MS5607_H */