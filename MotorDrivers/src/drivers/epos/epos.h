/**
 * @file	EPOS.h
 * @brief	EPOS Library.
 * @version	1.0.0
 * @date	12.02.2025
 * @author	LisumLab
 */

#ifndef EPOS_H
#define EPOS_H

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

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize EPOS4 handler.
 *
 * @param[in] epos		EPOS4 handler.
 * @param[in] handle	CANopen handle.
 * @return None.
 */
void epos_init(epos_t *epos, canopen_handle_t *handle);

/**
 * @brief Track (Subscribe to) EPOS4.
 *
 * @param[in] epos		EPOS4 handler.
 * @param[in] node_id	EPOS4 CANopen Node ID.
 * @return EPOS4 track id on success, -1 on failure.
 */
uint8_t epos_track(epos_t *epos, uint8_t node_id);

/**
 * @brief Configure EPOS4.
 *
 * @param[in] epos		EPOS4 handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] cfg		EPOS4 configuration.
 * @return status.
 */
uint32_t epos_cfg(epos_t *epos, uint8_t epos_id, epos_cfg_t *cfg);

/**
 * @brief Get EPOS4 configuration.
 *
 * @param[in] epos		EPOS4 handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[out] cfg		EPOS4 configuration.
 * @return status.
 */
uint32_t epos_cfg_get(epos_t *epos, uint8_t epos_id, epos_cfg_t *cfg);

/**
 * @brief Reset EPOS4 fault.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_fault_reset(epos_t *epos, uint8_t epos_id);

/**
 * @brief Broadcast EPOS Heartbeat.
 *
 * @param[in] epos	EPOS handler.
 * @return None
 */
void epos_heartbeat(epos_t *epos);

/**
 * @brief Broadcast CANopen Sync.
 *
 * @param[in] epos	EPOS handler.
 * @return None
 */
void epos_sync(epos_t *epos);

/**
 * @brief Get EPOS4 status.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[out] status	EPOS4 Status.
 * @return status
 */
uint32_t epos_status_get(epos_t *epos, uint8_t epos_id, uint16_t *status);

/**
 * @brief Get EPOS4 position.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[out] pos		EPOS4 Position [position units].
 * @return status
 */
uint32_t epos_position_get(epos_t *epos, uint8_t epos_id, int32_t *pos);

/**
 * @brief Get EPOS4 velocity.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[out] vel		EPOS4 Velocity [velocity units].
 * @return status
 */
uint32_t epos_velocity_get(epos_t *epos, uint8_t epos_id, int32_t *vel);

uint32_t epos_sensor_position_get(epos_t *epos, uint8_t epos_id,
								  uint32_t sensor_id, int32_t *pos);

uint32_t epos_ssi_position_get(epos_t *epos, uint8_t epos_id, int64_t *pos);

#ifdef __cplusplus
}
#endif

#endif /* EPOS_H */