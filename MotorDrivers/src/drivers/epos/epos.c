/**
 * @file	EPOS.c
 * @brief	EPOS Library.
 * @version	1.0.0
 * @date	12.02.2025
 * @author	LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>

#include "epos.h"
#include "epos_eeprom.h"
#include "epos_od.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/* pdo1tx structure */
typedef struct __attribute__((__packed__)) epos_pdo1tx_t {
	uint16_t status;
	int32_t pos;
} epos_pdo1tx_t;

/* pdo2tx structure */
typedef struct __attribute__((__packed__)) epos_pdo2tx_t {
	int32_t vel;
	int32_t curr;
} epos_pdo2tx_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* CANopen on receive callback */
void epos_on_recv(void *user_arg, uint16_t fcn_code, uint8_t node_id,
				  uint8_t *data, uint8_t data_size);

/* get actuator instance */
static epos_track_t *epos_track_get(epos_t *epos, uint8_t node_id);

/* configure EPOS units */
static uint32_t epos_unit_cfg(epos_t *epos, uint8_t epos_id,
							  epos_unit_cfg_t *cfg);

/* read EPOS units*/
static uint32_t epos_unit_cfg_get(epos_t *epos, uint8_t epos_id,
								  epos_unit_cfg_t *cfg);

/* configure EPOS digital incremental encoder 1 */
static uint32_t epos_dig_inc_enc1_cfg(epos_t *epos, uint8_t epos_id,
									  epos_dig_inc_enc_cfg_t *cfg);

/* read EPOS digital incremental encoder 1 configuration */
static uint32_t epos_dig_inc_enc1_cfg_get(epos_t *epos, uint8_t epos_id,
										  epos_dig_inc_enc_cfg_t *cfg);

/* configure EPOS SSI absolute encoder */
static uint32_t epos_ssi_abs_enc_cfg(epos_t *epos, uint8_t epos_id,
									 epos_ssi_abs_enc_cfg_t *cfg);

/* read EPOS SSI absolute encoder configuration */
static uint32_t epos_ssi_abs_enc_cfg_get(epos_t *epos, uint8_t epos_id,
										 epos_ssi_abs_enc_cfg_t *cfg);

/* configure EPOS hall sensor */
static uint32_t epos_hall_cfg(epos_t *epos, uint8_t epos_id,
							  epos_hall_sensor_cfg_t *cfg);

/* read EPOS hall sensor configuration*/
static uint32_t epos_hall_cfg_get(epos_t *epos, uint8_t epos_id,
								  epos_hall_sensor_cfg_t *cfg);

/* configure EPOS motor */
static uint32_t epos_motor_cfg(epos_t *epos, uint8_t epos_id,
							   epos_motor_cfg_t *cfg);

/* read EPOS motor configuration */
static uint32_t epos_motor_cfg_get(epos_t *epos, uint8_t epos_id,
								   epos_motor_cfg_t *cfg);

/* configure EPOS PDO Tx */
static uint32_t epos_pdotx_cfg(epos_t *epos, uint8_t epos_id);

/*******************************************************************************
 * Code
 ******************************************************************************/

void epos_init(epos_t *epos, canopen_handle_t *handle)
{
	/* configure canopen */
	epos->handle = handle;
	canopen_callback_config(epos->handle, epos_on_recv, epos);

	/* reset actuator structures */
	epos->track_count = 0;
	memset(epos->track, 0x00, sizeof(epos->track));
}

uint8_t epos_track(epos_t *epos, uint8_t node_id)
{
	if (epos_track_get(epos, node_id) != NULL ||
		epos->track_count >= EPOS_MAX_TRACK_CNT)
		return -1;

	epos->track[epos->track_count].node_id = node_id;

	return epos->track_count++;
}

