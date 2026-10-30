/**
 * @file    sbus.h
 * @brief   SBUS Library.
 * @version 1.0.0
 * @date    22.01.2025
 * @author  LisumLab
 */

/**
 * @note    SBUS is usually transfered over UART using inverted logic, so logic
 *          inverter circuit may need to be used.
 *          Standard UART Configuration for SBUS
 *              Baud = 100000,
 *              Number of Data Bits = 8,
 *              Number of Stop Bits = 2,
 *              Parity = Even
 *          Please note that there is also fast SBUS, whose Baud is 200000.
 *          Before using this Library User should refer to device datasheet.
 */

#ifndef SBUS_H
#define SBUS_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#ifdef SBUS_CONFIG
#include "sbus_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef SBUS_PACKET_TIMEOUT_MS
//!< Drop packet if certain time passed since packet start.
#define SBUS_PACKET_TIMEOUT_MS 5
#endif

#define SBUS_PACKET_SIZE 25		 //!< SBUS Packet size in bytes.
#define SBUS_PACKET_HEADER 0x0FU //!< SBUS Packet Header Byte.
#define SBUS_PACKET_FOOTER 0x00U //!< SBUS Packet Footer Byte.

#define SBUS_PWM_CHANNEL_MASK 0x7FFU //!< PWM Channel Flag (11bit value)
#define SBUS_PWM_CHANNEL_CNT 16		 //!< Number of PWM Channels
#define SBUS_DIGI_CHANNEL_CNT 2		 //!< Number of Digital Channels

#define SBUS_CH_17_OFFSET 0		 //!< Digital Channel 1 Value Offset.
#define SBUS_CH_18_OFFSET 1		 //!< Digital Channel 2 Value Offset.
#define SBUS_FRAME_LOST_OFFSET 2 //!< Frame Lost Flag Offset.
#define SBUS_FAILSAFE_OFFSET 3	 //!< Failsafe Flag Offset.

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief SBUS Parser State. */
typedef enum sbus_parser_state_e {
	SBUS_PARSER_STATE_HEADER = 0, //!< Parser is waiting for SBUS Header.
	SBUS_PARSER_STATE_DATA,		  //!< Parser is receiving channel data.
	SBUS_PARSER_STATE_FOOTER	  //!< Parser is waiting for SBUS Footer.
} sbus_parser_state_e;

/*! @brief SBUS Packet Data. */
typedef struct sbus_data_t {
	uint16_t pwm_ch[SBUS_PWM_CHANNEL_CNT];	//!< 16 PWM Channel Data.
	uint8_t digi_ch[SBUS_DIGI_CHANNEL_CNT]; //!< 2 Digital Channel Data.
	uint8_t frame_lost;						//!< SBUS Frame Lost flag.
	uint8_t failsafe;						//!< SBUS Failsafe flag.
} sbus_data_t;

/*! @brief SBUS device. */
typedef struct sbus_t {
	uint8_t arr[SBUS_PACKET_SIZE];	  //!< Parser internal value, do not edit!
	uint8_t i;						  //!< Parser internal value, do not edit!
	sbus_parser_state_e parser_state; //!< Parser internal value, do not edit!

	sbus_data_t data;		   //!< Received SBUS Data.
	uint64_t (*time_us)(void); //!< Pointer to device time function.
	uint64_t pckt_start_time;  //!< Time at which packet parse started [us].

#ifdef SBUS_DBG
	uint32_t drop_cnt; //!< Number of dropped packets.
	uint32_t pass_cnt; //!< Number of success packets.
#endif
} sbus_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize SBUS.
 *
 * @param[in] sbus      SBUS device.
 * @param[in] time_us   Pointer to device time function [us].
 * @return None
 */
void sbus_init(sbus_t *sbus, uint64_t (*time_us)());

/**
 * @brief Parse incoming SBUS data.
 *
 * @param[in] sbus  SBUS device.
 * @param[in] byte  Received SBUS packet byte.
 * @return Parsing status:  0 - Packet Parse Ongoing.
 *                          1 - Packet Parse Success.
 *                          -1 - Packet Parse Fail.
 */
int32_t sbus_parse(sbus_t *sbus, uint8_t byte);

/**
 * @brief Pack SBUS packet.
 *
 * @param[in] arr   Array which shall hold SBUS packet, array size must be at
 *                  least SBUS_PACKET_SIZE.
 * @param[in] data  SBUS Packet data.
 * @return None
 */
void sbus_pack(uint8_t *arr, sbus_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* SBUS_H */