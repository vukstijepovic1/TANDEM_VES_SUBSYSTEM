/**
 * @file    xbus_def.h
 * @brief   XBUS parser common definitions.
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

#ifndef XBUS_DEF_H
#define XBUS_DEF_H

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define XBUS_DEF_HEADER_SIZE 0x04U
#define XBUS_DEF_MAX_STD_DATA_SIZE 0xFEU
#define XBUS_DEF_CRC_SIZE 0x01U

#define XBUS_DEF_MAX_PACKET_SIZE                                               \
	(XBUS_DEF_HEADER_SIZE + XBUS_DEF_MAX_STD_DATA_SIZE + XBUS_DEF_CRC_SIZE)

#define XBUS_DEF_BID_FIRST_DEV 0x01U
#define XBUS_DEF_BID_MASTER_DEV 0xFFU

#define XBUS_DEF_PREAMBLE_BYTE 0xFAU

/* Message IDs */
#define XBUS_DEF_MTDATA2_MID 0x36U

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/* XBUS Parser state. */
typedef enum xbus_state_t {
	XBUS_STATE_PREAMBLE = 0,
	XBUS_STATE_BID = 1,
	XBUS_STATE_MID = 2,
	XBUS_STATE_LEN = 3,
	XBUS_STATE_DATA = 4,
	XBUS_STATE_CRC = 5
} xbus_state_t;

/* XBUS MTData2 Packet ID. */
typedef enum xbus_xdi_t {
	XBUS_XDI_TEMPERATURE = 0x0810U,
	XBUS_XDI_UTC_TIME = 0x1010U,
	XBUS_XDI_PACKET_COUNTER = 0x1020U,
	XBUS_XDI_SAMPLE_TIME_FINE = 0x1060U,
	XBUS_XDI_SAMPLE_TIME_COARSE = 0x1070U,
	XBUS_XDI_QUATERNION = 0x2010U,
	XBUS_XDI_ROTATION_MATRIX = 0x2020U,
	XBUS_XDI_EULER_ANGLES = 0x2030U,
	XBUS_XDI_BARO_PRESSURE = 0x3010U,
	XBUS_XDI_DELTA_V = 0X4010U,
	XBUS_XDI_ACCELERATION = 0x4020U,
	XBUS_XDI_FREE_ACCELERATION = 0x4030U,
	XBUS_XDI_ACCELERATION_HR = 0x4040U,
	XBUS_XDI_ALTITUDE_ELLIPSOID = 0x5020U,
	XBUS_XDI_POSITION_ECEF = 0x5030U,
	XBUS_XDI_LAT_LON = 0x5040U,
	XBUS_XDI_GNSS_PVT_DATA = 0x7010U,
	// GNSS Sat Data is not implemented!
	XBUS_XDI_GNSS_PVT_PULSE = 0x7030U,
	XBUS_XDI_RATE_OF_TURN = 0x8020U,
	XBUS_XDI_DELTA_Q = 0x8030U,
	XBUS_XDI_RATE_OF_TURN_HR = 0x8040U,
	XBUS_XDI_RAW_ACC_GYR_MAG_TEMP = 0xA010U,
	XBUS_XDI_RAW_GYRO_TEMP = 0xA020U,
	XBUS_XDI_MAGNETIC_FIELD = 0xC020U,
	XBUS_XDI_VELOCITY_XYZ = 0xD010U,
	XBUS_XDI_STATUS_BYTE = 0xE010U,
	XBUS_XDI_STATUS_WORD = 0xE020U,
	// Device ID and Location ID are not implemented!
} xbus_xdi_t;

/* MTData2 packet precision. */
typedef enum xbus_precision_t {
	XBUS_PRECISION_SINGLE = 0x0000U, //  Precision single (float32)
	XBUS_PRECISION_FP1220 = 0x0001U, //  Precision fixed point 12.20
	XBUS_PRECISION_DOUBLE = 0x0003U	 //  Precision double (float64)
} xbus_precision_t;