uint32_t epos_cfg(epos_t *epos, uint8_t epos_id, epos_cfg_t *cfg)
{
	uint32_t reg_val;

	/* enter pre-operational state */
	if (epos_nmt(epos, epos_id, CANOPEN_NMT_CS_ENTER_PRE_OPERATIONAL))
		return 1;

	/* configure units */
	if (epos_unit_cfg(epos, epos_id, &cfg->unit_cfg))
		return 1;

	/* configure sensors */
	reg_val = (cfg->sensor_cfg.sensor3_type & 0xFF) << 16 |
			  (cfg->sensor_cfg.sensor2_type & 0xFF) << 8 |
			  (cfg->sensor_cfg.sensor1_type & 0xFF);

	if (epos_obj_write(epos, epos_id, &ObjSensorsConfiguration, &reg_val))
		return 1;

	/* configure main/aux sensor and set pos, vel and curr control structure */
	reg_val = ((cfg->sensor_cfg.aux_sensor & 0x0F) << 20) |
			  ((cfg->sensor_cfg.main_sensor & 0x0F) << 16) | (1 << 8) |
			  (1 << 4) | 1;

	if (epos_obj_write(epos, epos_id, &ObjControlStructure, &reg_val))
		return 1;

	/* configure sensor 1 */
	if (cfg->sensor_cfg.sensor1_type == EPOS_SENSOR1_TYPE_DIGITAL_INC_ENC1) {
		/* configure digital incremental encoder 1 */
		if (epos_dig_inc_enc1_cfg(epos, epos_id, &cfg->enc_cfg1))
			return 1;
	}

	/* configure sensor 2 */
	switch (cfg->sensor_cfg.sensor2_type) {
	case EPOS_SENSOR2_TYPE_DIGITAL_INC_ENC2:
		/* unsupported */
		break;
	case EPOS_SENSOR2_TYPE_ANALOG_INC_ENC:
		/* unsupported */
		break;
	case EPOS_SENSOR2_TYPE_SSI_ABS_ENC:
		/* configure SSI absolute encoder */
		if (epos_ssi_abs_enc_cfg(epos, epos_id, &cfg->ssi_abs_enc_cfg))
			return 1;

		break;
	default:
		break;
	}

	/* configure sensor 3 */
	if (cfg->sensor_cfg.sensor3_type == EPOS_SENSOR3_TYPE_DIGITAL_HALL_SENSOR) {
		/* configure hall sensor */
		if (epos_hall_cfg(epos, epos_id, &cfg->hall_cfg))
			return 1;
	}

	/* configure motor */
	if (epos_motor_cfg(epos, epos_id, &cfg->motor_cfg))
		return 1;

	/* configure commutation sensors */
	reg_val = (cfg->commutation_sensor_abs & 0x0F) << 4 |
			  (cfg->commutation_sensor_rel & 0x0F);
	if (epos_obj_write(epos, epos_id, &ObjCommutationSensors, &reg_val))
		return 1;

	/* configure heartbeat period */
	reg_val = 0; // [ms]
	if (epos_obj_write(epos, epos_id, &ObjProducerHeartbeatTime, &reg_val))
		return 1;

	/* configure device behaviour in case of CAN heartbeat error */
	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjCommunicationError, &reg_val))
		return 1;

	if (epos_pdotx_cfg(epos, epos_id))
		return 1;

	/* save all parameters*/
	if (epos_eeprom_save(epos, epos_id))
		return 1;

	return 0;
}

