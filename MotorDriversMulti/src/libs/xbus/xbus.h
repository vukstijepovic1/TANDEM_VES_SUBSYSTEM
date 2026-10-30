/**
 * @file    xbus.h
 * @brief   XBUS parser.
 * @date    12.09.2024.
 * @version 1.0.0
 *
 * @details Full support for XBUS protocol isn't implemented, this version
 *          of parser focuses on MTData2 message which contains sensor data.
 *
 *          MTData2 message contains multiple data packets which user can
 *          track via subscription. Device MTData2 Data Packet output
 *          configuration is done using MT Manager Application, and isn't
 *          possible using this library for now.
 *
 *          Rest of XBUS Messages are silently dropped, same goes for
 *          MTData2 Data Packets which are not tracked.
 *
 * @author  LisumLab
 */

#ifndef XBUS_H
#define XBUS_H

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <assert.h>
#include <stdint.h>

#include "xbus_def.h"

#ifdef XBUS_CONFIG
#include "xbus_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef XBUS_MAX_SUB_CNT
//!< Max subscription count
#define XBUS_MAX_SUB_CNT 32U
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

// Forward Declaration.
struct xbus_t;

/* XBUS packet subscription callback. */
typedef void (*xbus_sub_callback)(struct xbus_t *xbus, void *pckt,
								  xbus_xdi_t xdi, uint8_t xdi_flags);

/* XBUS device */
typedef struct xbus_t {
	// Subscriptions and callbacks - Read Only
	uint16_t sub[XBUS_MAX_SUB_CNT];
	xbus_sub_callback sub_cb[XBUS_MAX_SUB_CNT];
	uint8_t sub_cnt;

	// Parsed XBUS packet - Read Only
	xbus_xdi_packet_t packet;
	// Linear Buffer - Read Only
	uint8_t data[XBUS_DEF_MAX_PACKET_SIZE];
	uint8_t len;
	uint16_t idx;

	// XBUS Parser State - Read Only
	xbus_state_t state;

#ifdef XBUS_COM_DBG
	// XBUS Communication debug variables.
	uint32_t drop_cnt;
	uint32_t pass_cnt;
#endif

} xbus_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize XBUS Parser.
 *
 * @param[in] xbus  XBUS Parser.
 * @return None.
 */
void xbus_init(xbus_t *xbus);

/**
 * @brief Subscribe to XBUS MTData2 Data Packet.
 *
 * @param[in] xbus  XBUS Parser.
 * @param[in] xdi   MTData2 Packet ID.
 * @param[in] flags Packet Flags which define:
 *                  - Data contained in packet (float32, fp1220 or float64).
 *                  - Referent coordinate system (ENU, NED, NWU).
 *                  Which is formed as:
 *                      flags = (xbus_precision_t | xbus_csys_t)
 * @param[in] cb   Subscription Callback.
 * @return Subscription ID.
 */
uint16_t xbus_sub(xbus_t *xbus, xbus_xdi_t xdi, uint8_t flags,
				  xbus_sub_callback cb);

/**
 * @brief Parse byte.
 *
 * @param[in] xbus  XBUS Parser.
 * @param[in] byte  Byte for parsing.
 * @return Parse status:    0 - Message parsed successfully.
 *                          1 - Parsing ongoing.
 *                          -1 - Parsing failed.
 */
uint32_t xbus_parse(xbus_t *xbus, uint8_t byte);

#endif /* XBUS_H */