/* MTData2 packet coordinate system. */
typedef enum xbus_csys_t {
	XBUS_CSYS_ENU = 0x0000U, //  Earth-North-Up
	XBUS_CSYS_NED = 0x0004U, //  North-East-Down
	XBUS_CSYS_NWU = 0x0008U	 //  North-West-Up
} xbus_csys_t;

/* MTData2 Packet Header. */
typedef struct __attribute__((__packed__)) xbus_data_pckt_header_t {
	uint16_t data_id;
	uint8_t data_len;
} xbus_data_pckt_header_t;

/* MTData2 Temperature Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_temperature_float32_t {
	xbus_data_pckt_header_t header;
	float temp; /* [degC] */

} xbus_xdi_temperature_float32_t;

/* MTData2 Temperature Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_temperature_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t temp; /* [degC] · 2^20 */

} xbus_xdi_temperature_fp1220_t;

/* MTData2 Temperature Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_temperature_float64_t {
	xbus_data_pckt_header_t header;
	double temp; /* [degC] */

} xbus_xdi_temperature_float64_t;

/* MTData2 UTC Time Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_utc_time_t {
	xbus_data_pckt_header_t header;
	uint8_t flags;	/* Flags. */
	uint8_t second; /* Second. */
	uint8_t minute; /* Minute. */
	uint8_t hour;	/* Hour. */
	uint8_t day;	/* Day. */
	uint8_t month;	/* Month. */
	uint16_t year;	/* Year. */
	uint32_t ns;	/* Nanoseconds. */

} xbus_xdi_utc_time_t;

/* MTData2 Packet Counter Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_packet_counter_t {
	xbus_data_pckt_header_t header;
	uint16_t packet_counter; /* Incremented with every MTData2 Message */

} xbus_xdi_packet_counter_t;

/* MTData2 Sample Time Fine Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_sample_time_fine_t {
	xbus_data_pckt_header_t header;
	uint32_t sample_time_fine; /*  Contains the sample time of an output
								*   expressed in 10 kHz clock ticks.
								*   When there is no GNSS-fix, this value is
								*   arbitrary for GNSS messages.
								*
								*   This outputs wraps around at:
								*   -   0xFFFFFFFFF
								*       for MTi 1-series and MTi 600-series.
								*   -   Exactly after one day (864000000 ticks)
								*       for MTi 10-series and MTi 100-series.
								*/

} xbus_xdi_sample_time_fine_t;

/* MTData2 Sample Time Coarse Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_sample_time_coarse_t {
	xbus_data_pckt_header_t header;
	uint32_t sample_time_coarse; /*  Contains the sample time of an output
								  *   expressed in seconds.
								  *   When there is no GNSS-fix, this value
								  *   is arbitrary for GNSS messages.
								  */

} xbus_xdi_sample_time_coarse_t;

/* MTData2 Quaternion Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_quaternion_float32_t {
	xbus_data_pckt_header_t header;
	float q3; /* Q3 component of Quaternion. */
	float q2; /* Q2 component of Quaternion. */
	float q1; /* Q1 component of Quaternion. */
	float q0; /* Q0 component of Quaternion. */

} xbus_xdi_quaternion_float32_t;

/* MTData2 Quaternion Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_quaternion_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t q3; /* Q3 component of Quaternion · 2^20. */
	int32_t q2; /* Q2 component of Quaternion · 2^20. */
	int32_t q1; /* Q1 component of Quaternion · 2^20. */
	int32_t q0; /* Q0 component of Quaternion · 2^20. */

} xbus_xdi_quaternion_fp1220_t;

/* MTData2 Quaternion Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_quaternion_float64_t {
	xbus_data_pckt_header_t header;
	double q3; /* Q3 component of Quaternion. */
	double q2; /* Q2 component of Quaternion. */
	double q1; /* Q1 component of Quaternion. */
	double q0; /* Q0 component of Quaternion. */

} xbus_xdi_quaternion_float64_t;

