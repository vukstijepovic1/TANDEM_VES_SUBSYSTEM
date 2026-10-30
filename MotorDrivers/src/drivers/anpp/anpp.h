/**
 * @file    anpp.h
 * @brief   Advanced Navigation Packet Protocol (ANPP) parser.
 * @version	1.0.0
 * @date    02.04.2025
 * @author  LisumLab
 */

#ifndef ANPP_H
#define ANPP_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "anpp_common.h"

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
 * @brief Initialize ANPP device.
 *
 * @param[in] anpp  ANPP Device.
 * @return None
 */
void anpp_init(anpp_t *anpp);

/**
 * @brief Parse byte of Advanced Navigation Packet Protocol.
 *
 * @param[in] anpp  ANPP Device.
 * @param[in] byte  Byte for parsing.
 * @return 0 - parse success, 1 - parse ongoing, -1 - parse error
 */
uint32_t anpp_parse(anpp_t *anpp, uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif /* ANPP_H */