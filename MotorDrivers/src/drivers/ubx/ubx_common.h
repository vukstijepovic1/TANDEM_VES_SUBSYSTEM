/**
 * @file    ubx_common.h
 * @brief   UBX common types and definitions.
 * @version	1.0.0
 * @date    02.04.2025
 * @author  LisumLab
 */

#ifndef UBX_COMMON_H
#define UBX_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#ifdef UBX_CONFIG
#include "ubx_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef UBX_MAX_PACKET_LEN
#define UBX_MAX_PACKET_LEN 128
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/* packet classes */
#define UBX_PACKET_CLASS_INVALID (uint8_t)0xFFU
#define UBX_PACKET_CLASS_ACK (uint8_t)0x05U
#define UBX_PACKET_CLASS_CFG (uint8_t)0x06U
#define UBX_PACKET_CLASS_INF (uint8_t)0x04U
#define UBX_PACKET_CLASS_LOG (uint8_t)0x21U
#define UBX_PACKET_CLASS_MGA (uint8_t)0x13U
#define UBX_PACKET_CLASS_MON (uint8_t)0x0AU
#define UBX_PACKET_CLASS_NAV (uint8_t)0x01U
#define UBX_PACKET_CLASS_NAV2 (uint8_t)0x29U
#define UBX_PACKET_CLASS_RXM (uint8_t)0x02U
#define UBX_PACKET_CLASS_SEC (uint8_t)0x27U
#define UBX_PACKET_CLASS_TIM (uint8_t)0x0DU
#define UBX_PACKET_CLASS_UPD (uint8_t)0x09U

