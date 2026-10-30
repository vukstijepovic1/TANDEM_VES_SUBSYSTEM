/**
 * @file    epos_common.c
 * @brief   EPOS common types and definitions.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "epos_common.h"
#include "epos_od.h"

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

uint32_t epos_obj_write(epos_t *epos, uint8_t epos_id,
						const canopen_od_entry_t *obj, void *val)
{
	/* check if EPOS ID is valid */
	if (epos_id >= epos->track_count)
		return 1;

	/* write data into EPOS4 object dictionary */
	return canopen_sdo_download(epos->handle, epos->track[epos_id].node_id, obj,
								val, EPOS_COMM_TIMEOUT_MS);
}

uint32_t epos_obj_read(epos_t *epos, uint8_t epos_id,
					   const canopen_od_entry_t *obj, void *val)
{
	/* check if EPOS ID is valid */
	if (epos_id >= epos->track_count)
		return 1;

	/* read data from EPOS4 object dictionary */
	return canopen_sdo_upload(epos->handle, epos->track[epos_id].node_id, obj,
							  val, EPOS_COMM_TIMEOUT_MS);
}

uint32_t epos_nmt(epos_t *epos, uint8_t epos_id, canopen_nmt_cs_e cs)
{
	/* check if EPOS ID is valid */
	if (epos_id >= epos->track_count)
		return 1;

	/* send Network Management message */
	return canopen_nmt(epos->handle, epos->track[epos_id].node_id, cs);
}

uint32_t epos_mode(epos_t *epos, uint8_t epos_id, epos_op_mode_e op_mode)
{
	epos_op_mode_e rx_mode;
	uint16_t retry = 0;

	/* enter Pre-Operational state */
	if (epos_nmt(epos, epos_id, CANOPEN_NMT_CS_ENTER_PRE_OPERATIONAL))
		return 1;

	/* check Mode of Operation */
	if (epos_obj_write(epos, epos_id, &ObjModesOfOperation, &op_mode))
		return 1;

	/* check Mode of Operation */
	do {
		if (epos_obj_read(epos, epos_id, &ObjModesOfOperationDisplay, &rx_mode))
			return 1;
	} while (retry++ < EPOS_ENTER_OP_MODE_MAX_RETRY && rx_mode != op_mode);

	if (rx_mode != op_mode)
		return 1;

	/* start remote node */
	if (epos_nmt(epos, epos_id, CANOPEN_NMT_CS_START_REMOTE_NODE))
		return 1;

	/* enter Switch on Disabled */
	if (epos_enter_switch_on_disabled(epos, epos_id))
		return 1;

	/* enter Ready to Switch On */
	if (epos_enter_ready_to_switch_on(epos, epos_id))
		return 1;

	return 0;
}

uint32_t epos_enter_switch_on_disabled(epos_t *epos, uint8_t epos_id)
{
	uint16_t sw = 0;
	uint16_t cw = 0;
	epos_state_e state;

	/* read and check current EPOS state
	 *
	 * For more information on state check please refer to
	 * EPOS4 Firmware Specification page 16 of 322
	 */
	if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
		return 1;

	state = sw & EPOS_DEF_STATE_MASK;

	if (state == EPOS_STATE_SWITCH_ON_DISABLED)
		return 0;

	if (state != EPOS_STATE_READY_TO_SWITCH_ON &&
		state != EPOS_STATE_OPERATION_ENABLED &&
		state != EPOS_STATE_SWITCHED_ON &&
		state != EPOS_STATE_QUICK_STOP_ACTIVE)
		return 1;

	/* send disable voltage controlword */
	cw = EPOS_DEF_CONTROLWORD_DISABLE_VOLTAGE;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	/* check if new EPOS state is valid */
	uint8_t retry = 0;
	do {
		if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
			return 1;

		state = sw & EPOS_DEF_STATE_MASK;

		if (state == EPOS_STATE_SWITCH_ON_DISABLED)
			break;

	} while (retry++ < EPOS_ENTER_STATE_MAX_RETRY);

	if (retry >= EPOS_ENTER_STATE_MAX_RETRY)
		return 1;

	return 0;
}

