/**
 * @file    anpp_common.h
 * @brief   Advanced Navigation Packet Protocol (ANPP) common definitions.
 * @version	1.0.0
 * @date    02.04.2025
 * @author  LisumLab
 */

#ifndef ANPP_COMMON_H
#define ANPP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#ifdef ANPP_CONFIG
#include "anpp_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef ANPP_MAX_PACKET_LEN
/* max packet data length */
#define ANPP_MAX_PACKET_LEN 255U
#endif

/* packet IDs */
#define ANPP_PACKET_ID_INVALID 0xFFU  // Invalid Packet
#define ANPP_PACKET_ID_ACK 0		  // Acknowledge Packet - Read only
#define ANPP_PACKET_ID_REQUEST 1	  // Request Packet - Write only
#define ANPP_PACKET_ID_BOOT_MODE 2	  // Boot Mode Packet - Read/Write
#define ANPP_PACKET_ID_DEV_INFO 3	  // Device Information Packet - Read only
#define ANPP_PACKET_ID_RESET 5		  // Reset Packet - Write only
#define ANPP_PACKET_ID_RAW_SENSORS 28 // Raw Sensors Packet - Read only.
#define ANPP_PACKET_ID_AIR_DATA 68	  // Air Data Packet - Read only.

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief ANPP Header. */
typedef struct __attribute__((__packed__)) anpp_hdr_t {
	uint8_t lrc;  // Header LRC.
	uint8_t id;	  // Packet ID.
	uint8_t len;  // Packet Length.
	uint16_t crc; // CRC16.
} anpp_hdr_t;

/*! @brief ANPP Raw Sensors Status. */
typedef struct __attribute__((__packed__)) anpp_raw_sensor_status_t {
	uint8_t abs_press_valid : 1;	   // Aboslute pressure valid.
	uint8_t diff_press_valid : 1;	   // Differential Pressure valid.
	uint8_t abs_press_over_range : 1;  // Absolute pressure sensor over-range.
	uint8_t diff_press_over_range : 1; // Diff pressure sensor over-range.
	uint8_t abs_press_failure : 1;	   // Absolute pressure sensor failure.
	uint8_t diff_press_failure : 1;	   // Differential pressure sesnor failure.
	uint8_t temp_sensor_valid : 1;	   // Temperature sensor valid.
	uint8_t temp_sensor_failure : 1;   // Temperature sensor failure.
} anpp_raw_sensor_status_t;

/*! @brief ANPP Raw Sensors Packet. */
typedef struct __attribute__((__packed__)) anpp_raw_sensor_t {
	float abs_press;				 // Absolute pressure [Pa].
	float diff_press;				 // Differential Pressure [Pa].
	anpp_raw_sensor_status_t status; // Raw Sensors Status.
	float temperature;				 // Temperature [degC].
} anpp_raw_sensor_t;

/*! @brief ANPP Air Data Status. */
typedef struct __attribute__((__packed__)) anpp_air_data_status_t {
	uint8_t baro_alt_valid : 1;			 // Barometric altitude valid.
	uint8_t airspeed_valid : 1;			 // Airspeed valid.
	uint8_t baro_alt_over_range : 1;	 // Barometric alt sensor over-range.
	uint8_t airspeed_over_range : 1;	 // Airspeed sensor over-range.
	uint8_t baro_alt_sensor_failure : 1; // Barometric altitude sensor fail.
	uint8_t airspeed_sensor_failure : 1; // Airspeed sensor failure.
	uint8_t res : 2;					 // Reserved (set to zero).
} anpp_air_data_status_t;

/*! @brief ANPP Air Data Packet. */
typedef struct __attribute__((__packed__)) anpp_air_data_t {
	float baro_alt_delay; // Barometric altitude delay [s].
	float airspeed_delay; // Airspeed delay [s].
	float baro_alt;		  // Barometric altitude [m].
	float true_airspeed;  // True Airspeed [m/s].
	float baro_alt_sigma; // Barometric altitude standard deviation [m].
	float airspeed_sigma; // Airspeed standard deviation [m/s].
	anpp_air_data_status_t status; // Air data status.
} anpp_air_data_t;

/*! @brief ANPP Parser state. */
typedef enum anpp_state_t {
	ANPP_STATE_HEADER = 0,	   // Parser is finding Packet Header.
	ANPP_STATE_PACKET_DATA = 1 // Parser is parsing data.
} anpp_state_t;

/*! @brief ANPP device. */
typedef struct anpp_t {
	anpp_hdr_t hdr;					  // Message header.
	uint8_t hdr_idx;				  // Message header index.
	uint8_t arr[ANPP_MAX_PACKET_LEN]; // Packet data.
	uint8_t idx;					  // Packet Data index.
	anpp_state_t state;				  // ANPP Parser state.

	uint8_t id;	 // packet ID.
	uint8_t len; // packet length.

#ifdef ANPP_DBG
	uint32_t drop_cnt; // Number of dropped messages (bad CRC).
	uint32_t pass_cnt; // Number of received messages.
#endif
} anpp_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize ANPP device.
 *
 * @param[in] anpp          ANPP Device.
 * @param[out] raw_sensor   ANPP Raw Sensor Packet.
 * @return 0 - success, 1 - failure
 */
uint32_t anpp_raw_sensors_decode(anpp_t *anpp, anpp_raw_sensor_t *raw_sensor);

/**
 * @brief Initialize ANPP device.
 *
 * @param[in] anpp      ANPP Device.
 * @param[out] air_data ANPP Air Data Packet.
 * @return 0 - success, 1 - failure
 */
uint32_t anpp_air_data_decode(anpp_t *anpp, anpp_air_data_t *air_data);

#ifdef __cplusplus
}
#endif

#endif /* ANPP_COMMON_H */