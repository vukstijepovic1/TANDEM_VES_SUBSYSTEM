/**
 * @file    epos_ppm.h
 * @brief   EPOS Profile Position Mode.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

#ifndef EPOS_PPM_H
#define EPOS_PPM_H

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

/*! @brief Profile Position Mode configuration. */
typedef struct epos_ppm_cfg_t {
	//!< Defines the absolute negative position limit [position units]
	int32_t min_pos;
	//!< Defines the absolute positive position limit [position units]
	int32_t max_pos;
	//!< Used as velocity limit in a PPM or PVM move [velocity units]
	uint32_t max_pvel;
	//!< Used with a <<Quick stop>> command to determine the deceleration of
	//   the quick stop profile. Value is given in [acceleration units]
	uint32_t qsdecel;
	//!< Used to limit the maximum allowed acceleration [rpm/s]
	uint32_t max_accel;
	//!< Represents velocity normally attained at the end of acceleration ramp
	//   during a profiled move (PPM, PVM) [velocity units]
	uint32_t pvel;
	//!< Defines acceleration value used during profiled move [accel units]
	uint32_t paccel;
	//!< Defines deceleration value used during profiled move [accel units]
	uint32_t pdecel;

} epos_ppm_cfg_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Configure EPOS4 Profile Position Mode.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] cfg		Profile Position Mode Configuration.
 * @return status.
 */
uint32_t epos_ppm_cfg(epos_t *epos, uint8_t epos_id, epos_ppm_cfg_t *cfg);

/**
 * @brief Control EPOS4 operating in Profile Position Mode.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] cw		Controlword.
 * @param[in] sp		Setpoint [position units].
 * @return status.
 */
uint32_t epos_ppm_control(epos_t *epos, uint8_t epos_id, uint16_t cw,
						  int32_t sp);

#ifdef __cplusplus
}
#endif

#endif /* EPOS_PPM_H */