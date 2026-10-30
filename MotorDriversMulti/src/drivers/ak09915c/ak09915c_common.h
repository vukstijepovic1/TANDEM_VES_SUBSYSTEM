/**
 * @file    ak09915c_common.h
 * @brief   AK09915C Magnetometer driver common types.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

#ifndef AK09915C_COMMON_H
#define AK09915C_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

#ifdef AK09915C_CONFIG
#include "ak09915c_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/* AK09915C communication interface. */
typedef struct ak09915c_interface_t {
	// Write register.
	lStatus_t (*write)(void *interface, uint8_t addr, uint8_t val);

	// Read registers.
	lStatus_t (*read)(void *interface, uint8_t addr, uint8_t *val,
					  uint32_t cnt);
} ak09915c_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* AK09915C_COMMON_H */