uint32_t epos_cfg_get(epos_t *epos, uint8_t epos_id, epos_cfg_t *cfg)
{
	uint32_t reg_val;

	/* get units configuration */
	if (epos_unit_cfg_get(epos, epos_id, &cfg->unit_cfg))
		return 1;

	/* get sensor configuration */
	if (epos_obj_read(epos, epos_id, &ObjSensorsConfiguration, &reg_val))
		return 1;

	cfg->sensor_cfg.sensor1_type = reg_val & 0xFF;
	cfg->sensor_cfg.sensor2_type = (reg_val >> 8) & 0xFF;
	cfg->sensor_cfg.sensor3_type = (reg_val >> 16) & 0xFF;

	/* get main/aux sensor and set pos, vel and curr control structure */
	if (epos_obj_read(epos, epos_id, &ObjControlStructure, &reg_val))
		return 1;

	cfg->sensor_cfg.aux_sensor = (reg_val >> 20) & 0x0F;
	cfg->sensor_cfg.main_sensor = (reg_val >> 16) & 0x0F;

	/* get sensor 1 configuration */
	if (cfg->sensor_cfg.sensor1_type == EPOS_SENSOR1_TYPE_DIGITAL_INC_ENC1) {
		/* get digital incremental encoder 1 configuration */
		if (epos_dig_inc_enc1_cfg_get(epos, epos_id, &cfg->enc_cfg1))
			return 1;
	}

	/* get sensor 2 configuration */
	switch (cfg->sensor_cfg.sensor2_type) {
	case EPOS_SENSOR2_TYPE_DIGITAL_INC_ENC2:
		/* unsupported */
		break;
	case EPOS_SENSOR2_TYPE_ANALOG_INC_ENC:
		/* unsupported */
		break;
	case EPOS_SENSOR2_TYPE_SSI_ABS_ENC:
		/* get SSI absolute encoder configuration */
		if (epos_ssi_abs_enc_cfg_get(epos, epos_id, &cfg->ssi_abs_enc_cfg))
			return 1;

		break;
	default:
		break;
	}

	/* get sensor 3 configuration */
	if (cfg->sensor_cfg.sensor3_type == EPOS_SENSOR3_TYPE_DIGITAL_HALL_SENSOR) {
		/* get hall sensor configuration */
		if (epos_hall_cfg_get(epos, epos_id, &cfg->hall_cfg))
			return 1;
	}

	/* get motor configuration */
	if (epos_motor_cfg_get(epos, epos_id, &cfg->motor_cfg))
		return 1;

	/* get commutation sensor configuration */
	if (epos_obj_read(epos, epos_id, &ObjCommutationSensors, &reg_val))
		return 1;

	cfg->commutation_sensor_rel = reg_val & 0x0F;
	cfg->commutation_sensor_abs = (reg_val >> 4) & 0x0F;

	return 0;
}

uint32_t epos_fault_reset(epos_t *epos, uint8_t epos_id)
{
	uint16_t cw = 0;
	uint16_t sw = 0;
	epos_state_e state;

	/* disable power */
	cw = 0b1110;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	/* send fault reset controlword */
	cw = EPOS_DEF_CONTROLWORD_FAULT_RESET;
	if (epos_obj_write(epos, epos_id, &ObjControlword, &cw))
		return 1;

	/* check if EPOS is in switch on disabled state */
	if (epos_obj_read(epos, epos_id, &ObjStatusword, &sw))
		return 1;

	state = sw & EPOS_DEF_STATE_MASK;

	if (state != EPOS_STATE_SWITCH_ON_DISABLED)
		return 1;

	/* enter ready to switch on state */
	if (epos_enter_ready_to_switch_on(epos, epos_id))
		return 1;

	return 0;
}

void epos_heartbeat(epos_t *epos)
{
	canopen_heartbeat(epos->handle, CANOPEN_NMT_SLAVE_STATE_OPERATIONAL);
}

void epos_sync(epos_t *epos) { canopen_sync(epos->handle); }

uint32_t epos_status_get(epos_t *epos, uint8_t epos_id, uint16_t *status)
{
	return epos_obj_read(epos, epos_id, &ObjStatusword, status);
}

uint32_t epos_position_get(epos_t *epos, uint8_t epos_id, int32_t *pos)
{
	return epos_obj_read(epos, epos_id, &ObjPositionActualValue, pos);
}

uint32_t epos_velocity_get(epos_t *epos, uint8_t epos_id, int32_t *vel)
{
	return epos_obj_read(epos, epos_id, &ObjVelocityActualValue, vel);
}

uint32_t epos_sensor_position_get(epos_t *epos, uint8_t epos_id,
								  uint32_t sensor_id, int32_t *pos)
{
	uint32_t ret = 1;

	if (sensor_id > 3 || sensor_id < 1)
		return 1;

	switch (sensor_id) {
	case 1:
		ret = epos_obj_read(epos, epos_id, &ObjPositionActualValueSensor1, pos);
		break;
	case 2:
		ret = epos_obj_read(epos, epos_id, &ObjPositionActualValueSensor2, pos);
		break;
	case 3:
		ret = epos_obj_read(epos, epos_id, &ObjPositionActualValueSensor3, pos);
		break;
	default:
		break;
	}

	return ret;
}

