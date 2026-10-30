/**
 * @file    epos_csp.h
 * @brief   EPOS Cyclic Synchronous Position Mode.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

#ifndef EPOS_CSP_H
#define EPOS_CSP_H

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

/*! @brief Cyclic Synchronous Position Mode Configuration. */
typedef struct epos_csp_cfg_t {
	//!< Used with a <<Quick stop>> command to determine the deceleration of
	//   the quick stop profile. Value is given in [acceleration units]
	uint32_t qsdecel;
	//!< Defines deceleration value used during profiled move [accel units]
	uint32_t pdecel;
	//!< Permitted difference between Position Actual and Demand value
	//   [position units]
	uint32_t follow_error_window;
	//!< Defines the absolute negative position limit [position units]
	int32_t min_pos;
	//!< Defines the absolute positive position limit [position units]
	int32_t max_pos;
	//!< Defines interpolation time period (time between PDOs) [ms]
	int32_t interpolation_time_period;

} epos_csp_cfg_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Configure EPOS4 Cyclic Synchronous Position Mode.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] cfg		Cyclic Synchronous Position Mode Configuration.
 * @return status.
 */
uint32_t epos_csp_cfg(epos_t *epos, uint8_t epos_id, epos_csp_cfg_t *cfg);

/**
 * @brief Control EPOS4 operating in Cyclic Synchronous Position Mode.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] cw		Controlword.
 * @param[in] sp		Setpoint [position units].
 * @return status.
 */
uint32_t epos_csp_control(epos_t *epos, uint8_t epos_id, uint16_t cw,
						  int32_t sp);

#ifdef __cplusplus
}
#endif

#endif /* EPOS_CSP_H */