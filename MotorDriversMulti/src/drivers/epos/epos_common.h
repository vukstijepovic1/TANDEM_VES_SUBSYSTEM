/**
 * @file    epos_common.h
 * @brief   EPOS common types and definitions
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

#ifndef EPOS_COMMON_H
#define EPOS_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"
#include "util.h"

#include "canopen.h"

#include "epos_def.h"

#ifdef EPOS_CONFIG
#include "epos_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* Maximum number of actuators */
#ifndef EPOS_MAX_TRACK_CNT
#define EPOS_MAX_TRACK_CNT 8U
#endif

/* Communication timeout [ms] */
#ifndef EPOS_COMM_TIMEOUT_MS
#define EPOS_COMM_TIMEOUT_MS 5
#endif

/* Save to EEPROM timeout [ms] */
#ifndef EPOS_EEPROM_TIMEOUT_MS
#define EPOS_EEPROM_TIMEOUT_MS 1000
#endif

/* Maximum number of retries during EPOS state change */
#ifndef EPOS_ENTER_STATE_MAX_RETRY
#define EPOS_ENTER_STATE_MAX_RETRY 100
#endif

/* Maximum number of retries during EPOS Operation Mode change */
#ifndef EPOS_ENTER_OP_MODE_MAX_RETRY
#define EPOS_ENTER_OP_MODE_MAX_RETRY 100
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief EPOS4 state. */
typedef enum epos_state_e {
	//!< Drive Function is disabled
	EPOS_STATE_NOT_READY_TO_SWITCH_ON = (uint16_t)0x0000,
	//!< Drive initialization is complete.
	//   Drive paramters may be changed.
	//   Drive function is disabled.
	EPOS_STATE_SWITCH_ON_DISABLED = (uint16_t)0x0040,
	//!< Drive parameters may be changed.
	//   Drive function is disabled.
	EPOS_STATE_READY_TO_SWITCH_ON = (uint16_t)0x0021,
	//!< Drive function is disabled.
	//   Current offset calibration done.
	EPOS_STATE_SWITCHED_ON = (uint16_t)0x0023,
	//!< No faults have been detected.
	//   Drive function is enabled and power is applied to the motor.
	EPOS_STATE_OPERATION_ENABLED = (uint16_t)0x0027,
	//!< <<Quick stop>> function is being executed.
	//   Drive function is enabled and power is applied to the motor.
	EPOS_STATE_QUICK_STOP_ACTIVE = (uint16_t)0x0007,
	//!< A fault has occurred in the drive.
	//   Selected fault reaction is being executed.
	EPOS_STATE_FAULT_REACTION_ACTIVE = (uint16_t)0x000F,
	//!< A fault has occured in the drive.
	//   Drive parameters may have changed.
	//   Drive function is disabled.
	EPOS_STATE_FAULT = (uint16_t)0x0008

} epos_state_e;

/*! @brief EPOS Modes of operation. */
typedef enum epos_op_mode_e {
	EPOS_OP_MODE_PP = (int8_t)1,  //!< Profile Position Mode
	EPOS_OP_MODE_PV = (int8_t)3,  //!< Profile Velocity Mode
	EPOS_OP_MODE_HM = (int8_t)6,  //!< Homing Mode
	EPOS_OP_MODE_CSP = (int8_t)8, //!< Cyclic Synchronous Position Mode
	EPOS_OP_MODE_CSV = (int8_t)9, //!< Cyclic Synchronous Velocity Mode
	EPOS_OP_MODE_CST = (int8_t)10 //!< Cyclic Synchronous Torque Mode
} epos_op_mode_e;

/*! @brief EPOS4 unit prefix. */
typedef enum epos_unit_prefix_e {
	EPOS_UNIT_PREFIX_NONE = (uint8_t)0x00,					 //!< 1e0
	EPOS_UNIT_PREFIX_ONE_TENTH = (uint8_t)0xFF,				 //!< 1e-1
	EPOS_UNIT_PREFIX_ONE_HUNDREDTH = (uint8_t)0xFE,			 //!< 1e-2
	EPOS_UNIT_PREFIX_ONE_THOUSANDTH = (uint8_t)0xFD,		 //!< 1e-3
	EPOS_UNIT_PREFIX_ONE_TEN_THOUSANDTH = (uint8_t)0xFC,	 //!< 1e-4
	EPOS_UNIT_PREFIX_ONE_HUNDRED_THOUSANDTH = (uint8_t)0xFB, //!< 1e-5
	EPOS_UNIT_PREFIX_ONE_MILIONTH = (uint8_t)0xFA,			 //!< 1e-6
} epos_unit_prefix_e;