uint32_t epos_ssi_position_get(epos_t *epos, uint8_t epos_id, int64_t *pos)
{
	return epos_obj_read(epos, epos_id, &ObjSSIPositionRawValueComplete, pos);
}

/******************************* User Callbacks *******************************/

/* Process received CANOpen packet */
void epos_on_recv(void *user_arg, uint16_t fcn_code, uint8_t node_id,
				  uint8_t *data, uint8_t data_size)
{
	epos_t *epos;
	epos_track_t *track = NULL;
	epos_pdo1tx_t pdo1tx;
	epos_pdo2tx_t pdo2tx;

	epos = (epos_t *)user_arg;
	track = epos_track_get(epos, node_id);

	/* check if actuator is tracked */
	if (track == NULL)
		return;

	/* process message according to function code */
	switch (fcn_code) {
	case CANOPEN_COB_TYPE_PDO1_TX:
		memcpy(&pdo1tx, data, sizeof(pdo1tx));

		track->status = pdo1tx.status;
		track->pos = pdo1tx.pos;
		track->state = pdo1tx.status & EPOS_DEF_STATE_MASK;

		break;
	case CANOPEN_COB_TYPE_PDO2_TX:
		memcpy(&pdo2tx, data, sizeof(pdo2tx));

		track->vel = pdo2tx.vel;
		track->curr = pdo2tx.curr;

		break;
	default:
		break;
	}
}

/****************************** static functions ******************************/

static epos_track_t *epos_track_get(epos_t *epos, uint8_t node_id)
{
	/* check if node id is broadcast */
	if (node_id == CANOPEN_NODE_ID_BROADCAST)
		return NULL;

	for (uint8_t i = 0; i < epos->track_count; i++) {
		epos_track_t *track = NULL;

		track = (epos_track_t *)&epos->track[i];

		if (track->node_id == node_id)
			return track;
	}

	return NULL;
}

static uint32_t epos_unit_cfg(epos_t *epos, uint8_t epos_id,
							  epos_unit_cfg_t *cfg)
{
	uint32_t reg_val;

	/* configure position units */
	if (cfg->pos_prefix != EPOS_UNIT_PREFIX_NONE)
		return 1;

	reg_val = (cfg->pos_prefix & 0xFF) << 24 | (EPOS_DEF_UNIT_INCREMENTS << 16);

	if (epos_obj_write(epos, epos_id, &ObjSIUnitPosition, &reg_val))
		return 1;

	/* configure velocity units */
	reg_val = (cfg->vel_prefix & 0xFF) << 24 |
			  (EPOS_DEF_UNIT_REVOLUTIONS << 16) | (EPOS_DEF_UNIT_MINUTE << 8);

	if (epos_obj_write(epos, epos_id, &ObjSIUnitVelocity, &reg_val))
		return 1;

	/* configure acceleration units */
	if (cfg->accel_prefix != EPOS_UNIT_PREFIX_NONE)
		return 1;

	reg_val = (cfg->accel_prefix & 0xFF) << 24 | (EPOS_DEF_UNIT_RPM << 16) |
			  (EPOS_DEF_UNIT_SECOND << 8);

	if (epos_obj_write(epos, epos_id, &ObjSIUnitAcceleration, &reg_val))
		return 1;

	return 0;
}

static uint32_t epos_unit_cfg_get(epos_t *epos, uint8_t epos_id,
								  epos_unit_cfg_t *cfg)
{
	uint32_t reg_val;

	/* read position units */
	if (epos_obj_read(epos, epos_id, &ObjSIUnitPosition, &reg_val))
		return 1;

	cfg->pos_prefix = (reg_val >> 24) & 0xFF;

	/* read velocity units */
	if (epos_obj_read(epos, epos_id, &ObjSIUnitVelocity, &reg_val))
		return 1;

	cfg->vel_prefix = (reg_val >> 24) & 0xFF;

	/* read acceleration units */
	if (epos_obj_read(epos, epos_id, &ObjSIUnitAcceleration, &reg_val))
		return 1;

	cfg->accel_prefix = (reg_val >> 24) & 0xFF;

	return 0;
}

