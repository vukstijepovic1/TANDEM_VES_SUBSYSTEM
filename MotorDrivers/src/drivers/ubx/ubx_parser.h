/**
 * @file    ubx_parser.h
 * @brief   UBX parser.
 * @version	1.0.0
 * @date    02.04.2025
 * @author  LisumLab
 */

#ifndef UBX_PARSER_H
#define UBX_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ubx_common.h"
#include <stdint.h>

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
 * @brief Decode UBX Geodetic position solution.
 *
 * @param[in] inst	UBX instance.
 * @return None
 */
void ubx_init(ubx_t *inst);

/**
 * @brief Decode UBX Geodetic position solution.
 *
 * @param[in] inst	UBX instance.
 * @return 0 - parse success, 1 - parse ongoing, -1 - parse error
 */
uint32_t ubx_parse(ubx_t *inst, const uint8_t recv_byte);

#ifdef __cplusplus
}
#endif

#endif /* UBX_PARSER_H */