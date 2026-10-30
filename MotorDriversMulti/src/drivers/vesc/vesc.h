/**
 * @file    ftp_handler.h
 * @brief   Drivers for VESC.
 * @version	0.0.1
 * @date    12.08.2025
 * @author  BetaTehPro
 * 
 * @note U NEAD TO DEFINE THIS MACROS IN DEV_CONFIG.H file 
 *                      USE_LPC_LIKE_MAVLINK_BRIDGE_TO_GS 
 *                      USE_LPC_LIKE_SERIAL_BRIDGE_TO_ESP 
 * 
 * @note U NEAD TO INIT SYS_TIMER AND PUT THIS FUNCTION IN MAIN
 *                uint32_t GetTick(void) {return sys_tick_ms;}
}
 */

#ifndef VESC_H
#define VESC_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "can.h"
#include "mav.h"
#include "uart.h"


/*******************************************************************************
 * Defines - Config
 ******************************************************************************/

#define UART_BAUD_RATE_VESC   115200
#define MINE_CAN_ID           0
#define VESC_CAN_HAL_INSTANCE CAN_HAL_INSTANCE_1
#define GET_VALUE_LENGTH      79
#define W8_FOR_STATUS_US      1000000

/*******************************************************************************
 * Enumes
 ******************************************************************************/

typedef enum {
CAN_PACKET_SET_DUTY                      = 0,  //  :)
CAN_PACKET_SET_CURRENT                   = 1,  //  :)
CAN_PACKET_SET_CURRENT_BRAKE             = 2,  //  :)
CAN_PACKET_SET_RPM                       = 3,  //  :)
CAN_PACKET_SET_POS                       = 4,  //  :)
CAN_PACKET_FILL_RX_BUFFER                = 5,  //  :)
CAN_PACKET_FILL_RX_BUFFER_LONG           = 6,  //  :)
CAN_PACKET_PROCESS_RX_BUFFER             = 7,  //  :)
CAN_PACKET_PROCESS_SHORT_BUFFER          = 8,  //  :)
CAN_PACKET_STATUS                        = 9,  //  :)
CAN_PACKET_SET_CURRENT_REL               = 10, //  :)
CAN_PACKET_SET_CURRENT_BRAKE_REL         = 11, //  :)
CAN_PACKET_SET_CURRENT_HANDBRAKE         = 12, //  :)
CAN_PACKET_SET_CURRENT_HANDBRAKE_REL     = 13, //  :)
CAN_PACKET_STATUS_2                      = 14, //  :)
CAN_PACKET_STATUS_3                      = 15, //  :)
CAN_PACKET_STATUS_4                      = 16, //  :)
CAN_PACKET_PING                          = 17, //  :)
CAN_PACKET_PONG                          = 18, //  :)
CAN_PACKET_DETECT_APPLY_ALL_FOC          = 19, //  :)
CAN_PACKET_DETECT_APPLY_ALL_FOC_RES      = 20, //  :)
CAN_PACKET_CONF_CURRENT_LIMITS           = 21, //  :)
CAN_PACKET_CONF_STORE_CURRENT_LIMITS     = 22, //  :)
CAN_PACKET_CONF_CURRENT_LIMITS_IN        = 23, //  :)
CAN_PACKET_CONF_STORE_CURRENT_LIMITS_IN  = 24, //  :)
CAN_PACKET_CONF_FOC_ERPM                 = 25, //  :)
CAN_PACKET_CONF_STORE_FOC_ERPM           = 26, //  :)
CAN_PACKET_STATUS_5                      = 27, //  :)
CAN_PACKET_STATUS_6                      = 58, //  :)
CAN_PACKET_CUSTOM_STATUS                 = 60,
CAN_PACKET_MAKE_ENUM_32_BITS             = 0xFFFFFFFF
} command_numbers_e;    //DELAJ FUKCIJE POSTOJE TODOOOO

typedef struct {
    float temp_mos;
    float temp_motor;
    float current_motor;
    float current_in;
    float id;
    float iq;
    float duty_now;
    float rpm;
    float v_in;
    float amp_hours;
    float amp_hours_charged;
    float watt_hours;
    float watt_hours_charged;
    int32_t tachometer;
    int32_t tachometer_abs;
    uint8_t fault_code;
    float pid_pos;
    uint8_t vesc_id;
} vesc_values_t;


typedef enum {
    STATE_WAIT_START,
    STATE_LEN,
    STATE_PAYLOAD,
    STATE_CRC_HIGH,
    STATE_CRC_LOW,
    STATE_END
} parser_state_t;