static uint32_t epos_dig_inc_enc1_cfg(epos_t *epos, uint8_t epos_id,
									  epos_dig_inc_enc_cfg_t *cfg)
{
	uint32_t reg_val;

	/* configure number of pulses */
	if (epos_obj_write(epos, epos_id,
					   &ObjDigitalIncrementalEncoder1NumberOfPulses,
					   &cfg->pulses))
		return 1;

	/* configure encoder type */
	reg_val = ((cfg->method & 0x01) << 9) | ((cfg->direction & 0x01) << 4) |
			  (cfg->type & 0x01);

	if (epos_obj_write(epos, epos_id, &ObjDigitalIncrementalEncoder1Type,
					   &reg_val))
		return 1;

	return 0;
}

static uint32_t epos_dig_inc_enc1_cfg_get(epos_t *epos, uint8_t epos_id,
										  epos_dig_inc_enc_cfg_t *cfg)
{
	uint32_t reg_val;

	/* read number of pulses */
	if (epos_obj_read(epos, epos_id,
					  &ObjDigitalIncrementalEncoder1NumberOfPulses,
					  &cfg->pulses))
		return 1;

	/* read encoder type */
	if (epos_obj_read(epos, epos_id, &ObjDigitalIncrementalEncoder1Type,
					  &reg_val))
		return 1;

	cfg->type = reg_val & 0x01;
	cfg->direction = (reg_val >> 4) & 0x01;
	cfg->method = (reg_val >> 9) & 0x01;

	return 0;
}

static uint32_t epos_ssi_abs_enc_cfg(epos_t *epos, uint8_t epos_id,
									 epos_ssi_abs_enc_cfg_t *cfg)
{
	uint32_t reg_val;

	/* configure data rate [kbit/s] */
	if (epos_obj_write(epos, epos_id, &ObjSSIDataRate, &cfg->data_rate))
		return 1;

	/* configure packet structure (number of data bits) */
	reg_val = (cfg->special_bits_leading << 24) | (cfg->multi_turn_bits << 16) |
			  (cfg->single_turn_bits << 8) | cfg->special_bits_trailing;

	if (epos_obj_write(epos, epos_id, &ObjSSINumberOfDataBits, &reg_val))
		return 1;

	/* configure encoding type */
	reg_val = ((cfg->direction & 0x01) << 4) | (cfg->encoding_type & 0x0F);

	if (epos_obj_write(epos, epos_id, &ObjSSIEncodingType, &reg_val))
		return 1;

	/* configure timeout [us] */
	if (epos_obj_write(epos, epos_id, &ObjSSITimeoutTime, &cfg->timeout))
		return 1;

	/* configure power-up time [us] */
	if (epos_obj_write(epos, epos_id, &ObjSSIPowerUpTime, &cfg->power_up_time))
		return 1;

	/* configure position bits */
	if (cfg->multi_turn_bits_used + cfg->single_turn_bits_used > 32)
		return 1;

	reg_val = (cfg->multi_turn_bits_used << 8) | (cfg->single_turn_bits_used);

	if (epos_obj_write(epos, epos_id, &ObjSSIPositionBits, &reg_val))
		return 1;

	return 0;
}

static uint32_t epos_ssi_abs_enc_cfg_get(epos_t *epos, uint8_t epos_id,
										 epos_ssi_abs_enc_cfg_t *cfg)
{
	uint32_t reg_val;

	/* read data rate [kbit/s] */
	if (epos_obj_read(epos, epos_id, &ObjSSIDataRate, &cfg->data_rate))
		return 1;

	/* read packet structure (number of data bits) */
	if (epos_obj_read(epos, epos_id, &ObjSSINumberOfDataBits, &reg_val))
		return 1;

	cfg->special_bits_leading = (reg_val >> 24) & 0xFF;
	cfg->multi_turn_bits = (reg_val >> 16) & 0xFF;
	cfg->single_turn_bits = (reg_val >> 8) & 0xFF;
	cfg->special_bits_trailing = reg_val & 0xFF;

	/* read encoding type */
	if (epos_obj_read(epos, epos_id, &ObjSSIEncodingType, &reg_val))
		return 1;

	cfg->direction = (reg_val >> 4) & 0x01;
	cfg->encoding_type = reg_val & 0x0F;

	/* read timeout [us] */
	if (epos_obj_read(epos, epos_id, &ObjSSITimeoutTime, &cfg->timeout))
		return 1;

	/* read power-up time [us] */
	if (epos_obj_read(epos, epos_id, &ObjSSIPowerUpTime, &cfg->power_up_time))
		return 1;

	/* read position bits */
	if (epos_obj_read(epos, epos_id, &ObjSSIPositionBits, &reg_val))
		return 1;

	cfg->multi_turn_bits_used = (reg_val >> 8) & 0xFF;
	cfg->single_turn_bits_used = reg_val & 0xFF;

	return 0;
}