/* MTData2 Rotation Matrix Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rotation_matrix_float32_t {
	xbus_data_pckt_header_t header;
	float i; /* i component of MT rotation matrix (DCM). */
	float h; /* h component of MT rotation matrix (DCM). */
	float g; /* g component of MT rotation matrix (DCM). */
	float f; /* f component of MT rotation matrix (DCM). */
	float e; /* e component of MT rotation matrix (DCM). */
	float d; /* d component of MT rotation matrix (DCM). */
	float c; /* c component of MT rotation matrix (DCM). */
	float b; /* b component of MT rotation matrix (DCM). */
	float a; /* a component of MT rotation matrix (DCM). */

} xbus_xdi_rotation_matrix_float32_t;

/* MTData2 Rotation Matrix Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rotation_matrix_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t i; /* i component of MT rotation matrix (DCM) · 2^20. */
	int32_t h; /* h component of MT rotation matrix (DCM) · 2^20. */
	int32_t g; /* g component of MT rotation matrix (DCM) · 2^20. */
	int32_t f; /* f component of MT rotation matrix (DCM) · 2^20. */
	int32_t e; /* e component of MT rotation matrix (DCM) · 2^20. */
	int32_t d; /* d component of MT rotation matrix (DCM) · 2^20. */
	int32_t c; /* c component of MT rotation matrix (DCM) · 2^20. */
	int32_t b; /* b component of MT rotation matrix (DCM) · 2^20. */
	int32_t a; /* a component of MT rotation matrix (DCM) · 2^20. */

} xbus_xdi_rotation_matrix_fp1220_t;

/* MTData2 Rotation Matrix Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rotation_matrix_float64_t {
	xbus_data_pckt_header_t header;
	double i; /* i component of MT rotation matrix (DCM). */
	double h; /* h component of MT rotation matrix (DCM). */
	double g; /* g component of MT rotation matrix (DCM). */
	double f; /* f component of MT rotation matrix (DCM). */
	double e; /* e component of MT rotation matrix (DCM). */
	double d; /* d component of MT rotation matrix (DCM). */
	double c; /* c component of MT rotation matrix (DCM). */
	double b; /* b component of MT rotation matrix (DCM). */
	double a; /* a component of MT rotation matrix (DCM). */

} xbus_xdi_rotation_matrix_float64_t;

/* MTData2 Euler Angles Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_euler_angles_float32_t {
	xbus_data_pckt_header_t header;
	float yaw;	 /* [deg] */
	float pitch; /* [deg] */
	float roll;	 /* [deg] */

} xbus_xdi_euler_angles_float32_t;

/* MTData2 Euler Angles Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_euler_angles_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t yaw;   /* [deg] · 2^20 */
	int32_t pitch; /* [deg] · 2^20 */
	int32_t roll;  /* [deg] · 2^20 */

} xbus_xdi_euler_angles_fp1220_t;

/* MTData2 Euler Angles Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_euler_angles_float64_t {
	xbus_data_pckt_header_t header;
	double yaw;	  /* [deg] */
	double pitch; /* [deg] */
	double roll;  /* [deg] */

} xbus_xdi_euler_angles_float64_t;

/* MTData2 Baro Pressure Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_baro_pressure_t {
	xbus_data_pckt_header_t header;
	uint32_t pressure; /* [Pa] */

} xbus_xdi_baro_pressure_t;

/* MTData2 Delta V Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_delta_v_float32_t {
	xbus_data_pckt_header_t header;
	float dv_z; /* [m/s] */
	float dv_y; /* [m/s] */
	float dv_x; /* [m/s] */

} xbus_xdi_delta_v_float32_t;

/* MTData2 Delta V Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_delta_v_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t dv_z; /* [m/s] · 2^20 */
	int32_t dv_y; /* [m/s] · 2^20 */
	int32_t dv_x; /* [m/s] · 2^20 */

} xbus_xdi_delta_v_fp1220_t;

/* MTData2 Delta V Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_delta_v_float64_t {
	xbus_data_pckt_header_t header;
	double dv_z; /* [m/s] */
	double dv_y; /* [m/s] */
	double dv_x; /* [m/s] */

} xbus_xdi_delta_v_float64_t;

/* MTData2 Acceleration Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_acceleration_float32_t {
	xbus_data_pckt_header_t header;
	float acc_z; /* [m/s^2] */
	float acc_y; /* [m/s^2] */
	float acc_x; /* [m/s^2] */

} xbus_xdi_acceleration_float32_t;