typedef struct {
    parser_state_t state;
    uint8_t payload[256];
    uint16_t payload_len;
    uint16_t payload_idx;
    uint16_t crc_calc;
    uint16_t crc_rx;
} vesc_parser_t;

/*******************************************************************************
 * Structs
 ******************************************************************************/
// Status 1: duty cycle, total current, rpm
typedef struct {
    uint8_t status_id;
    uint8_t vesc_id;       
    float duty_cycle;
    float total_current;
    float rpm;
} VescStatus1_t;

// Status 2: amp-hours charged and consumed
typedef struct {
    uint8_t status_id;
    uint8_t vesc_id;
    float amp_hours_charged;
    float amp_hours;
} VescStatus2_t;

// Status 3: watt-hours charged and consumed
typedef struct {
    uint8_t status_id;
    uint8_t vesc_id;
    float watt_hours_charged;
    float watt_hours;
} VescStatus3_t;

// Status 4: PID position, input current, motor temp, FET temp
typedef struct {
    uint8_t status_id;
    uint8_t vesc_id;
    float pid_pos;
    float total_current_in;
    float motor_temp;
    float fet_temp;
} VescStatus4_t;

// Status 5: input voltage, tachometer
typedef struct {
    uint8_t status_id;
    uint8_t vesc_id;
    float input_voltage;
    float tachometer_value;
} VescStatus5_t;

// Status 6: ADC1, ADC2, ADC3, PPM
typedef struct {
    uint8_t status_id;
    uint8_t vesc_id;
    float adc1;
    float adc2;
    float adc3;
    float ppm;
} VescStatus6_t;


/*******************************************************************************
 * Prothotypes
 ******************************************************************************/


/*bear minimum*/

/**
 * @brief Sets the PWM duty cycle on a VESC.
 *
 * Sends a command to the VESC with ID @p vesc_id
 * to update its duty cycle to @p duty_cycle.
 *
 * @param vesc_id ID of the target VESC on CAN or serial bus
 * @param duty_cycle Desired duty cycle, in the range -1.0 to 1.0
 * @return lStatus_t
 *         Returns a status code indicating success or failure of the command:
 *         - 0 / STATUS_OK: Command successfully sent and acknowledged.
 *         - non-zero: An error occurred while sending the command.
 */
lStatus_t vesc_set_duty(uint8_t vesc_id, float duty_cycle);


/**
 * @brief  Sets the target motor current on a VESC via CAN.
 *
 * This function sends a CAN command to the VESC identified by `vesc_id`
 * to set the desired motor current in amperes. The current value can be
 * positive or negative depending on the direction of rotation.
 *
 * @param  vesc_id  The ID of the target VESC on the CAN bus.
 * @param  current  The desired motor current in amperes (A).
 *
 * @return lStatus_t
 *         Returns a status code indicating success or failure of the command:
 *         - 0 / STATUS_OK: Command successfully sent and acknowledged.
 *         - non-zero: An error occurred while sending the command.
 *
 * @note   The VESC must be configured to accept current control commands.
 *         Make sure that the requested current does not exceed the VESC's
 *         configured current limits to avoid triggering faults.
 */
lStatus_t vesc_set_current(uint8_t vesc_id, float current);

/**
 * @brief  Sends a braking current command to a VESC via CAN.
 *
 * This function sends a CAN command to the VESC identified by `vesc_id`
 * to apply a braking (regenerative or resistive) current in amperes.
 * The value can be positive or negative depending on the desired braking direction.
 *
 * @param  vesc_id  The ID of the target VESC on the CAN bus.
 * @param  current  The braking current in amperes (A). Positive value applies brake.
 *
 * @return lStatus_t
 *         Returns a status code indicating success or failure of the command:
 *         - 0 / STATUS_OK: Command successfully sent and acknowledged.
 *         - non-zero: An error occurred while sending the command.
 *
 * @note   Ensure the VESC is configured to accept current braking commands
 *         and that the requested current does not exceed the configured limits.
 */
lStatus_t vesc_set_current_brake(uint8_t vesc_id, float current);

/**
 * @brief  Sets the target motor speed in RPM via CAN.
 *
 * Sends a CAN command to the VESC identified by `vesc_id`
 * to regulate motor speed to the specified target RPM.
 *
 * @param  vesc_id  The ID of the target VESC on the CAN bus.
 * @param  rpm      Desired motor speed in revolutions per minute (ERPM).
 *                  Positive values rotate forward, negative backward.
 *
 * @return lStatus_t
 *         Status code of the operation:
 *         - 0 / STATUS_OK: Command sent successfully.
 *         - non-zero: Error while sending the command.
 *
 * @note   INPUT IS NOT RPM, INPUT IS ERPM.
 */
