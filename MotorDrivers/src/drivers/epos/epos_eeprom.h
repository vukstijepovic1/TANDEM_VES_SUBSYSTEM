/**
 * @file    epos_common.h
 * @brief   EPOS EEPROM.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

#ifndef EPOS_EEPROM_H
#define EPOS_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "epos_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief EPOS4 custom EEPROM. */
typedef struct epos_custom_eeprom_t {
	uint32_t param1;
	uint32_t param2;
	uint32_t param3;
	uint32_t param4;
} epos_custom_eeprom_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Save Current EPOS4 configuration.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_eeprom_save(epos_t *epos, uint8_t epos_id);

/**
 * @brief Restore Default EPOS4 configuration.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_eeprom_restore(epos_t *epos, uint8_t epos_id);

/**
 * @brief Write into EPOS4 custom EEPROM.
 * @note In order to retain data in custom EEPROM user must use epos_eeprom_save
 *       after writing into custom EEPROM
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] rx        EPOS4 custom EEPROM.
 * @return status.
 */
uint32_t epos_custom_eeprom_write(epos_t *epos, uint8_t epos_id,
								  epos_custom_eeprom_t *tx);

/**
 * @brief Read EPOS4 custom EEPROM.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[out] rx       EPOS4 custom EEPROM.
 * @return status.
 */
uint32_t epos_custom_eeprom_read(epos_t *epos, uint8_t epos_id,
								 epos_custom_eeprom_t *rx);

#ifdef __cplusplus
}
#endif

#endif /* EPOS_EEPROM_H */