uint32_t epos_enter_ready_to_switch_on(epos_t *epos, uint8_t epos_id)
{
	uint16_t sw = 0;
	uint16_t cw = 0;
	epos_state_e state;

	/* read and check current EPOS state
	 *
	 * For more information on state check please refer to
	 * EPOS4 Firmware Specification page 16 of 322
	 */
	if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
		return 1;

	state = sw & EPOS_DEF_STATE_MASK;

	if (state == EPOS_STATE_READY_TO_SWITCH_ON)
		return 0;

	if (state != EPOS_STATE_SWITCH_ON_DISABLED &&
		state != EPOS_STATE_SWITCHED_ON &&
		state != EPOS_STATE_OPERATION_ENABLED)
		return 1;

	/* send shutdown controlword */
	cw = EPOS_DEF_CONTROLWORD_SHUTDOWN;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	/* check if new EPOS state is valid */
	uint8_t retry = 0;
	do {
		if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
			return 1;

		state = sw & EPOS_DEF_STATE_MASK;

		if (state == EPOS_STATE_READY_TO_SWITCH_ON)
			break;

	} while (retry++ < EPOS_ENTER_STATE_MAX_RETRY);

	if (retry >= EPOS_ENTER_STATE_MAX_RETRY)
		return 1;

	return 0;
}

uint32_t epos_enter_switched_on(epos_t *epos, uint8_t epos_id)
{
	uint16_t sw = 0;
	uint16_t cw = 0;
	epos_state_e state;

	/* read and check current EPOS state
	 *
	 * For more information on state check please refer to
	 * EPOS4 Firmware Specification page 16 of 322
	 */
	if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
		return 1;

	state = sw & EPOS_DEF_STATE_MASK;

	if (state == EPOS_STATE_SWITCHED_ON)
		return 0;

	if (state != EPOS_STATE_READY_TO_SWITCH_ON &&
		state != EPOS_STATE_OPERATION_ENABLED)
		return 1;

	/* send switch on controlword */
	cw = EPOS_DEF_CONTROLWORD_SWITCH_ON;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	/* check if new EPOS state is valid */
	uint8_t retry = 0;
	do {
		if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
			return 1;

		state = sw & EPOS_DEF_STATE_MASK;

		if (state == EPOS_STATE_SWITCHED_ON)
			break;

	} while (retry++ < EPOS_ENTER_STATE_MAX_RETRY);

	if (retry >= EPOS_ENTER_STATE_MAX_RETRY)
		return 1;

	return 0;
}

uint32_t epos_enter_operation_enabled(epos_t *epos, uint8_t epos_id)
{
	uint16_t sw = 0;
	uint16_t cw = 0;
	epos_state_e state;

	/* read and check current EPOS state
	 *
	 * For more information on state check please refer to
	 * EPOS4 Firmware Specification page 16 of 322
	 */
	if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
		return 1;

	state = sw & EPOS_DEF_STATE_MASK;

	if (state == EPOS_STATE_OPERATION_ENABLED)
		return 0;

	if (state != EPOS_STATE_SWITCHED_ON &&
		state != EPOS_STATE_READY_TO_SWITCH_ON && // Special Case
		state != EPOS_STATE_QUICK_STOP_ACTIVE)
		return 1;

	/* send enable operation controlword */
	cw = EPOS_DEF_CONTROLWORD_ENABLE_OPERATION;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	/* check if new EPOS state is valid */
	uint8_t retry = 0;
	do {
		if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
			return 1;

		state = sw & EPOS_DEF_STATE_MASK;

		if (state == EPOS_STATE_OPERATION_ENABLED)
			break;

	} while (retry++ < EPOS_ENTER_STATE_MAX_RETRY);

	if (retry >= EPOS_ENTER_STATE_MAX_RETRY)
		return 1;

	return 0;
}

uint32_t epos_enter_quick_stop_active(epos_t *epos, uint8_t epos_id)
{
	uint16_t sw = 0;
	uint16_t cw = 0;
	epos_state_e state;

	/* read and check current EPOS state
	 *
	 * For more information on state check please refer to
	 * EPOS4 Firmware Specification page 16 of 322
	 */
	if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
		return 1;

	state = sw & EPOS_DEF_STATE_MASK;

	if (state == EPOS_STATE_QUICK_STOP_ACTIVE)
		return 0;

	if (state != EPOS_STATE_OPERATION_ENABLED)
		return 1;

	/* send quick stop controlword */
	cw = EPOS_DEF_CONTROLWORD_QUICK_STOP;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	/* check if new EPOS state is valid */
	uint8_t retry = 0;
	do {
		if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
			return 1;

		state = sw & EPOS_DEF_STATE_MASK;

		if (state == EPOS_STATE_QUICK_STOP_ACTIVE)
			break;

	} while (retry++ < EPOS_ENTER_STATE_MAX_RETRY);

	if (retry >= EPOS_ENTER_STATE_MAX_RETRY)
		return 1;

	return 0;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/