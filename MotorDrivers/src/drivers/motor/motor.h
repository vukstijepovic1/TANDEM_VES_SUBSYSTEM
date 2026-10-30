/**
 * @file    motor.h
 * @brief   EDePro turbomotor driver.
 * @version	1.0.0
 * @date    18.05.2025
 * @author  LisumLab
 */

#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#ifdef MOTOR_CONFIG
#include "motor_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* Motor Status Message length */
#define MOTOR_STATUS_LEN 133

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief Motor parser states */
typedef enum motor_parser_state_e {
	MOTOR_PARSER_STATE_HEADER1 = 0,
	MOTOR_PARSER_STATE_HEADER2 = 1,
	MOTOR_PARSER_STATE_DATA = 2,
	MOTOR_PARSER_STATE_CRC1 = 3,
	MOTOR_PARSER_STATE_CRC2 = 4
} motor_parser_state_e;

/*! @brief Motor status */
typedef struct __attribute__((__packed__)) motor_status_data_t {
	float time;					// Engine time since turning on [s]
	float w_gg;					// Angular Rate of Gas Generator [Hz]
	float w_gg_N;				// Nominal Rate Velocity of Gas generator [Hz]
	float w_ft;					// Angular Rate of Free Turbine [Hz]
	float w_ft_N;				// Nominal Rate Velocity of Free Turbine [Hz]
	float fuel_flow;			// Fuel Flow [g/s]
	float oil_flow_arm;			// Oil Flow - arm [g/s]
	float oil_flow_reducer;		// Oil Flow - reducer [g/s]
	float battery_volt;			// Battery voltage [V]
	float temp_front_bearing;	// Temperature of front bearing [degC]
	float temp_rear_bearing;	// Temperature of rear bearing [degC]
	float temp_elastic_bearing; // Temperature of elastic bearing [degC]
	float temp_rigid_bearing;	// Temperature of rigid bearing [degC]
	float temp_input_oil;		// Temperature of input oil [degC]
	float temp_arm_oil;			// Temperature of oil - arm [degC]
	float temp_reducer_oil;		// Temperature of oil - reducer [degC]
	float temp_exhaust_fume;	// Temperature of exhaust fumes [degC]
	float press_arm_oil;		// Oil pressure - arm [bar]
	float fuel_level;			// Fuel level [l]
	float oil_level_bearing;	// Oil level - bearing [%]
	float oil_level_reducer;	// Oil level - reducer [%]
	float PWM_fuel_pump;		// Fuel pump PWM [%]
	float current_fuel_pump;	// Fuel pump current [A]
	float PWM_oil_pump;			// Oil pump PWM [%]
	float current_oil_pump;		// Oil pump current [A]
	float PWM_oil_suction_pump_reducer; // Suction pump (reducer) PWM [%]
	float PWM_oil_suction_pump_arm;		// Suction pump (arm) PWM [%]
	float PWM_oil_pressure_pump;		// Oil pressure pump PWM [%]
	uint8_t activate;					// Activate Struct
	uint16_t turn_on;					// Turn ON Struct
	uint8_t valve_state;				// Valve State
	uint8_t pressure_state;				// Pressure State
	uint32_t warning;					// Warning Struct
	uint32_t error;						// Error Struct
	uint32_t EvnC;						// EvnC Struct
} motor_status_data_t;

/*! @brief Motor status message */
typedef struct __attribute__((__packed__)) motor_status_t {
	uint8_t header[2];
	motor_status_data_t data;
	uint8_t crc[2];
} motor_status_t;

/*! @brief Motor structure */
typedef struct motor_t {
	uint8_t arr[MOTOR_STATUS_LEN]; //!< Parser buffer
	motor_parser_state_e state;	   //!< Parser state
	uint16_t idx;				   //!< Parser index
	motor_status_t status;		   //!< Received motor status

#ifdef MOTOR_DBG
	uint32_t pass_cnt; //!< Number of received messages with valid CRC16
	uint32_t drop_cnt; //!< Number of received messages with invalid CRC16
#endif
} motor_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Init motor parser.
 *
 * @param[in] motor motor instance.
 * @return None
 */
void motor_init(motor_t *motor);

/**
 * @brief Parse motor status.
 *
 * @param[in] motor motor instance.
 * @param[in] data byte to be processed.
 * @return 0 - parse success, 1 - parse ongoing, -1 parse error
 */
uint32_t motor_parse(motor_t *motor, uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H */