/* MTData2 Acceleration Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_acceleration_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t acc_z; /* [m/s^2] · 2^20 */
	int32_t acc_y; /* [m/s^2] · 2^20 */
	int32_t acc_x; /* [m/s^2] · 2^20 */

} xbus_xdi_acceleration_fp1220_t;

/* MTData2 Acceleration Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_acceleration_float64_t {
	xbus_data_pckt_header_t header;
	double acc_z; /* [m/s^2] */
	double acc_y; /* [m/s^2] */
	double acc_x; /* [m/s^2] */

} xbus_xdi_acceleration_float64_t;

/* MTData2 Free Acceleration Packet - Single Precision. */
typedef struct __attribute__((__packed__))
xbus_xdi_free_acceleration_float32_t {
	xbus_data_pckt_header_t header;
	float free_acc_z; /* [m/s^2] */
	float free_acc_y; /* [m/s^2] */
	float free_acc_x; /* [m/s^2] */

} xbus_xdi_free_acceleration_float32_t;

/* MTData2 Free Acceleration Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_free_acceleration_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t free_acc_z; /* [m/s^2] · 2^20 */
	int32_t free_acc_y; /* [m/s^2] · 2^20 */
	int32_t free_acc_x; /* [m/s^2] · 2^20 */

} xbus_xdi_free_acceleration_fp1220_t;

/* MTData2 Free Acceleration Packet - Double Precision. */
typedef struct __attribute__((__packed__))
xbus_xdi_free_acceleration_float64_t {
	xbus_data_pckt_header_t header;
	double free_acc_z; /* [m/s^2] */
	double free_acc_y; /* [m/s^2] */
	double free_acc_x; /* [m/s^2] */

} xbus_xdi_free_acceleration_float64_t;

/* MTData2 Acceleration HR Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_acceleration_hr_float32_t {
	xbus_data_pckt_header_t header;
	float acc_z; /* [m/s^2] */
	float acc_y; /* [m/s^2] */
	float acc_x; /* [m/s^2] */

} xbus_xdi_acceleration_hr_float32_t;

/* MTData2 Acceleration HR Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_acceleration_hr_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t acc_z; /* [m/s^2] · 2^20 */
	int32_t acc_y; /* [m/s^2] · 2^20 */
	int32_t acc_x; /* [m/s^2] · 2^20 */

} xbus_xdi_acceleration_hr_fp1220_t;

/* MTData2 Acceleration HR Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_acceleration_hr_float64_t {
	xbus_data_pckt_header_t header;
	double acc_z; /* [m/s^2] */
	double acc_y; /* [m/s^2] */
	double acc_x; /* [m/s^2] */

} xbus_xdi_acceleration_hr_float64_t;

/* MTData2 Altitude Ellipsoid Packet - Single Precision. */
typedef struct __attribute__((__packed__))
xbus_xdi_altitude_ellipsoid_float32_t {
	xbus_data_pckt_header_t header;
	float alt_ellipsoid; /* [m] */

} xbus_xdi_altitude_ellipsoid_float32_t;

/* MTData2 Altitude Ellipsoid Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__))
xbus_xdi_altitude_ellipsoid_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t alt_ellipsoid; /* [m] · 2^20 */

} xbus_xdi_altitude_ellipsoid_fp1220_t;

/* MTData2 Altitude Ellipsoid Packet - Double Precision. */
typedef struct __attribute__((__packed__))
xbus_xdi_altitude_ellipsoid_float64_t {
	xbus_data_pckt_header_t header;
	double alt_ellipsoid; /* [m] */

} xbus_xdi_altitude_ellipsoid_float64_t;

/* MTData2 Position ECEF Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_position_ecef_float32_t {
	xbus_data_pckt_header_t header;
	float ecef_z; /* [m] */
	float ecef_y; /* [m] */
	float ecef_x; /* [m] */

} xbus_xdi_position_ecef_float32_t;

/* MTData2 Position ECEF Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_position_ecef_float64_t {
	xbus_data_pckt_header_t header;
	double ecef_z; /* [m] */
	double ecef_y; /* [m] */
	double ecef_x; /* [m] */

} xbus_xdi_position_ecef_float64_t;