/* packet ids */
#define UBX_PACKET_ID_INVALID (uint8_t)0xFFU
/* ack packets */
#define UBX_PACKET_ID_ACK_ACK (uint8_t)0x01U
#define UBX_PACKET_ID_ACK_NAK (uint8_t)0x00U
/* cfg packets */
#define UBX_PACKET_ID_CFG_ANT (uint8_t)0x13U
#define UBX_PACKET_ID_CFG_CFG (uint8_t)0x09U
#define UBX_PACKET_ID_CFG_DAT (uint8_t)0x06U
#define UBX_PACKET_ID_CFG_DGNSS (uint8_t)0x70U
#define UBX_PACKET_ID_CFG_GEOFENCE (uint8_t)0x69U
#define UBX_PACKET_ID_CFG_GNSS (uint8_t)0x3EU
#define UBX_PACKET_ID_CFG_INF (uint8_t)0x02U
#define UBX_PACKET_ID_CFG_LOGFILTER (uint8_t)0x47U
#define UBX_PACKET_ID_CFG_MSG (uint8_t)0x01U
#define UBX_PACKET_ID_CFG_NAV5 (uint8_t)0x24U
#define UBX_PACKET_ID_CFG_NAVX5 (uint8_t)0x23U
#define UBX_PACKET_ID_CFG_NMEA (uint8_t)0x17U
#define UBX_PACKET_ID_CFG_ODO (uint8_t)0x1EU
#define UBX_PACKET_ID_CFG_PRT (uint8_t)0x00U
#define UBX_PACKET_ID_CFG_PWR (uint8_t)0x57U
#define UBX_PACKET_ID_CFG_RATE (uint8_t)0x08U
#define UBX_PACKET_ID_CFG_RINV (uint8_t)0x34U
#define UBX_PACKET_ID_CFG_RST (uint8_t)0x04U
#define UBX_PACKET_ID_CFG_SBAS (uint8_t)0x16U
#define UBX_PACKET_ID_CFG_TMODE3 (uint8_t)0x71U
#define UBX_PACKET_ID_CFG_TP5 (uint8_t)0x31U
#define UBX_PACKET_ID_CFG_USB (uint8_t)0x1BU
#define UBX_PACKET_ID_CFG_VALDEL (uint8_t)0x8CU
#define UBX_PACKET_ID_CFG_VALGET (uint8_t)0x8BU
#define UBX_PACKET_ID_CFG_VALSET (uint8_t)0x8AU
/* inf packets */
#define UBX_PACKET_ID_INF_DEBUG (uint8_t)0x04U
#define UBX_PACKET_ID_INF_ERROR (uint8_t)0x00U
#define UBX_PACKET_ID_INF_NOTICE (uint8_t)0x02U
#define UBX_PACKET_ID_INF_TEST (uint8_t)0x03U
#define UBX_PACKET_ID_INF_WARNING (uint8_t)0x01U
/* log packets */
#define UBX_PACKET_ID_LOG_CREATE (uint8_t)0x07U
#define UBX_PACKET_ID_LOG_ERASE (uint8_t)0x03U
#define UBX_PACKET_ID_LOG_FINDTIME (uint8_t)0x0EU
#define UBX_PACKET_ID_LOG_INFO (uint8_t)0x08U
#define UBX_PACKET_ID_LOG_RETRIEVE (uint8_t)0x09U
#define UBX_PACKET_ID_LOG_RETRIEVEPOS (uint8_t)0x0BU
#define UBX_PACKET_ID_LOG_RETRIEVEPOSEXTRA (uint8_t)0x0FU
#define UBX_PACKET_ID_LOG_RETRIEVESTRING (uint8_t)0x0DU
#define UBX_PACKET_ID_LOG_STRING (uint8_t)0x04U
/* mga packets */
#define UBX_PACKET_ID_MGA_ACK (uint8_t)0x60U
#define UBX_PACKET_ID_MGA_BDS (uint8_t)0x03U
#define UBX_PACKET_ID_MGA_DBD (uint8_t)0x80U
#define UBX_PACKET_ID_MGA_GAL (uint8_t)0x02U
#define UBX_PACKET_ID_MGA_GLO (uint8_t)0x06U
#define UBX_PACKET_ID_MGA_GPS (uint8_t)0x00U
#define UBX_PACKET_ID_MGA_INI (uint8_t)0x40U
#define UBX_PACKET_ID_MGA_QZSS (uint8_t)0x05U
/* mon packets */
#define UBX_PACKET_ID_MON_COMMS (uint8_t)0x36U
#define UBX_PACKET_ID_MON_GNSS (uint8_t)0x28U
#define UBX_PACKET_ID_MON_HW (uint8_t)0x09U
#define UBX_PACKET_ID_MON_HW2 (uint8_t)0x0BU
#define UBX_PACKET_ID_MON_HW3 (uint8_t)0x37U
#define UBX_PACKET_ID_MON_IO (uint8_t)0x02U
#define UBX_PACKET_ID_MON_MSGPP (uint8_t)0x06U
#define UBX_PACKET_ID_MON_PATCH (uint8_t)0x27U
#define UBX_PACKET_ID_MON_RF (uint8_t)0x38U
#define UBX_PACKET_ID_MON_RXBUF (uint8_t)0x07U
#define UBX_PACKET_ID_MON_RXR (uint8_t)0x21U
#define UBX_PACKET_ID_MON_SPAN (uint8_t)0x31U
#define UBX_PACKET_ID_MON_SYS (uint8_t)0x39U
#define UBX_PACKET_ID_MON_TXBUF (uint8_t)0x08U
#define UBX_PACKET_ID_MON_VER (uint8_t)0x04U
/* nav packets */
#define UBX_PACKET_ID_NAV_CLOCK (uint8_t)0x22U
#define UBX_PACKET_ID_NAV_COV (uint8_t)0x36U
#define UBX_PACKET_ID_NAV_DOP (uint8_t)0x04U
#define UBX_PACKET_ID_NAV_EOE (uint8_t)0x61U
#define UBX_PACKET_ID_NAV_GEOFENCE (uint8_t)0x39U
#define UBX_PACKET_ID_NAV_HPPOSECEF (uint8_t)0x13U
#define UBX_PACKET_ID_NAV_HPPOSLLH (uint8_t)0x14U
#define UBX_PACKET_ID_NAV_ODO (uint8_t)0x09U
#define UBX_PACKET_ID_NAV_ORB (uint8_t)0x34U
#define UBX_PACKET_ID_NAV_PL (uint8_t)0x62U
#define UBX_PACKET_ID_NAV_POSECEF (uint8_t)0x01U
#define UBX_PACKET_ID_NAV_POSLLH (uint8_t)0x02U
#define UBX_PACKET_ID_NAV_PVT (uint8_t)0x07U
#define UBX_PACKET_ID_NAV_RELPOSNED (uint8_t)0x3CU
#define UBX_PACKET_ID_NAV_RESETODO (uint8_t)0x10U
#define UBX_PACKET_ID_NAV_SAT (uint8_t)0x35U
#define UBX_PACKET_ID_NAV_SBAS (uint8_t)0x32U
#define UBX_PACKET_ID_NAV_SIG (uint8_t)0x43U
#define UBX_PACKET_ID_NAV_SLAS (uint8_t)0x42U
#define UBX_PACKET_ID_NAV_STATUS (uint8_t)0x03U
#define UBX_PACKET_ID_NAV_SVIN (uint8_t)0x3BU
#define UBX_PACKET_ID_NAV_TIMEBDS (uint8_t)0x24U
#define UBX_PACKET_ID_NAV_TIMEGAL (uint8_t)0x25U
#define UBX_PACKET_ID_NAV_TIMEGLO (uint8_t)0x23U
#define UBX_PACKET_ID_NAV_TIMEGPS (uint8_t)0x20U
#define UBX_PACKET_ID_NAV_TIMELS (uint8_t)0x26U
#define UBX_PACKET_ID_NAV_TIMEQZSS (uint8_t)0x27U
#define UBX_PACKET_ID_NAV_TIMETRUSTED (uint8_t)0x64U
#define UBX_PACKET_ID_NAV_TIMEUTC (uint8_t)0x21U
#define UBX_PACKET_ID_NAV_VELECEF (uint8_t)0x11U
#define UBX_PACKET_ID_NAV_VELNED (uint8_t)0x12U
/* nav2 packets */
#define UBX_PACKET_ID_NAV2_CLOCK (uint8_t)0x22U
#define UBX_PACKET_ID_NAV2_COV (uint8_t)0x36U
#define UBX_PACKET_ID_NAV2_DOP (uint8_t)0x04U
#define UBX_PACKET_ID_NAV2_EOE (uint8_t)0x61U
#define UBX_PACKET_ID_NAV2_ODO (uint8_t)0x09U
#define UBX_PACKET_ID_NAV2_POSECEF (uint8_t)0x01U
#define UBX_PACKET_ID_NAV2_POSLLH (uint8_t)0x02U
#define UBX_PACKET_ID_NAV2_PVT (uint8_t)0x07U
#define UBX_PACKET_ID_NAV2_SAT (uint8_t)0x35U
#define UBX_PACKET_ID_NAV2_SBAS (uint8_t)0x32U
#define UBX_PACKET_ID_NAV2_SIG (uint8_t)0x43U
#define UBX_PACKET_ID_NAV2_SLAS (uint8_t)0x42U
#define UBX_PACKET_ID_NAV2_STATUS (uint8_t)0x03U
#define UBX_PACKET_ID_NAV2_SVIN (uint8_t)0x3BU
#define UBX_PACKET_ID_NAV2_TIMEBDS (uint8_t)0x24U
#define UBX_PACKET_ID_NAV2_TIMEGAL (uint8_t)0x25U
#define UBX_PACKET_ID_NAV2_TIMEGLO (uint8_t)0x23U
#define UBX_PACKET_ID_NAV2_TIMEGPS (uint8_t)0x20U
#define UBX_PACKET_ID_NAV2_TIMELS (uint8_t)0x26U
#define UBX_PACKET_ID_NAV2_TIMEQZSS (uint8_t)0x27U
#define UBX_PACKET_ID_NAV2_TIMEUTC (uint8_t)0x21U
#define UBX_PACKET_ID_NAV2_VELECEF (uint8_t)0x11U
#define UBX_PACKET_ID_NAV2_VELNED (uint8_t)0x12U
/* rxm packets */
#define UBX_PACKET_ID_RXM_COR (uint8_t)0x34U
#define UBX_PACKET_ID_RXM_MEASX (uint8_t)0x14U
#define UBX_PACKET_ID_RXM_PMP (uint8_t)0x72U
#define UBX_PACKET_ID_RXM_PMREQ (uint8_t)0x41U
#define UBX_PACKET_ID_RXM_QZSSL6 (uint8_t)0x73U
#define UBX_PACKET_ID_RXM_RAWX (uint8_t)0x15U
#define UBX_PACKET_ID_RXM_RLM (uint8_t)0x59U
#define UBX_PACKET_ID_RXM_RTCM (uint8_t)0x32U
#define UBX_PACKET_ID_RXM_SFRBX (uint8_t)0x13U
#define UBX_PACKET_ID_RXM_SPARTN (uint8_t)0x33U
#define UBX_PACKET_ID_RXM_SPARTNKEY (uint8_t)0x36U
/* sec packets */
#define UBX_PACKET_ID_SEC_OSNMA (uint8_t)0x0AU
#define UBX_PACKET_ID_SEC_SIG (uint8_t)0x09U
#define UBX_PACKET_ID_SEC_SIGLOG (uint8_t)0x10U
#define UBX_PACKET_ID_SEC_UNIQID (uint8_t)0x03U
/* tim packets */
#define UBX_PACKET_ID_TIM_TM2 (uint8_t)0x03U
#define UBX_PACKET_ID_TIM_TP (uint8_t)0x01U
#define UBX_PACKET_ID_TIM_VRFY (uint8_t)0x06U
/* upd packets */
#define UBX_PACKET_ID_UPD_SOS (uint8_t)0x14U