lStatus_t vesc_set_rpm(uint8_t vesc_id, float rpm);

/**
 * @brief Sets the desired motor position on a VESC.
 *
 * This function sends a CAN or serial message to the VESC with the specified ID
 * to move the motor to the given position.
 *
 * @param vesc_id The ID of the target VESC.
 * @param pos Desired motor position (in degries).
 *
 *  @return lStatus_t
 *         Status code of the operation:
 *         - 0 / STATUS_OK: Command sent successfully.
 *         - non-zero: Error while sending the command.
 * 
 */
lStatus_t vesc_set_pos(uint8_t vesc_id, float pos);

/**
 * @brief Sets the desired relative motor current on a VESC.
 *
 * This function sends a CAN or serial message to the VESC with the specified ID
 * to apply the given relative current to the motor.
 *
 * @param vesc_id The ID of the target VESC.
 * @param current_rel Desired motor current as a relative value (typically -1.0 to 1.0).
 *
 * @return lStatus_t
 *         Status code of the operation:
 *         - 0 / STATUS_OK: Command sent successfully.
 *         - non-zero: Error while sending the command.
 */
lStatus_t vesc_set_current_rel(uint8_t vesc_id, float current_rel);

/**
 * @brief Sets the desired relative braking current on a VESC.
 *
 * This function sends a CAN or serial message to the VESC with the specified ID
 * to apply the given relative braking current to the motor.
 *
 * @param vesc_id The ID of the target VESC.
 * @param current_rel Desired braking current as a relative value (-1.0 to 1.0).
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_set_current_brake_rel(uint8_t vesc_id, float current_rel);

/**
 * @brief Applies a handbrake current to the motor on a VESC.
 *
 * This function sends a CAN or serial message to the VESC with the specified ID
 * to immediately apply a fixed handbrake current.
 *
 * @param vesc_id The ID of the target VESC.
 * @param current Desired handbrake current in amperes.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_set_current_handbrake(uint8_t vesc_id, float current);

/**
 * @brief Applies a relative handbrake current to the motor on a VESC.
 *
 * This function sends a CAN or serial message to the VESC with the specified ID
 * to immediately apply a relative handbrake current.
 *
 * @param vesc_id The ID of the target VESC.
 * @param current_rel Desired handbrake current as a relative value (-1.0 to 1.0).
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_set_current_handbrake_rel(uint8_t vesc_id, float current_rel);


/**
 * @brief Requests VESC status 1: duty cycle, total current, and RPM.
 *
 * @param vesc_id ID of the target VESC.
 * @param duty_cycle Pointer to store the duty cycle (-1.0 to 1.0).
 * @param total_current Pointer to store the total motor current in amperes.
 * @param rpm Pointer to store the motor ERPM.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t request_vesc_status_1(uint8_t vesc_id, float *duty_cycle, float *total_current, float *rpm);

/**
 * @brief Requests VESC status 2: amp-hours charged and consumed.
 *
 * @param vesc_id ID of the target VESC.
 * @param amp_hours_charged Pointer to store amp-hours charged.
 * @param amp_hours Pointer to store amp-hours consumed.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t request_vesc_status_2(uint8_t vesc_id, float *amp_hours_charged, float *amp_hours);

/**
 * @brief Requests VESC status 3: watt-hours charged and consumed.
 *
 * @param vesc_id ID of the target VESC.
 * @param watt_hours_charged Pointer to store watt-hours charged.
 * @param watt_hours Pointer to store watt-hours consumed.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t request_vesc_status_3(uint8_t vesc_id, float *watt_hours_charged, float *watt_hours);

/**
 * @brief Requests VESC status 4: PID position, input current, motor and FET temperatures.
 *
 * @param vesc_id ID of the target VESC.
 * @param pid_pos Pointer to store PID position (in degries).
 * @param total_current_in Pointer to store input current in amperes.
 * @param motor_temp Pointer to store motor temperature in °C.
 * @param fet_temp Pointer to store FET temperature in °C.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t request_vesc_status_4(uint8_t vesc_id, float *pid_pos, float *total_current_in, float *motor_temp, float *fet_temp);

/**
 * @brief Requests VESC status 5: input voltage and tachometer value.
 *
 * @param vesc_id ID of the target VESC.
 * @param input_voltage Pointer to store input voltage in volts.
 * @param tachometer_value Pointer to store tachometer value .
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t request_vesc_status_5(uint8_t vesc_id, float *input_voltage, float *tachometer_value);


/*princess treatment */


