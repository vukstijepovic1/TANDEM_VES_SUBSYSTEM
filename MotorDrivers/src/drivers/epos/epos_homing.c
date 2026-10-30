/**
 * @file    epos_ppm.c
 * @brief   EPOS Homing Mode.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "epos_homing.h"
#include "epos_eeprom.h"
#include "epos_od.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* position referenced to home position mask */
#define EPOS_POSITION_REF_TO_HOME_MASK (1 << 15)
/* homing error mask */
#define EPOS_HOMING_ERROR_MASK (1 << 13)
/* homing attained mask */
#define EPOS_HOMING_ATTAINED_MASK (1 << 12)

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

uint32_t epos_homing_cfg(epos_t *epos, uint8_t epos_id, epos_homing_cfg_t *cfg)
{
	/* enter pre-operational state. */
	if (epos_nmt(epos, epos_id, CANOPEN_NMT_CS_ENTER_PRE_OPERATIONAL))
		return 1;

	/* configure homing speed during switch search. */
	if (epos_obj_write(epos, epos_id, &ObjSpeedForSwitchSearch,
					   &cfg->hspeed_sw_search))
		return 1;

	/* configure homing speed during zero search. */
	if (epos_obj_write(epos, epos_id, &ObjSpeedForZeroSearch,
					   &cfg->hspeed_zero_search))
		return 1;

	/* configure homing acceleration. */
	if (epos_obj_write(epos, epos_id, &ObjHomingAcceleration, &cfg->haccel))
		return 1;

	/* configure homing offset move distance. */
	if (epos_obj_write(epos, epos_id, &ObjHomeOffsetMoveDistance,
					   &cfg->hoff_move_dist))
		return 1;

	/* configure homing current threshold. */
	if (epos_obj_write(epos, epos_id, &ObjCurrentThresholdForHomingMode,
					   &cfg->hcurr_threshold))
		return 1;

	/* save all parameters */
	if (epos_eeprom_save(epos, epos_id))
		return 1;

	return 0;
}

uint32_t epos_homing(epos_t *epos, uint8_t epos_id, epos_homing_method_e method,
					 int32_t home_pos, uint32_t timeout)
{
	uint32_t ret = 1;
	uint16_t cw = 0;

	/* enter homing mode */
	if (epos_mode(epos, epos_id, EPOS_OP_MODE_HM))
		return 1;

	/* set home position. */
	if (epos_obj_write(epos, epos_id, &ObjHomePosition, &home_pos))
		return 1;

	/* enter operation enabled state */
	if (epos_enter_operation_enabled(epos, epos_id))
		return 1;

	/* select homing method */
	if (epos_obj_write(epos, epos_id, &ObjHomingMethod, &method))
		return 1;

	/* enter operation enabled */
	cw = EPOS_DEF_CONTROLWORD_ENABLE_OPERATION;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	HAL_DelayMS(1);

	/* homing operation start */
	cw = 0x001F;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	uint64_t t0;

	t0 = HAL_GetTimeUS();
	while (1) {
		uint16_t sw = 0;

		epos_obj_read(epos, epos_id, &ObjStatusword, &sw);

		/* check timeout */
		if (HAL_GetTimeUS() - t0 >= timeout * 1000) {
			ret = 1;
			break;
		}

		/* check faults */
		if (sw & EPOS_HOMING_ERROR_MASK) {
			ret = 1;
			break;
		}

		/* check homing progress */
		if ((sw & EPOS_POSITION_REF_TO_HOME_MASK) &&
			(sw & EPOS_HOMING_ATTAINED_MASK)) {
			ret = 0;
			break;
		}
	}

	if (epos_enter_ready_to_switch_on(epos, epos_id))
		return 1;

	return ret;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/