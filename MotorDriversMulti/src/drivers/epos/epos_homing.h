/**
 * @file    epos_csp.h
 * @brief   EPOS Homing Mode.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

#ifndef EPOS_HOMING_H
#define EPOS_HOMING_H

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

/*! @brief Homing methods. */
typedef enum epos_homing_method_e {
	//!< The actual position is changed and considered as the future Home
	//   position. The method may be used if the axis is disabled.
	EPOS_HOMING_METHOD_ACT_POS = (int8_t)37,
	//!< The direction for homing is positive
	EPOS_HOMING_METHOD_INDEX_POS_SPEED = (int8_t)34,
	//!< The direction for homing is negative.
	EPOS_HOMING_METHOD_INDEX_NEG_SPEED = (int8_t)33,
	//!< The principle is similar to homing method with home switch, negative
	//   speed and index except that the Home position is not dependent on the
	//   index pulse but only on the falling edge of the home switch.
	EPOS_HOMING_METHOD_HOME_SWITCH_NEG_SPEED = (int8_t)27,
	//!< The principle is similar to homing method 7 except that the Home
	//   position is not dependent on the index pulse but only on the rising
	//   edge of the home switch.
	EPOS_HOMING_METHOD_HOME_SWITCH_POS_SPEED = (int8_t)23,
	//!< The principle is similar to homing method 2 except that the Home
	//   position is not dependent on the index pulse but only on the positive
	//   edge of the positive limit switch.
	EPOS_HOMING_METHOD_POS_LIMIT_SWITCH = (int8_t)18,
	//!< The principle is similar to homing method with negative limit switch
	//   and index except that the Home position is not dependent on the index
	//   pulse but only on the negative edge of the negative limit switch.
	EPOS_HOMING_METHOD_NEG_LIMIT_SWITCH = (int8_t)17,
	//!< The method uses a home switch, which is active only during part of the
	//   movement. In effect, the switch acts as the position of the axis
	//   sweeps past the switch.
	EPOS_HOMING_METHOD_HOME_SWITCH_NEG_SPEED_AND_INDEX = (int8_t)11,
	//!< The method uses a home switch, which is active only during part of the
	//   movement. In effect, the switch acts as the position of the axis
	//   sweeps past the switch.
	EPOS_HOMING_METHOD_HOME_SWITCH_POS_SPEED_AND_INDEX = (int8_t)7,
	//!< The initial direction of the movement is positive if the positive
	//   limit switch is inactive
	EPOS_HOMING_METHOD_POS_LIMIT_SWITCH_AND_INDEX = (int8_t)2,
	//!< The initial direction of the movement is negative if the negative
	//   limit switch is inactive.
	EPOS_HOMING_METHOD_NEG_LIMIT_SWITCH_AND_INDEX = (int8_t)1,
	//!< The method uses a mechanical end stop on the positive side. The edge
	//   is detected when the averaged output current rises above Current
	//   threshold for homing mode.
	EPOS_HOMING_METHOD_CURR_THR_POS_SPEED_AND_INDEX = (int8_t)-1,
	//!< The method uses a mechanical end stop on the left side. The edge is
	//   detected when the averaged output current rises above Current
	//   threshold for homing mode.
	EPOS_HOMING_METHOD_CURR_THR_NEG_SPEED_AND_INDEX = (int8_t)-2,
	//!< The principle is similar to homing method -1 except that the Home
	//   position is not dependent on the index pulse but only on the
	//   mechanical end stop.
	EPOS_HOMING_METHOD_CURR_THR_POS_SPEED = (int8_t)-3,
	//!< The principle is similar to homing method -2 except that the Home
	//   position is not dependent on the index pulse but only on the
	//   mechanical end stop.
	EPOS_HOMING_METHOD_CURR_THR_NEG_SPEED = (int8_t)-4
} epos_homing_method_e;

/*! @brief Homing configuration. */
typedef struct epos_homing_cfg_t {
	//!< Speed for switch search [velocity units].
	uint32_t hspeed_sw_search;

	//!< Speed for zero search [velocity units].
	//   Used to search the index in a homing sequence
	uint32_t hspeed_zero_search;

	//!< Used to define acceleration and deceleration ramps in the homing
	//   profile [acceleration units].
	uint32_t haccel;

	//!< Represents a moving distance in a homing procedure. It is useful to
	//   move away from a detected position (for example mechanical limit stop
	//   or limit switch) at the end of the homing sequence, thus preventing
	//   the axis from a border damage respectively limit switch error
	//   [position units]
	int32_t hoff_move_dist;

	//!< Used for homing modes «−1», «−2», «−3», and «−4». A mechanical border
	//   will be detected when the measured motor current rises above the
	//   specified threshold [mA].
	uint16_t hcurr_threshold;

} epos_homing_cfg_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Configure EPOS4 Homing Mode.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] cfg		Homing Configuration.
 * @return status.
 */
uint32_t epos_homing_cfg(epos_t *epos, uint8_t epos_id, epos_homing_cfg_t *cfg);

/**
 * @brief Perform EPOS4 Homing.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] method	Homing Method.
 * @param[in] home_pos	Home position [position units].
 * @param[in] timeout	Homing timeout [ms].
 * @return status.
 */
uint32_t epos_homing(epos_t *epos, uint8_t epos_id, epos_homing_method_e method,
					 int32_t home_pos, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* EPOS_HOMING_H */