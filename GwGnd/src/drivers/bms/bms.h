/**
 * @file    bms.h
 * @brief   Daly BMS driver.
 * @version	1.0.0
 * @date    18.05.2025
 * @author  LisumLab
 */

#ifndef BMS_H
#define BMS_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#ifdef BMS_CONFIG
#include "bms_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* Daly BMS register count */
#define BMS_STATUS_REG_COUNT 62
/* Daly BMS status request length in bytes */
#define BMS_STATUS_REQUEST_LEN 8
/* Daly BMS status reply length in bytes*/
#define BMS_STATUS_RESPONSE_LEN (3 + 2 * BMS_STATUS_REG_COUNT + 2)

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief BMS Status message */
typedef struct bms_status_t {
	uint16_t cell_volt[14];		 // cell voltage [mV]
	int16_t temp;				 // temperature [degC]
	uint16_t sum_volt;			 // sum voltage [10 * V]
	int16_t curr;				 // Current [10 * A]
	uint16_t soc;				 // State of charge [10 * %]
	uint16_t max_volt;			 // Maximum cell voltage [mV]
	uint16_t min_volt;			 // Minimum cell voltage [mV]
	int16_t max_temp;			 // Maximum monomer temperature [degC]
	int16_t min_temp;			 // Minimum monomer temperature [degC]
	uint16_t charge_state;		 // 0 - idle, 1 - charging , 2 - discharging
	uint16_t rem_capacity;		 // Remaining capacity [10 * Ah]
	uint16_t cell_num;			 // Number of cells
	uint16_t temp_sensor_num;	 // Number of temperatures
	uint16_t cycles;			 // Number of charge/discharge cycles
	uint16_t eq_stat;			 // Equilibrium status
	uint16_t charge_mos_stat;	 // Charge MOSFET status
	uint16_t discharge_mos_stat; // Discharge MOSFET status
	uint16_t avg_volt;			 // Average voltage [mV]
	uint16_t diff_volt;			 // Differential voltage [mV]
	uint16_t power;				 // Power [W]
	uint16_t fault1;			 // Fault status 1
	uint16_t fault2;			 // Fault status 2
	uint16_t fault3;			 // Fault status 3
	uint16_t fault4;			 // Fault status 4
} bms_status_t;

/*! @brief BMS parser state */
typedef enum bms_parser_state_e {
	BMS_PARSER_STATE_ADDR = 0,
	BMS_PARSER_STATE_CMD,
	BMS_PARSER_STATE_LEN,
	BMS_PARSER_STATE_DATA,
	BMS_PARSER_STATE_CRC1,
	BMS_PARSER_STATE_CRC2
} bms_parser_state_e;

/*! @brief BMS structure */
typedef struct bms_t {
	uint8_t arr[BMS_STATUS_RESPONSE_LEN]; //!< BMS parser buffer
	bms_parser_state_e state;			  //!< parser state
	uint16_t idx;						  //!< parser index
	uint16_t len;						  //!< parser packet length
	uint16_t data_idx;					  //!< parser data index
	bms_status_t status;				  //!< BMS status

#ifdef BMS_DBG
	uint32_t drop_cnt; //!< Number of received messages with valid CRC16
	uint32_t pass_cnt; //!< Number of received messages with invalid CRC16
#endif
} bms_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize Daly BMS parser.
 *
 * @param[in] bms BMS instance.
 * @return None
 */
void bms_init(bms_t *bms);

/**
 * @brief Parse BMS status reply of data.
 *
 * @param[in] bms	BMS instance.
 * @param[in] byte	byte to be parsed.
 * @return 0 - parse success, 1 - parse ongoing, -1 parse error
 */
uint32_t bms_parse(bms_t *bms, uint8_t data);

/**
 * @brief Pack BMS status request.
 *
 * @param[in] bms 	BMS instance.
 * @param[in] data 	buffer which will be used to contain status request.
 *					Length of array must be at least BMS_STATUS_REQUEST_LEN
 * @return None
 */
void bms_status_req_pack(bms_t *bms, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif /* BMS_H */
