/**
 * @file    ms5611_common.h
 * @brief   MS5611 Barometer driver common types.
 * @version 1.1.0
 * @date    28.01.2025
 * @author  LisumLab
 */

#ifndef MS5611_COMMON_H
#define MS5611_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

#ifdef MS5611_CONFIG
#include "ms5611_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief MS5611 communication interface. */
typedef struct ms5611_interface_t {
	// Send command.
	lStatus_t (*send_cmd)(void *interface, uint8_t cmd);

	// Read registers.
	lStatus_t (*read)(void *interface, uint8_t addr, uint8_t *val,
					  uint32_t cnt);
} ms5611_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MS5611_COMMON_H */