/* MTData2 Lat Lon Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_lat_lon_float32_t {
	xbus_data_pckt_header_t header;
	float lon; /* [deg] */
	float lat; /* [deg] */

} xbus_xdi_lat_lon_float32_t;

/* MTData2 Lat Lon Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_lat_lon_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t lon; /* [deg] · 2^20 */
	int32_t lat; /* [deg] · 2^20 */

} xbus_xdi_lat_lon_fp1220_t;

/* MTData2 Lat Lon Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_lat_lon_float64_t {
	xbus_data_pckt_header_t header;
	double lon; /* [deg] */
	double lat; /* [deg] */

} xbus_xdi_lat_lon_float64_t;

/* MTData2 GNSS PVT Data Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_gnss_pvt_data_t {
	xbus_data_pckt_header_t header;
	uint16_t edop;	   /* Easting DOP, · 10^2 */
	uint16_t ndop;	   /* Northing DOP, · 10^2 */
	uint16_t hdop;	   /* Horizontal DOP, · 10^2 */
	uint16_t vdop;	   /* Vertical DOP, · 10^2 */
	uint16_t tdop;	   /* Time DOP, · 10^2 */
	uint16_t pdop;	   /* Position DOP, · 10^2 */
	uint16_t gdop;	   /* Geometric DOP, · 10^2 */
	int32_t head_veh;  /* 2D heading of vehicle, [deg] · 10^5 */
	uint32_t head_acc; /* Heading accuracy estimate (both motion and vehicle),
						*  [deg] · 10^5 */
	uint32_t s_acc;	   /* Speed accuracy estimate, [mm/s] */
	int32_t head_mot;  /* 2D heading of motion, [deg] · 10^5 */
	int32_t g_speed;   /* 2D ground speed, [mm/s] */
	int32_t vel_d;	   /* NED down velocity, [mm/s] */
	int32_t vel_e;	   /* NED east velocity, [mm/s] */
	int32_t vel_n;	   /* NED north velocity, [mm/s] */
	uint32_t v_acc;	   /* Vertical accuracy estimate, [mm] */
	uint32_t h_acc;	   /* Horizontal accuracy estimate, [mm] */
	int32_t h_msl;	   /* Height above mean sea level, [mm] */
	int32_t height;	   /* Height above ellipsoid, [mm] */
	int32_t lat;	   /* Latitude, [deg] · 10^7 */
	int32_t lon;	   /* Longitude, [deg] · 10^7 */
	uint8_t res1;
	uint8_t num_sv;	  /* Number of satellites used in navigation solution */
	uint8_t flags;	  /*  Fix Status Flags:
					   *       bit (0) = valid fix (within DOP and
					   *                            accuracy masks)
					   *       bit (1) = differential corrections are applied
					   *       bit (2..4) = reserved (ignore)
					   *       bit (5) = heading of vehicle is valid
					   */
	uint8_t fix_type; /*  GNSS fix type (range 0..5):
					   *       0x00 = No Fix
					   *       0x01 = Dead Reckoning only
					   *       0x02 = 2D-Fix
					   *       0x03 = 3D-Fix
					   *       0x04 = GNSS + dead reckoning
					   *       combined
					   *       0x05 = Time only fix
					   *       0x06..0xFF: reserved
					   */
	int32_t nano;	  /* Fraction of second -1e-9 .. 1e-9, [ns] */
	uint32_t t_acc;	  /* Time accuracy estimate (UTC), [ns] */
	uint8_t valid;	  /*  Validity flags:
					   *       bit (0) = UTC Date is valid
					   *       bit (1) = UTC Time of Day is valid
					   *       bit (2) = UTC Time of Day has been fully
					   *               resolved (i.e. no seconds uncertainty)
					   */
	uint8_t sec;	  /* Seconds of minute 0..60 (UTC), [s] */
	uint8_t min;	  /* Minute of hour 0..59 (UTC), [min] */
	uint8_t hour;	  /* Hour of the day 0..23 (UTC), [h] */
	uint8_t day;	  /* Day of the month (UTC), [d] */
	uint8_t month;	  /* Month (UTC), [m] */
	uint16_t year;	  /* Year (UTC), [y] */
	uint32_t itow;	  /* GPS time of week, [ms] */

} xbus_xdi_gnss_pvt_data_t;