/**
 * @brief Requests VESC status 6: ADC and PPM values.
 *
 * @param vesc_id ID of the target VESC.
 * @param adc1 Pointer to store ADC1 value.
 * @param adc2 Pointer to store ADC2 value.
 * @param adc3 Pointer to store ADC3 value.
 * @param ppm  Pointer to store PPM input value.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t request_vesc_status_6(uint8_t vesc_id, 
                                float  *adc1,
                                float  *adc2,
                                float  *adc3,
                                float  *ppm);

/**
 * @brief Sends a PONG message to the target VESC.
 *
 * @param vesc_id ID of the target VESC.
 * @param mine_id ID to include in the PONG message.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_pong(uint8_t vesc_id, uint8_t mine_id);

/**
 * @brief Sends a PING message to the target VESC and receives a response.
 *
 * @param vesc_id ID of the target VESC.
 * @param response_id Pointer to store the ID received in the PONG response.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_ping(uint8_t vesc_id, uint8_t mine_id, uint8_t *response_id);

/**
 * @brief Sets the minimum and maximum current limits for the target VESC (temporary).
 *
 * @param vesc_id ID of the target VESC.
 * @param min Minimum current limit in amperes.
 * @param max Maximum current limit in amperes.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_conf_set_current_limits(uint8_t vesc_id, float min, float max);

/**
 * @brief Sets the minimum and maximum current-in limits for the target VESC (temporary).
 *
 * @param vesc_id ID of the target VESC.
 * @param min Minimum current-in limit in amperes.
 * @param max Maximum current-in limit in amperes.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_conf_set_current_in_limits(uint8_t vesc_id, float min, float max);

/**
 * @brief Sets and stores the minimum and maximum current limits for the target VESC (persistent).
 *
 * @param vesc_id ID of the target VESC.
 * @param min Minimum current limit in amperes.
 * @param max Maximum current limit in amperes.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_conf_set_current_limits_store(uint8_t vesc_id, float min, float max);

/**
 * @brief Sets and stores the minimum and maximum current-in limits for the target VESC (persistent).
 *
 * @param vesc_id ID of the target VESC.
 * @param min Minimum current-in limit in amperes.
 * @param max Maximum current-in limit in amperes.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_conf_set_current_in_limits_store(uint8_t vesc_id, float min, float max);

/**
 * @brief Sets the FOC open-loop RPM and sensorless RPM for the target VESC (temporary).
 *
 * @param vesc_id ID of the target VESC.
 * @param foc_openloop_rpm Open-loop RPM for FOC control.
 * @param foc_sl_erpm Sensorless RPM threshold for FOC control.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_conf_foc_erpm(uint8_t vesc_id, float foc_openloop_rpm, float foc_sl_erpm);

/**
 * @brief Sets and stores the FOC open-loop RPM and sensorless RPM for the target VESC (persistent).
 *
 * @param vesc_id ID of the target VESC.
 * @param foc_openloop_rpm Open-loop RPM for FOC control.
 * @param foc_sl_erpm Sensorless RPM threshold for FOC control.
 *
 * @return lStatus_t Status code of the operation.
 */
lStatus_t vesc_conf_foc_erpm_store(uint8_t vesc_id, float foc_openloop_rpm, float foc_sl_erpm);

/**
 * @brief Sends a DETECT_APPLY_ALL_FOC command to the target VESC, optionally waiting for a response.
 *
 * This function requests the VESC to perform FOC detection and apply it. It can optionally wait
 * for active status messages and return a success/fail status based on the response.
 *
 * @param vesc_id ID of the target VESC on the CAN bus.
 * @param active_status_msgs If true, waits for response messages to confirm detection.
 * @param max_power_loss Maximum allowed power loss during FOC detection.
 *
 * @return lStatus_t Status code of the operation (Success, Fail, or Timeout).
 */
lStatus_t vesc_detect_apply_all_foc(uint8_t vesc_id, bool active_status_msgs, float max_power_loss);

/**
 * @brief Sends a data buffer to the target VESC over CAN bus, handling short and long messages.
 *
 * This function splits the data into appropriately sized CAN messages and sends them sequentially.
 * It calculates CRC for the entire buffer when sending long messages.
 *
 * @param vesc_id ID of the target VESC on the CAN bus.
 * @param data Pointer to the data buffer to be sent.
 * @param len Length of the data buffer in bytes.
 * @param send Command or identifier to include with the buffer.
 *
 * @return lStatus_t Status code of the operation (Success, Fail, or Timeout).
 */
lStatus_t vesc_send_buffer(uint8_t vesc_id, uint8_t *data, uint16_t len, uint8_t send);


#ifdef __cplusplus
}
#endif



#endif /* VESC_H */