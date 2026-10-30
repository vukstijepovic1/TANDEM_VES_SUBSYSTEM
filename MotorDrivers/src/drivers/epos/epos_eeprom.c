/**
 * @file    epos_eeprom.c
 * @brief   EPOS EEPROM.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "epos_eeprom.h"
#include "epos_od.h"

#include "epos_def.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

uint32_t epos_eeprom_save(epos_t *epos, uint8_t epos_id)
{
	uint32_t reg_val = EPOS_DEF_SAVE_ALL_PARAMETERS_MAGIC;

	return canopen_sdo_download(epos->handle, epos->track[epos_id].node_id,
								&ObjSaveAllParameters, &reg_val,
								EPOS_EEPROM_TIMEOUT_MS);
}

uint32_t epos_eeprom_restore(epos_t *epos, uint8_t epos_id)
{
	uint32_t reg_val = EPOS_DEF_RESTORE_ALL_PARAMETERS_MAGIC;

	return canopen_sdo_download(epos->handle, epos->track[epos_id].node_id,
								&ObjRestoreAllDefaultParameters, &reg_val,
								EPOS_EEPROM_TIMEOUT_MS);
}

uint32_t epos_custom_eeprom_write(epos_t *epos, uint8_t epos_id,
								  epos_custom_eeprom_t *tx)
{
	/* write custom EEPROM parameter 1 */
	if (epos_obj_write(epos, epos_id, &ObjCustomPersistentMemory1, &tx->param1))
		return 1;

	/* write custom EEPROM parameter 2 */
	if (epos_obj_write(epos, epos_id, &ObjCustomPersistentMemory2, &tx->param2))
		return 1;

	/* write custom EEPROM parameter 3 */
	if (epos_obj_write(epos, epos_id, &ObjCustomPersistentMemory3, &tx->param3))
		return 1;

	/* write custom EEPROM parameter 4 */
	if (epos_obj_write(epos, epos_id, &ObjCustomPersistentMemory4, &tx->param4))
		return 1;

	return 0;
}

uint32_t epos_custom_eeprom_read(epos_t *epos, uint8_t epos_id,
								 epos_custom_eeprom_t *rx)
{
	/* read custom EEPROM parameter 1 */
	if (epos_obj_read(epos, epos_id, &ObjCustomPersistentMemory1, &rx->param1))
		return 1;

	/* read custom EEPROM parameter 2 */
	if (epos_obj_read(epos, epos_id, &ObjCustomPersistentMemory2, &rx->param2))
		return 1;

	/* read custom EEPROM parameter 3 */
	if (epos_obj_read(epos, epos_id, &ObjCustomPersistentMemory3, &rx->param3))
		return 1;

	/* read custom EEPROM parameter 4 */
	if (epos_obj_read(epos, epos_id, &ObjCustomPersistentMemory4, &rx->param4))
		return 1;

	return 0;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/