/**
 * @file    epos_ppm.c
 * @brief   EPOS Profile Position Mode.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "epos_ppm.h"
#include "epos_eeprom.h"
#include "epos_od.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/* PPM control structure */
typedef struct __attribute__((__packed__)) epos_ppm_control_t {
	uint16_t cw;
	int32_t sp;
} epos_ppm_control_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* configure PPM PDO Rx */
static uint32_t epos_ppm_pdorx_cfg(epos_t *epos, uint8_t epos_id);

/*******************************************************************************
 * Code
 ******************************************************************************/

uint32_t epos_ppm_cfg(epos_t *epos, uint8_t epos_id, epos_ppm_cfg_t *cfg)
{
	/* enter pre-operational state */
	if (epos_nmt(epos, epos_id, CANOPEN_NMT_CS_ENTER_PRE_OPERATIONAL))
		return 1;

	/* configure minimum position */
	if (epos_obj_write(epos, epos_id, &ObjMinPositionLimit, &cfg->min_pos))
		return 1;

	/* configure maximum position */
	if (epos_obj_write(epos, epos_id, &ObjMaxPositionLimit, &cfg->max_pos))
		return 1;

	/* configure maximum profile velocity */
	if (epos_obj_write(epos, epos_id, &ObjMaxProfileVelocity, &cfg->max_pvel))
		return 1;

	/* configure quick stop deceleration */
	if (epos_obj_write(epos, epos_id, &ObjQuickStopDeceleration, &cfg->qsdecel))
		return 1;

	/* configure maximum acceleration */
	if (epos_obj_write(epos, epos_id, &ObjMaxAcceleration, &cfg->max_accel))
		return 1;

	/* configure profile velocity */
	if (epos_obj_write(epos, epos_id, &ObjProfileVelocity, &cfg->pvel))
		return 1;

	/* configure profiel acceleration */
	if (epos_obj_write(epos, epos_id, &ObjProfileAcceleration, &cfg->paccel))
		return 1;

	/* configure profiel deceleration */
	if (epos_obj_write(epos, epos_id, &ObjProfileDeceleration, &cfg->pdecel))
		return 1;

	/* configure PDOs */
	if (epos_ppm_pdorx_cfg(epos, epos_id))
		return 1;

	/* save all parameters */
	if (epos_eeprom_save(epos, epos_id))
		return 1;

	return 0;
}

uint32_t epos_ppm_control(epos_t *epos, uint8_t epos_id, uint16_t cw,
						  int32_t sp)
{
	/* check if EPOS ID is valid */
	if (epos_id >= epos->track_count)
		return 1;

	epos_ppm_control_t ctrl = {.cw = cw, .sp = sp};

	return canopen_pdo1rx(epos->handle, epos->track[epos_id].node_id,
						  (uint8_t *)&ctrl, sizeof(ctrl));
}

/****************************** static functions ******************************/

static uint32_t epos_ppm_pdorx_cfg(epos_t *epos, uint8_t epos_id)
{
	uint32_t reg_val;

	/* config Receive PDO1 */
	reg_val = EPOS_DEF_PDO_VALID | EPOS_DEF_PDO_RTR_ALLOWED |
			  CANOPEN_COB_TYPE_PDO1_RX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByRxPDO1, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_SYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeRxPDO1, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInRxPDO1,
					   &reg_val))
		return 1;

	reg_val = (ObjControlword.idx << 16) | (ObjControlword.subidx << 8) |
			  (ObjControlword.data_size * 8); // Data length = 16bit

	if (epos_obj_write(epos, epos_id, &ObjFirstMappedObjectInRxPDO1, &reg_val))
		return 1;

	reg_val = (ObjTargetPosition.idx << 16) | (ObjTargetPosition.subidx << 8) |
			  (ObjTargetPosition.data_size * 8); // Data length = 32bit

	if (epos_obj_write(epos, epos_id, &ObjSecondMappedObjectInRxPDO1, &reg_val))
		return 1;

	reg_val = 0x02;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInRxPDO1,
					   &reg_val))
		return 1;

	/* config Receive PDO2 */
	reg_val = EPOS_DEF_PDO_INVALID | EPOS_DEF_PDO_RTR_NOT_ALLOWED |
			  CANOPEN_COB_TYPE_PDO2_RX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByRxPDO2, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_ASYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeRxPDO2, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInRxPDO2,
					   &reg_val))
		return 1;

	/* config Receive PDO3 */
	reg_val = EPOS_DEF_PDO_INVALID | EPOS_DEF_PDO_RTR_NOT_ALLOWED |
			  CANOPEN_COB_TYPE_PDO3_RX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByRxPDO3, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_ASYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeRxPDO3, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInRxPDO3,
					   &reg_val))
		return 1;

	/* config Receive PDO4 */
	reg_val = EPOS_DEF_PDO_INVALID | EPOS_DEF_PDO_RTR_NOT_ALLOWED |
			  CANOPEN_COB_TYPE_PDO4_RX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByRxPDO4, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_ASYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeRxPDO4, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInRxPDO4,
					   &reg_val))
		return 1;

	return 0;
}

/********************************* End Of File ********************************/