/* MTData2 GNSS PVT Pulse Packet. */
typedef struct xbus_xdi_gnss_pvt_pulse_t {
	xbus_data_pckt_header_t header;
	uint32_t gnss_pvt_pulse; /*  Sample time of the PVT data sample expressed
							  *   in 10 kHz clock ticks.
							  *   This output is in the same clock domain as
							  *   the sampleTimeFine and can be used to relate
							  *   measurement samples to PVT samples in time.
							  */

} xbus_xdi_gnss_pvt_pulse_t;

/* MTData2 Rate of Turn Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rate_of_turn_float32_t {
	xbus_data_pckt_header_t header;
	float gyr_z; /* [rad/s] */
	float gyr_y; /* [rad/s] */
	float gyr_x; /* [rad/s] */

} xbus_xdi_rate_of_turn_float32_t;

/* MTData2 Rate of Turn Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rate_of_turn_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t gyr_z; /* [rad/s] · 2^20 */
	int32_t gyr_y; /* [rad/s] · 2^20 */
	int32_t gyr_x; /* [rad/s] · 2^20 */

} xbus_xdi_rate_of_turn_fp1220_t;

/* MTData2 Rate of Turn Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rate_of_turn_float64_t {
	xbus_data_pckt_header_t header;
	double gyr_z; /* [rad/s] */
	double gyr_y; /* [rad/s] */
	double gyr_x; /* [rad/s] */

} xbus_xdi_rate_of_turn_float64_t;

/* MTData2 Delta Q Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_delta_q_float32_t {
	xbus_data_pckt_header_t header;
	float dq3; /* Delta Quaternion value of SDI output */
	float dq2; /* Delta Quaternion value of SDI output */
	float dq1; /* Delta Quaternion value of SDI output */
	float dq0; /* Delta Quaternion value of SDI output */

} xbus_xdi_delta_q_float32_t;

/* MTData2 Delta Q Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_delta_q_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t dq3; /* Delta Quaternion value of SDI output, · 2^20 */
	int32_t dq2; /* Delta Quaternion value of SDI output, · 2^20 */
	int32_t dq1; /* Delta Quaternion value of SDI output, · 2^20 */
	int32_t dq0; /* Delta Quaternion value of SDI output, · 2^20 */

} xbus_xdi_delta_q_fp1220_t;

/* MTData2 Delta Q Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_delta_q_float64_t {
	xbus_data_pckt_header_t header;
	double dq3; /* Delta Quaternion value of SDI output */
	double dq2; /* Delta Quaternion value of SDI output */
	double dq1; /* Delta Quaternion value of SDI output */
	double dq0; /* Delta Quaternion value of SDI output */

} xbus_xdi_delta_q_float64_t;

/* MTData2 Rate of Turn HR Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rate_of_turn_hr_float32_t {
	xbus_data_pckt_header_t header;
	float gyr_z; /* [rad/s] */
	float gyr_y; /* [rad/s] */
	float gyr_x; /* [rad/s] */

} xbus_xdi_rate_of_turn_hr_float32_t;

/* MTData2 Rate of Turn HR Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rate_of_turn_hr_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t gyr_z; /* [rad/s] · 2^20 */
	int32_t gyr_y; /* [rad/s] · 2^20 */
	int32_t gyr_x; /* [rad/s] · 2^20 */

} xbus_xdi_rate_of_turn_hr_fp1220_t;

/* MTData2 Rate of Turn HR Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_rate_of_turn_hr_float64_t {
	xbus_data_pckt_header_t header;
	double gyr_z; /* [rad/s] */
	double gyr_y; /* [rad/s] */
	double gyr_x; /* [rad/s] */

} xbus_xdi_rate_of_turn_hr_float64_t;

