/**
 *	@file     eth.h
 *  @brief    HAL ETH library.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef ETH_H
#define ETH_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

#ifdef DEV_CONFIG
#include "dev_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef HAL_ETH_DROP_CRC
/*! Should ETH Layer drop packet CRC or not. */
#define HAL_ETH_DROP_CRC 1
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief Ethernet HAL Physical Layer - used by higher level protocols. */
typedef struct eth_hal_phy_t {
	uint16_t (*recv)(void *phy, uint8_t *data, uint16_t size);
	uint16_t (*send)(void *phy, const uint8_t *data, uint16_t size);
} eth_hal_phy_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 *  @brief Initialize Ethernet.
 *
 *  @param[in] eth Ethernet HAL Physical Layer.
 *  @param[in] dev_mac Device MAC Address.
 *  @return None
 */
void HAL_ETH_Init(eth_hal_phy_t *eth, const uint8_t *dev_mac);

/**
 *  @brief Receive data via ETH - if available.
 *
 *  @param[in] phy Ethernet HAL Physical Layer.
 *  @param[in] data Receive data.
 *  @param[in] size Receive data size.
 *  @return Size of received data.
 */
uint16_t HAL_ETH_Recv(void *phy, uint8_t *data, uint16_t size);

/**
 *  @brief Transmit data via ETH.
 *
 *  @param[in] phy Ethernet HAL Physical Layer.
 *  @param[in] data Transmit data.
 *  @param[in] size Transmit data size.
 *  @return Size of transmitted data.
 */
uint16_t HAL_ETH_Send(void *phy, const uint8_t *data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* ETH_H */