static uint32_t epos_hall_cfg(epos_t *epos, uint8_t epos_id,
							  epos_hall_sensor_cfg_t *cfg)
{
	uint32_t type;

	/* configure sensor type */
	type = ((cfg->method & 0x01) << 4) | (cfg->polarity & 0x01);

	if (epos_obj_write(epos, epos_id, &ObjDigitalHallSensorType, &type))
		return 1;

	return 0;
}

static uint32_t epos_hall_cfg_get(epos_t *epos, uint8_t epos_id,
								  epos_hall_sensor_cfg_t *cfg)
{
	uint32_t reg_val;

	/* get sensor type */
	if (epos_obj_read(epos, epos_id, &ObjDigitalHallSensorType, &reg_val))
		return 1;

	cfg->polarity = reg_val & 0x01;
	cfg->method = (reg_val >> 4) & 0x01;

	return 0;
}

static uint32_t epos_motor_cfg(epos_t *epos, uint8_t epos_id,
							   epos_motor_cfg_t *cfg)
{
	/* configure nominal current [mA] */
	if (epos_obj_write(epos, epos_id, &ObjNominalCurrent, &cfg->nom_curr))
		return 1;

	/* configure current limit [mA] */
	if (epos_obj_write(epos, epos_id, &ObjOutputCurrentLimit, &cfg->curr_limit))
		return 1;

	/* configure number of pole pairs */
	if (epos_obj_write(epos, epos_id, &ObjNumberOfPolePairs, &cfg->pp_num))
		return 1;

	/* configure thermal time constant [.1s] */
	if (epos_obj_write(epos, epos_id, &ObjThermalTimeConstantWinding,
					   &cfg->tt_constant))
		return 1;

	/* configure torque constant [uNm/A] */
	if (epos_obj_write(epos, epos_id, &ObjTorqueConstant,
					   &cfg->torque_constant))
		return 1;

	/* configure motor type */
	if (epos_obj_write(epos, epos_id, &ObjMotorType, &cfg->type))
		return 1;

	/* configure max motor speed [rpm] */
	uint32_t max_speed = 0;

	switch (cfg->type) {
	case EPOS_MOTOR_TYPE_PM_DC:
		max_speed = 100000;
		break;
	case EPOS_MOTOR_TYPE_SIN_PM_BL:
		max_speed = 50000 / cfg->pp_num;
		break;
	case EPOS_MOTOR_TYPE_TRAP_PM_BL:
		max_speed = 100000 / cfg->pp_num;
		break;
	default:
		return 1;
	}

	if (cfg->max_speed > max_speed)
		return 1;

	if (epos_obj_write(epos, epos_id, &ObjMaxMotorSpeed, &cfg->max_speed))
		return 1;

	return 0;
}

static uint32_t epos_motor_cfg_get(epos_t *epos, uint8_t epos_id,
								   epos_motor_cfg_t *cfg)
{
	/* read nominal current [mA] */
	if (epos_obj_read(epos, epos_id, &ObjNominalCurrent, &cfg->nom_curr))
		return 1;

	/* read current limit [mA] */
	if (epos_obj_read(epos, epos_id, &ObjOutputCurrentLimit, &cfg->curr_limit))
		return 1;

	/* read number of pole pairs */
	if (epos_obj_read(epos, epos_id, &ObjNumberOfPolePairs, &cfg->pp_num))
		return 1;

	/* read thermal time constant [.1s] */
	if (epos_obj_read(epos, epos_id, &ObjThermalTimeConstantWinding,
					  &cfg->tt_constant))
		return 1;

	/* read torque constant [uNm/A] */
	if (epos_obj_read(epos, epos_id, &ObjTorqueConstant, &cfg->torque_constant))
		return 1;

	/* read motor type */
	if (epos_obj_read(epos, epos_id, &ObjMotorType, &cfg->type))
		return 1;

	/* read max motor speed [rpm] */
	if (epos_obj_read(epos, epos_id, &ObjMaxMotorSpeed, &cfg->max_speed))
		return 1;

	return 0;
}