/*! @brief EPOS4 sensor1 type. */
typedef enum epos_sensor1_type_e {
	//!< None
	EPOS_SENSOR1_TYPE_NONE = (uint8_t)0x00,
	//!< Digital incremental encoder 1
	EPOS_SENSOR1_TYPE_DIGITAL_INC_ENC1 = (uint8_t)0x01
} epos_sensor1_type_e;

/*! @brief EPOS4 sensor2 type. */
typedef enum epos_sensor2_type_e {
	//!< None
	EPOS_SENSOR2_TYPE_NONE = (uint8_t)0x00,
	//!< Digital incremental encoder 2
	EPOS_SENSOR2_TYPE_DIGITAL_INC_ENC2 = (uint8_t)0x01,
	//!< Analog incremental encoder
	EPOS_SENSOR2_TYPE_ANALOG_INC_ENC = (uint8_t)0x02,
	//!< SSI absolute encoder
	EPOS_SENSOR2_TYPE_SSI_ABS_ENC = (uint8_t)0x03
} epos_sensor2_type_e;

/*! @brief EPOS4 sensor3 type. */
typedef enum epos_sensor3_type_e {
	//!< None
	EPOS_SENSOR3_TYPE_NONE = (uint8_t)0x00,
	//!< Digital Hall Sensor (EC motors only)
	EPOS_SENSOR3_TYPE_DIGITAL_HALL_SENSOR = (uint8_t)0x10
} epos_sensor3_type_e;

/*! @brief EPOS4 sensor. */
typedef enum epos_sensor_e {
	EPOS_SENSOR_NONE = 0,
	EPOS_SENSOR1 = 1,
	EPOS_SENSOR2 = 2,
	EPOS_SENSOR3 = 3
} epos_sensor_e;

/*! @brief EPOS4 sensor method. */
typedef enum epos_sensor_method_e {
	//!< Speed measured as time between two consecutive sensor edges - Default
	EPOS_SENSOR_METHOD1 = 0,
	//!< Speed measured as number of sensor edges per control cycle
	EPOS_SENSOR_METHOD2 = 1
} epos_sensor_method_e;

/*! @brief EPOS4 sensor direction. */
typedef enum epos_sensor_direction_e {
	//!< Maxon
	EPOS_SENSOR_DIRECTION_MAXON = 0,
	//!< Inverted (or encoder mounted on motor shaft)
	EPOS_SENSOR_DIRECTION_INVERTED = 1
} epos_sensor_direction_e;

/*! @brief EPOS4 encoder type. */
typedef enum epos_enc_type_e {
	//!< Encoder without index (2-channel)
	EPOS_ENC_TYPE_WITHOUT_INDEX = 0,
	//!< Encoder with index (3-channel)
	EPOS_ENC_TYPE_WITH_INDEX = 1,
} epos_enc_type_e;

/*! @brief EPOS4 SSI encoding type. */
typedef enum epos_ssi_encoding_type_t {
	EPOS_SSI_ENCODING_TYPE_BINARY = 0,
	EPOS_SSI_ENCODING_TYPE_GRAY = 1,
} epos_ssi_encoding_type_t;

/*! @brief EPOS4 motor type. */
typedef enum epos_motor_type_e {
	//!< Brushed DC motor (maxon DC motor)
	EPOS_MOTOR_TYPE_PM_DC = 1,
	//!< Brushless DC motor BLDC sinus commutated (maxon EC motor)
	EPOS_MOTOR_TYPE_SIN_PM_BL = 10,
	//!< Brushless DC motor BLDC block commutated (maxon EC motor)
	EPOS_MOTOR_TYPE_TRAP_PM_BL = 11
} epos_motor_type_e;

/*! @brief EPOS4 commutation sensor - absolute. */
typedef enum epos_commutation_sensor_abs_e {
	EPOS_COMMUTATION_SENSOR_ABS_SENSOR_NONE = 0,
	EPOS_COMMUTATION_SENSOR_ABS_SENSOR2 = 2,
	EPOS_COMMUTATION_SENSOR_ABS_SENSOR3 = 3
} epos_commutation_sensor_abs_e;