/* MTData2 Raw Acc Gyr Mag Temp Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_raw_acc_gyr_mag_temp_t {
	xbus_data_pckt_header_t header;
	int16_t temp;	 /* [degC] · 2^8 */
	uint16_t mag_z;	 /* Raw */
	uint16_t mag_y;	 /* Raw */
	uint16_t mag_x;	 /* Raw */
	uint16_t gyro_z; /* Raw */
	uint16_t gyro_y; /* Raw */
	uint16_t gyro_x; /* Raw */
	uint16_t acc_z;	 /* Raw */
	uint16_t acc_y;	 /* Raw */
	uint16_t acc_x;	 /* Raw */

} xbus_xdi_raw_acc_gyr_mag_temp_t;

/* MTData2 Raw Gyro Temp Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_raw_gyro_temp_t {
	xbus_data_pckt_header_t header;
	int16_t temp_gyr_z; /* [degC] · 2^8 */
	int16_t temp_gyr_y; /* [degC] · 2^8 */
	int16_t temp_gyr_x; /* [degC] · 2^8 */

} xbus_xdi_raw_gyro_temp_t;

/* MTData2 Magnetic Field Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_magnetic_field_float32_t {
	xbus_data_pckt_header_t header;
	float mag_z; /* Arbitrary units */
	float mag_y; /* Arbitrary units */
	float mag_x; /* Arbitrary units */

} xbus_xdi_magnetic_field_float32_t;

/* MTData2 Magnetic Field Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_magnetic_field_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t mag_z; /* Arbitrary units · 2^20 */
	int32_t mag_y; /* Arbitrary units · 2^20 */
	int32_t mag_x; /* Arbitrary units · 2^20 */

} xbus_xdi_magnetic_field_fp1220_t;

/* MTData2 Magnetic Field Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_magnetic_field_float64_t {
	xbus_data_pckt_header_t header;
	double mag_z; /* Arbitrary units */
	double mag_y; /* Arbitrary units */
	double mag_x; /* Arbitrary units */

} xbus_xdi_magnetic_field_float64_t;

/* MTData2 Velocity XYZ Packet - Single Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_velocity_xyz_float32_t {
	xbus_data_pckt_header_t header;
	float vel_z; /* [m/s] */
	float vel_y; /* [m/s] */
	float vel_x; /* [m/s] */

} xbus_xdi_velocity_xyz_float32_t;

/* MTData2 Velocity XYZ Packet - Fixed Point 12.20 Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_velocity_xyz_fp1220_t {
	xbus_data_pckt_header_t header;
	int32_t vel_z; /* [m/s] · 2^20 */
	int32_t vel_y; /* [m/s] · 2^20 */
	int32_t vel_x; /* [m/s] · 2^20 */

} xbus_xdi_velocity_xyz_fp1220_t;

/* MTData2 Velocity XYZ Packet - Double Precision. */
typedef struct __attribute__((__packed__)) xbus_xdi_velocity_xyz_float64_t {
	xbus_data_pckt_header_t header;
	double vel_z; /* [m/s] */
	double vel_y; /* [m/s] */
	double vel_x; /* [m/s] */

} xbus_xdi_velocity_xyz_float64_t;

/* MTData2 Status Byte Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_status_byte_t {
	xbus_data_pckt_header_t header;
	uint8_t status_byte; /* StatusWord.bits[0..7] */

} xbus_xdi_status_byte_t;

/* MTData2 Status Word Packet. */
typedef struct __attribute__((__packed__)) xbus_xdi_status_word_t {
	xbus_data_pckt_header_t header;
	uint32_t status_word; /* Refer to MT_Low-Level_Documentation Table 27. */

} xbus_xdi_status_word_t;