static uint32_t epos_pdotx_cfg(epos_t *epos, uint8_t epos_id)
{
	uint32_t reg_val;

	/* configure Transmit PDO1 */
	reg_val = EPOS_DEF_PDO_VALID | EPOS_DEF_PDO_RTR_NOT_ALLOWED |
			  CANOPEN_COB_TYPE_PDO1_TX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByTxPDO1, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_SYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeTxPDO1, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInTxPDO1,
					   &reg_val))
		return 1;

	reg_val = (ObjStatusword.idx << 16) | (ObjStatusword.subidx << 8) |
			  (ObjStatusword.data_size * 8); // Data length = 16bit

	if (epos_obj_write(epos, epos_id, &ObjFirstMappedObjectInTxPDO1, &reg_val))
		return 1;

	reg_val = (ObjPositionActualValue.idx << 16) |
			  (ObjPositionActualValue.subidx << 8) |
			  (ObjPositionActualValue.data_size * 8); // Data length = 32bit

	if (epos_obj_write(epos, epos_id, &ObjSecondMappedObjectInTxPDO1, &reg_val))
		return 1;

	reg_val = 0x02;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInTxPDO1,
					   &reg_val))
		return 1;

	/* configure Transmit PDO2 */
	reg_val = EPOS_DEF_PDO_VALID | EPOS_DEF_PDO_RTR_NOT_ALLOWED |
			  CANOPEN_COB_TYPE_PDO2_TX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByTxPDO2, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_SYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeTxPDO2, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInTxPDO2,
					   &reg_val))
		return 1;

	reg_val = (ObjVelocityActualValue.idx << 16) |
			  (ObjVelocityActualValue.subidx << 8) |
			  (ObjVelocityActualValue.data_size * 8); // Data length = 32bit

	if (epos_obj_write(epos, epos_id, &ObjFirstMappedObjectInTxPDO2, &reg_val))
		return 1;

	reg_val = (ObjCurrentActualValue.idx << 16) |
			  (ObjCurrentActualValue.subidx << 8) |
			  (ObjCurrentActualValue.data_size * 8); // Data length = 32bit

	if (epos_obj_write(epos, epos_id, &ObjSecondMappedObjectInTxPDO2, &reg_val))
		return 1;

	reg_val = 0x02;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInTxPDO2,
					   &reg_val))
		return 1;

	/* configure Transmit PDO3 */
	reg_val = EPOS_DEF_PDO_INVALID | EPOS_DEF_PDO_RTR_NOT_ALLOWED |
			  CANOPEN_COB_TYPE_PDO3_TX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByTxPDO3, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_ASYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeTxPDO3, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInTxPDO3,
					   &reg_val))
		return 1;

	/* configure Transmit PDO4 */
	reg_val = EPOS_DEF_PDO_INVALID | EPOS_DEF_PDO_RTR_NOT_ALLOWED |
			  CANOPEN_COB_TYPE_PDO4_TX | epos->track[epos_id].node_id;

	if (epos_obj_write(epos, epos_id, &ObjCOBIDUsedByTxPDO4, &reg_val))
		return 1;

	reg_val = EPOS_DEF_PDO_ASYNCHRONOUS_TRANSMISSION;
	if (epos_obj_write(epos, epos_id, &ObjTransmissionTypeTxPDO4, &reg_val))
		return 1;

	reg_val = 0x00;
	if (epos_obj_write(epos, epos_id, &ObjNumberOfMappedObjectsInTxPDO4,
					   &reg_val))
		return 1;

	return 0;
}

/********************************* End Of File ********************************/