/*! @brief EPOS4 commutation sensor - relative. */
typedef enum epos_commutation_sensor_rel_e {
	EPOS_COMMUTATION_SENSOR_REL_SENSOR_NONE = 0,
	EPOS_COMMUTATION_SENSOR_REL_SENSOR1 = 1,
	EPOS_COMMUTATION_SENSOR_REL_SENSOR2 = 2,
	EPOS_COMMUTATION_SENSOR_REL_SENSOR3 = 3
} epos_commutation_sensor_rel_e;

/*! @brief EPOS4 unit configuration. */
typedef struct epos_unit_cfg_t {
	//!< Position Unit Prefix (only 1e0 allowed)
	//   Resulting in unit being [incr]
	epos_unit_prefix_e pos_prefix;
	//!< Velocity Unit Prefix
	//   Resulting in unit being [rev/min] * vel_prefix
	epos_unit_prefix_e vel_prefix;
	//!< Acceleration Unit Prefix (only 1e0 allowed)
	//   Resulting in unit being [(rev/min)/s]
	epos_unit_prefix_e accel_prefix;

} epos_unit_cfg_t;

/*! @brief EPOS4 sensor configuration. */
typedef struct epos_sensor_cfg_t {
	//!< Sensor1 Type
	epos_sensor1_type_e sensor1_type;
	//!< Sensor2 Type
	epos_sensor2_type_e sensor2_type;
	//!< Sensor3 Type
	epos_sensor3_type_e sensor3_type;
	//!< Main sensor
	epos_sensor_e main_sensor;
	//!< Auxiliary sensor
	epos_sensor_e aux_sensor;

} epos_sensor_cfg_t;

/*! @brief EPOS4 digital incremental encoder configuration. */
typedef struct epos_dig_inc_enc_cfg_t {
	//!< Counts per turn
	uint32_t pulses;
	//!< Digital Incremental encoder type
	epos_enc_type_e type;
	//!< Sensor Speed Calculation method
	epos_sensor_method_e method;
	//!< Sensor Polarity
	epos_sensor_direction_e direction;

} epos_dig_inc_enc_cfg_t;

/*! @brief EPOS4 SSI absolute encoder configuration. */
typedef struct epos_ssi_abs_enc_cfg_t {
	//!< Data rate (clock frequency) [kbit/s]
	uint16_t data_rate;
	//!< Number of leading special bits
	uint8_t special_bits_leading;
	//!< Number of multi-turn bits
	uint8_t multi_turn_bits;
	//!< Number of single-turn bits
	uint8_t single_turn_bits;
	//!< Number of trailing special bits
	uint8_t special_bits_trailing;
	//!< Sensor direction
	epos_sensor_direction_e direction;
	//!< Sensor encoding type
	epos_ssi_encoding_type_t encoding_type;
	//!< Minimum duration after lack clock edge of a sequence until the first
	//   clock edge of the next sequence [us].
	uint16_t timeout;
	//!< Duration from power-up until the SSI encoder is initialized and ready
	//   for operation [ms]
	uint16_t power_up_time;
	//!< Number of multi turn bits used, must not exceed 32 when combined with
	//   single turn bits.
	uint8_t multi_turn_bits_used;
	//!< Number of single turn bits used, must not exceed 32 when combined with
	//   multi turn bits.
	uint8_t single_turn_bits_used;

} epos_ssi_abs_enc_cfg_t;

/*! @brief EPOS4 hall sensor configuration. */
typedef struct epos_hall_sensor_cfg_t {
	//!< Sensor Speed Calculation method
	epos_sensor_method_e method;
	//!< Sensor Polarity
	epos_sensor_direction_e polarity;
} epos_hall_sensor_cfg_t;

/*! @brief EPOS4 motor configuration. */
typedef struct epos_motor_cfg_t {
	//!< Nominal current of the motor [mA]
	uint32_t nom_curr;
	//!< Maximum permissible current of the motor [mA]
	//   Recommended double the value of Nominal Current
	uint32_t curr_limit;
	//!< Number of magnetic pole pairs of the rotor of the brushless DC motor
	uint8_t pp_num;
	//!< Thermal time constant of motor winding [0.1s]
	uint16_t tt_constant;
	//!< Torque constant [uNm/A]
	uint32_t torque_constant;
	//!< Motor Type
	epos_motor_type_e type;
	//!< Indicated the configured maximum allowed speed for the motor [rpm]
	uint32_t max_speed;

} epos_motor_cfg_t;

