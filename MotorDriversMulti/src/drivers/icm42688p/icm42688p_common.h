/**
 * @file    icm42688p_common.h
 * @brief   ICM42688P Accelerometer & Gyroscope common types.
 * @version	1.0.0
 * @date    23.01.2025
 * @author  LisumLab
 */

#ifndef ICM42688P_COMMON_H
#define ICM42688P_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

#ifdef ICM42688P_CONFIG
#include "icm42688p_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief ICM42688P communication interface. */
typedef struct icm42688p_interface_t {
	// Write register.
	lStatus_t (*write)(void *interface, uint8_t addr, uint8_t data);

	// Read registers.
	lStatus_t (*read)(void *interface, uint8_t addr, uint8_t *val,
					  uint32_t cnt);
} icm42688p_interface_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ICM42688P_COMMON_H */