/* Union which shall contain MTData2 packets. */
typedef union xbus_xdi_packet_t {
	xbus_data_pckt_header_t header;

	xbus_xdi_temperature_float32_t temperature_float32;
	xbus_xdi_temperature_fp1220_t temperature_fp1220;
	xbus_xdi_temperature_float64_t temperature_float64;

	xbus_xdi_utc_time_t utc_time;

	xbus_xdi_packet_counter_t packet_counter;

	xbus_xdi_sample_time_fine_t sample_time_fine;

	xbus_xdi_sample_time_coarse_t sample_time_coarse;

	xbus_xdi_quaternion_float32_t quaternion_float32;
	xbus_xdi_quaternion_fp1220_t quaternion_fp1220;
	xbus_xdi_quaternion_float64_t quaternion_float64;

	xbus_xdi_rotation_matrix_float32_t rotation_matrix_float32;
	xbus_xdi_rotation_matrix_fp1220_t rotation_matrix_fp1220;
	xbus_xdi_rotation_matrix_float64_t rotation_matrix_float64;

	xbus_xdi_euler_angles_float32_t euler_angles_float32;
	xbus_xdi_euler_angles_fp1220_t euler_angles_fp1220;
	xbus_xdi_euler_angles_float64_t euler_angles_float64;

	xbus_xdi_baro_pressure_t baro_pressure;

	xbus_xdi_delta_v_float32_t delta_v_float32;
	xbus_xdi_delta_v_fp1220_t delta_v_fp1220;
	xbus_xdi_delta_v_float64_t delta_v_float64;

	xbus_xdi_acceleration_float32_t acceleration_float32;
	xbus_xdi_acceleration_fp1220_t acceleration_fp1220;
	xbus_xdi_acceleration_float64_t acceleration_float64;

	xbus_xdi_free_acceleration_float32_t free_acceleration_float32;
	xbus_xdi_free_acceleration_fp1220_t free_acceleration_fp1220;
	xbus_xdi_free_acceleration_float64_t free_acceleration_float64;

	xbus_xdi_acceleration_hr_float32_t acceleration_hr_float32;
	xbus_xdi_acceleration_hr_fp1220_t acceleration_hr_fp1220;
	xbus_xdi_acceleration_hr_float64_t acceleration_hr_float64;

	xbus_xdi_altitude_ellipsoid_float32_t altitude_ellipsoid_float32;
	xbus_xdi_altitude_ellipsoid_fp1220_t altitude_ellipsoid_fp1220;
	xbus_xdi_altitude_ellipsoid_float64_t altitude_ellipsoid_float64;

	xbus_xdi_position_ecef_float32_t position_ecef_float32;
	xbus_xdi_position_ecef_float64_t position_ecef_float64;

	xbus_xdi_lat_lon_float32_t lat_lon_float32;
	xbus_xdi_lat_lon_fp1220_t lat_lon_fp1220;
	xbus_xdi_lat_lon_float64_t lat_lon_float64;

	xbus_xdi_gnss_pvt_data_t gnss_pvt_data;

	xbus_xdi_gnss_pvt_pulse_t gnss_pvt_pulse;

	xbus_xdi_rate_of_turn_float32_t rate_of_turn_float32;
	xbus_xdi_rate_of_turn_fp1220_t rate_of_turn_fp1220;
	xbus_xdi_rate_of_turn_float64_t rate_of_turn_float64;

	xbus_xdi_delta_q_float32_t delta_q_float32;
	xbus_xdi_delta_q_fp1220_t delta_q_fp1220;
	xbus_xdi_delta_q_float64_t delta_q_float64;

	xbus_xdi_rate_of_turn_hr_float32_t rate_of_turn_hr_float32;
	xbus_xdi_rate_of_turn_hr_fp1220_t rate_of_turn_hr_fp1220;
	xbus_xdi_rate_of_turn_hr_float64_t rate_of_turn_hr_float64;

	xbus_xdi_raw_acc_gyr_mag_temp_t raw_acc_gyr_mag_temp;

	xbus_xdi_raw_gyro_temp_t raw_gyro_temp;

	xbus_xdi_magnetic_field_float32_t magnetic_field_float32;
	xbus_xdi_magnetic_field_fp1220_t magnetic_field_fp1220;
	xbus_xdi_magnetic_field_float64_t magnetic_field_float64;

	xbus_xdi_velocity_xyz_float32_t velocity_xyz_float32;
	xbus_xdi_velocity_xyz_fp1220_t velocity_xyz_fp1220;
	xbus_xdi_velocity_xyz_float64_t velocity_xyz_float64;

	xbus_xdi_status_byte_t status_byte;

	xbus_xdi_status_word_t status_word;

} xbus_xdi_packet_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#endif /* XBUS_DEF_H */