/*! @brief EPOS4 configuration. */
typedef struct epos_cfg_t {
	//!< Units configuration
	epos_unit_cfg_t unit_cfg;
	//!< Actuator sensors configuration
	epos_sensor_cfg_t sensor_cfg;
	//!< Digital Incremental Encoder 1 Configuration
	epos_dig_inc_enc_cfg_t enc_cfg1;
	//!< SSI absolute encoder configuration
	epos_ssi_abs_enc_cfg_t ssi_abs_enc_cfg;
	//!< Hall Sensor configuration
	epos_hall_sensor_cfg_t hall_cfg;
	//!< Motor configuration
	epos_motor_cfg_t motor_cfg;
	//!< Commutation sensor - absolute
	epos_commutation_sensor_abs_e commutation_sensor_abs;
	//!< Commutation sensor - relative
	epos_commutation_sensor_rel_e commutation_sensor_rel;

} epos_cfg_t;

/*! @brief EPOS4 device data. */
typedef struct epos_track_t {
	uint8_t node_id; //!< CANopen Node ID
	int32_t pos;	 //!< position [position units]
	int32_t vel;	 //!< velocity [velocity units]
	int32_t curr;	 //!< current [mA]
	uint16_t status; //!< status

	epos_state_e state; //!< state

} epos_track_t;

/*! @brief EPOS struct. */
typedef struct epos_t {
	canopen_handle_t *handle; //!< CANopen handle

	epos_track_t track[EPOS_MAX_TRACK_CNT]; //!< EPOS4 track data
	uint8_t track_count;					//!< Number of tracked EPOS4 devices
} epos_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Write data into EPOS4 object dictionary.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] obj		EPOS object dictionary entry.
 * @param[in] val		Write value.
 * @return status.
 */
uint32_t epos_obj_write(epos_t *epos, uint8_t epos_id,
						const canopen_od_entry_t *obj, void *val);

/**
 * @brief Read data from EPOS4 object dictionary.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] obj		EPOS object dictionary entry.
 * @param[out] val		Receive value.
 * @return status.
 */
uint32_t epos_obj_read(epos_t *epos, uint8_t epos_id,
					   const canopen_od_entry_t *obj, void *val);

/**
 * @brief Transmit EPOS4 NMT message
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] cs		CANopen NMT Message Command Specifier.
 * @return status.
 */
uint32_t epos_nmt(epos_t *epos, uint8_t epos_id, canopen_nmt_cs_e cs);

/**
 * @brief Set EPOS4 actuator mode.
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @param[in] op_mode	Mode of operation.
 * @return status.
 */
uint32_t epos_mode(epos_t *epos, uint8_t epos_id, epos_op_mode_e op_mode);

/**
 * @brief Enter Switch On Disabled state.
 * @note Power Disabled (Low-level power disabled).
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_enter_switch_on_disabled(epos_t *epos, uint8_t epos_id);

/**
 * @brief Enter Ready to Switch On state.
 * @note Power Disabled (Low-level power disabled).
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_enter_ready_to_switch_on(epos_t *epos, uint8_t epos_id);

/**
 * @brief Enter Switched On state.
 * @note Power Enabled (High-level power disabled, no torque on motor).
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_enter_switched_on(epos_t *epos, uint8_t epos_id);

/**
 * @brief Enter Operation Enabled state.
 * @note Torque Enabled (torque on motor).
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_enter_operation_enabled(epos_t *epos, uint8_t epos_id);

/**
 * @brief Enter Quick Stop Active state.
 * @note Torque Enabled (torque on motor).
 *
 * @param[in] epos		EPOS handler.
 * @param[in] epos_id	EPOS4 track ID (assigned by EPOS layer).
 * @return status.
 */
uint32_t epos_enter_quick_stop_active(epos_t *epos, uint8_t epos_id);

#ifdef __cplusplus
}
#endif

#endif /* EPOS_COMMON_H */