/*! @brief UBX preamble. */
typedef struct __attribute__((__packed__)) ubx_preamble_t {
	/* Preamble sync character 1. */
	uint8_t head1;
	/* Preamble sync character 2. */
	uint8_t head2;
	/* Message class. */
	uint8_t msg_class;
	/* Message ID. */
	uint8_t msg_id;
	/* Length. */
	uint16_t len;
} ubx_preamble_t;

/*! @brief UBX checksum. */
typedef struct __attribute__((__packed__)) ubx_checksum_t {
	/* checksum A. */
	uint8_t ck_a;
	/* checksum B. */
	uint8_t ck_b;
} ubx_checksum_t;

/*! @brief UBX Message acknowledged. */
typedef struct __attribute__((__packed__)) ubx_ack_ack_t {
	/* UBX Preamble. */
	ubx_preamble_t pre;
	/* Class ID of the Acknowledged Message. */
	uint8_t clsID;
	/* Message ID of the Acknowledged Message. */
	uint8_t msgID;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_ack_ack_t;

/*! @brief UBX Message not acknowledged. */
typedef struct __attribute__((__packed__)) ubx_ack_nak_t {
	/* UBX Preamble. */
	ubx_preamble_t pre;
	/* Class ID of the Not-Acknowledged Message. */
	uint8_t clsID;
	/* Message ID of the Not-Acknowledged Message. */
	uint8_t msgID;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_ack_nak_t;

/*! @brief UBX High precision geodetic position solution. */
typedef struct __attribute__((__packed__)) ubx_nav_hpposllh_t {
	/* UBX Preamble. */
	ubx_preamble_t pre;
	/* Message version (0x00 for this version). */
	uint8_t version;
	/* Reserved. */
	uint8_t reserved0[2];
	/* Additional flags: See Interface description. */
	uint8_t flags;
	/*
	 * GPS time of week of the navigation epoch. See the section iTOW timestamps
	 * in Integration manual for details. [ms]
	 */
	uint32_t itow;
	/* Longitude. 1e-7[deg] */
	int32_t lon;
	/* Lattitude. 1e-7[deg] */
	int32_t lat;
	/* Height above ellipsoid. [mm] */
	int32_t height;
	/* Height above mean sea level. [mm] */
	int32_t hmsl;
	/*
	 * High precision component of longitude. Must be in the range -99..+99.
	 * 1e-9[deg]
	 */
	int8_t lonhp;
	/*
	 * High precision component of lattitude. Must be in the range -99..+99.
	 * 1e-9[deg]
	 */
	int8_t lathp;
	/*
	 * High precision component of height above ellipsoid. Must be in the range
	 * -99..+99. 0.1[mm]
	 */
	int8_t heighthp;
	/*
	 * High precision component of height above mean sea level. Must be in the
	 * range -99..+99. 0.1[mm]
	 */
	int8_t hmslhp;
	/* Horizontal accuracy estimate. 0.1[mm] */
	uint32_t hacc;
	/* Vertical accuracy estimate. 0.1[mm] */
	uint32_t vacc;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_nav_hpposllh_t;

/*! @brief UBX Position solution in ECEF. */
typedef struct __attribute__((__packed__)) ubx_nav_posecef_t {
	/* UBX Preamble. */
	ubx_preamble_t pre;
	/*
	 * GPS time of week of the navigation epoch. See the section iTOW timestamps
	 * in Integration manual for details. [ms]
	 */
	uint32_t itow;
	/* ECEF X coordinate. [cm] */
	int32_t ecefx;
	/* ECEF Y coordinate. [cm] */
	int32_t ecefy;
	/* ECEF Z coordinate. [cm] */
	int32_t ecefz;
	/* Position Accuracy Estimate. [cm] */
	uint32_t pacc;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_nav_posecef_t;

/*! @brief UBX Geodetic position solution. */
typedef struct __attribute__((__packed__)) ubx_nav_posllh_t {
	/* UBX Preamble. */
	ubx_preamble_t pre;
	/*
	 * GPS time of week of the navigation epoch. See the section iTOW timestamps
	 * in Integration manual for details. [ms]
	 */
	uint32_t itow;
	/* Longitude. 1e-7[deg] */
	int32_t lon;
	/* Lattitude. 1e-7[deg] */
	int32_t lat;
	/* Height above ellipsoid. [mm] */
	int32_t height;
	/* Height above mean sea level. [mm] */
	int32_t hmsl;
	/* Horizontal accuracy estimate. [mm] */
	uint32_t hacc;
	/* Vertical accuracy estimate. [mm] */
	uint32_t vacc;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_nav_posllh_t;

/*! @brief UBX Navigation position velocity time solution. */
typedef struct __attribute__((__packed__)) ubx_nav_pvt_t {
	/* UBX Preamble. */
	ubx_preamble_t pre;
	/*
	 * GPS time of week of the navigation epoch. See the section iTOW timestamps
	 * in Integration manual for details. [ms]
	 */
	uint32_t itow;
	/* Year (UTC). */
	uint16_t year;
	/* Month, range 1..12 (UTC). */
	uint8_t month;
	/* Day of month, range 1..31 (UTC). */
	uint8_t day;
	/* Hour of day, range 0..23 (UTC). */
	uint8_t hour;
	/* Minute of hour, range 0..59 (UTC). */
	uint8_t min;
	/* Seconds of minute, range 0..60 (UTC). */
	uint8_t sec;
	/* Validity flags: See Interface description. */
	uint8_t valid;
	/* Time accuracy estimate (UTC). [ns] */
	uint32_t tacc;
	/* Fraction of second, range -1e9 .. 1e9 (UTC). [ns] */
	int32_t nano;
	/*
	 * GNSSfix Type:    0 = no fix
	 *                  1 = dead reckoning only
	 *                  2 = 2D-fix
	 *                  3 = 3D-fix
	 *                  4 = GNSS + dead reckoning combined
	 *                  5 = time only fix
	 */
	uint8_t fixtype;
	/* Fix status flags: See Interface description. */
	uint8_t flags;
	/* Additional flags: See Interface description. */
	uint8_t flags2;
	/* Number of satellites used in Nav Solution. */
	uint8_t numsv;
	/* Longitude. 1e-7[deg] */
	int32_t lon;
	/* Lattitude. 1e-7[deg] */
	int32_t lat;
	/* Height above ellipsoid. [mm] */
	int32_t height;
	/* Height above mean sea level. [mm] */
	int32_t hmsl;
	/* Horizontal accuracy estimate. [mm] */
	uint32_t hacc;
	/* Vertical accuracy estimate. [mm] */
	uint32_t vacc;
	/* NED north velocity. [mm/s] */
	int32_t veln;
	/* NED east velocity. [mm/s] */
	int32_t vele;
	/* NED down velocity. [mm/s] */
	int32_t veld;
	/* Ground Speed (2-D). [mm/s] */
	int32_t gspeed;
	/* Heading of motion (2-D). 1e-5[deg] */
	int32_t headmot;
	/* Speed accuracy estimate. [mm/s] */
	uint32_t sacc;
	/* Heading accuracy estimate (both motion and vehicle). 1e-5[deg] */
	uint32_t headacc;
	/* Position DOP. [0.01] */
	uint16_t pdop;
	/* Additional flags: See Interface description. */
	uint8_t flags3;
	/* Reserved. */
	uint8_t reserved0[5];
	/*
	 * Heading of vehicle (2-D), this is only valid when headVehValid is set,
	 * otherwise the output is set to the heading of motion. 1e-5[deg]
	 */
	int32_t headveh;
	/*
	 * Magnetic declination. Only supported in ADR 4.10 and later. 1e-2[deg]
	 */
	int16_t magdec;
	/*
	 * Magnetic declination accuracy. Only supported in ADR 4.10 and later.
	 * 1e-2[deg]
	 */
	uint16_t magacc;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_nav_pvt_t;

/*! @brief UBX Relative positioning information in NED frame. */
typedef struct __attribute__((__packed__)) ubx_nav_relposned_t {
	/* UBX Preamble. */
	ubx_preamble_t pre;
	/* Message version (0x01 for this version). */
	uint8_t version;
	/* Reserved. */
	uint8_t reserved0;
	/* Reference station ID. Must be in the range 0..4095. */
	uint16_t refstationid;
	/*
	 * GPS time of week of the navigation epoch. See the section iTOW timestamps
	 * in Integration manual for details. [ms]
	 */
	uint32_t itow;
	/* North component of relative position vector. [cm] */
	int32_t relposn;
	/* East component of relative position vector. [cm] */
	int32_t relpose;
	/* Down component of relative position vector. [cm] */
	int32_t relposd;
	/* Length of the relative position vector. [cm] */
	int32_t relposlength;
	/* Heading of the relative position vector. 1e-5[deg] */
	int32_t relposheading;
	/* Reserved. */
	uint8_t reserved1[4];
	/*
	 * High-precision North component of relative position vector. Must be in
	 * the range -99 to +99. 0.1[mm]
	 */
	int8_t relposhpn;
	/*
	 * High-precision East component of relative position vector. Must be in the
	 * range -99 to +99. 0.1[mm]
	 */
	int8_t relposhpe;
	/*
	 * High-precision Down component of relative position vector. Must be in the
	 * range -99 to +99. 0.1[mm]
	 */
	int8_t relposhpd;
	/*
	 * High-precision component of the length of the component of relative
	 * position vector. Must be in the range -99 to +99. 0.1[mm]
	 */
	int8_t relposhplength;
	/* Accuracy of relative position North  component. 0.1[mm] */
	uint32_t accn;
	/* Accuracy of relative position East component. 0.1[mm] */
	uint32_t acce;
	/* Accuracy of relative position Down component. 0.1[mm] */
	uint32_t accd;
	/* Accuracy of length of the relative position vector. 0.1[mm] */
	uint32_t acclength;
	/* Accuracy of heading of the relative position vector. 1e-5[deg] */
	uint32_t accheading;
	/* Reserved. */
	uint8_t reserved2[4];
	/* Flags: See Interface description. */
	uint32_t flags;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_nav_relposned_t;

/*! @brief UBX UTC time solution. */
typedef struct __attribute__((__packed__)) ubx_nav_timeutc_t {
	/* UBX Preamble */
	ubx_preamble_t pre;
	/*
	 * GPS time of week of the navigation epoch. See the section iTOW timestamps
	 * in Integration manual for details. [ms]
	 */
	uint32_t itow;
	/* Time accuracy estimate (UTC). [ns] */
	uint32_t tacc;
	/* Fraction of second, range -1e9 .. 1e9 (UTC). [ns] */
	int32_t nano;
	/* Year. */
	uint16_t year;
	/* Month. */
	uint8_t month;
	/* Day. */
	uint8_t day;
	/* Hour. */
	uint8_t hour;
	/* Minute. */
	uint8_t min;
	/* Second. */
	uint8_t sec;
	/* Validity Flags: See Interface description. */
	uint8_t valid;
	/* UBX Checksum. */
	ubx_checksum_t ck;
} ubx_nav_timeutc_t;

/*! @brief UBX parser state enumeration. */
typedef enum ubx_parser_state_e {
	UBX_PARSER_STATE_HEADER1 = 0,
	UBX_PARSER_STATE_HEADER2,
	UBX_PARSER_STATE_CLASS,
	UBX_PARSER_STATE_ID,
	UBX_PARSER_STATE_LENGTH1,
	UBX_PARSER_STATE_LENGTH2,
	UBX_PARSER_STATE_PAYLOAD,
	UBX_PARSER_STATE_CHECKSUM1,
	UBX_PARSER_STATE_CHECKSUM2,
} ubx_parser_state_e;

/*! @brief UBX instance definition. */
typedef struct ubx_t {
	ubx_parser_state_e state;
	uint8_t arr[UBX_MAX_PACKET_LEN];
	uint16_t idx;
	uint8_t packet_class;
	uint8_t packet_id;
	uint16_t length;
	uint16_t recv_checksum;
	ubx_checksum_t checksum;
#ifdef UBX_DBG
	uint16_t pass_cnt;
	uint16_t drop_cnt;
#endif
} ubx_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Decode UBX Message acknowledged.
 *
 * @param[in] inst  UBX packet.
 * @param[out] ack  UBX Message acknowledged.
 * @return None
 */
void ubx_ack_ack_decode(ubx_t *inst, ubx_ack_ack_t *ack);

/**
 * @brief Decode UBX Message not acknowledged.
 *
 * @param[in] inst  UBX instance.
 * @param[out] nak  UBX Message not acknowledged.
 * @return None
 */
void ubx_ack_nak_decode(ubx_t *inst, ubx_ack_nak_t *nak);

/**
 * @brief Decode UBX High precision geodetic position solution.
 *
 * @param[in] inst      UBX instance.
 * @param[out] hpposllh UBX High precision geodetic position solution.
 * @return None
 */
void ubx_nav_hpposllh_decode(ubx_t *inst, ubx_nav_hpposllh_t *hpposllh);

/**
 * @brief Decode UBX Position solution in ECEF.
 *
 * @param[in] inst      UBX instance.
 * @param[out] posecef  UBX Position solution in ECEF.
 * @return None
 */
void ubx_nav_posecef_decode(ubx_t *inst, ubx_nav_posecef_t *posecef);

/**
 * @brief Decode UBX Geodetic position solution.
 *
 * @param[in] inst      UBX instance.
 * @param[out] posllh   UBX Geodetic position solution.
 * @return None
 */
void ubx_nav_posllh_decode(ubx_t *inst, ubx_nav_posllh_t *posllh);

/**
 * @brief Decode UBX Navigation position velocity time solution.
 *
 * @param[in] inst  UBX instance.
 * @param[out] pvt  UBX Navigation position velocity time solution.
 * @return None
 */
void ubx_nav_pvt_decode(ubx_t *inst, ubx_nav_pvt_t *pvt);

/**
 * @brief Decode UBX Relative positioning information in NED frame.
 *
 * @param[in] inst          UBX instance.
 * @param[out] relposned    UBX Relative positioning information in NED frame.
 * @return None
 */
void ubx_nav_relposned_decode(ubx_t *inst, ubx_nav_relposned_t *relposned);

/**
 * @brief Decode UBX UTC time solution.
 *
 * @param[in] inst      UBX instance.
 * @param[out] timeutc  UBX UTC time solution.
 * @return None
 */
void ubx_nav_timeutc_decode(ubx_t *inst, ubx_nav_timeutc_t *timeutc);

#ifdef __cplusplus
}
#endif

#endif /* UBX_COMMON_H */