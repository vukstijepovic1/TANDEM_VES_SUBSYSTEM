/**
 * @file    ms5607_common.h
 * @brief   MS5607 Barometer driver common types.
 * @version 1.1.0
 * @date    29.01.2025
 * @author  LisumLab
 */

#ifndef MS5607_COMMON_H
#define MS5607_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

#ifdef MS5607_CONFIG
#include "ms5607_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief MS5607 communication interface. */
typedef struct ms5607_interface_t {
	// Send command.
	lStatus_t (*send_cmd)(void *interface, uint8_t cmd);

	// Read registers.
	lStatus_t (*read)(void *interface, uint8_t addr, uint8_t *val,
					  uint32_t cnt);
} ms5607_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MS5607_COMMON_H */