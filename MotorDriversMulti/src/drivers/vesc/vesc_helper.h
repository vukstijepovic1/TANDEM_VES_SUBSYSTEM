/**
 * @file    vesc_helper.h
 * @brief   Helper functions for vesc library.
 * @version	0.0.1
 * @date    13.08.2025
 * @author  BetaTehPro
 */

#ifndef VESC_HELPER_H
#define VESC_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include "can.h"
#include "vesc.h"
#include "types.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
 #define COMM_GET_VALUES 4
/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
/**
 * @brief Prepare a CAN message with given parameters.
 * 
 * This function initializes a CAN message structure with the specified
 * command number, VESC ID, data length, ID format, and frame type.
 * It does not populate the data payload itself.
 * 
 * @param msg Pointer to the CAN message structure to prepare.
 * @param command_number Command identifier to be set in the message.
 * @param vesc_id VESC device ID for the target.
 * @param length Length of the CAN message data in bytes (0-8).
 * @param idFormat Format of the CAN ID (standard or extended).
 * @param frameType Type of CAN frame (data or remote frame).
 */
void prepare_tx_can_msg (can_hal_msg_t *msg, 
                        command_numbers_e command_number,
                        uint8_t vesc_id,
                        uint8_t length,
                        can_hal_id_format_t idFormat,
                        can_hal_frame_t frameType);

/**
 * @brief Pack a floating-point value into a CAN message data buffer.
 * 
 * Converts a floating-point value into a scaled integer representation
 * and stores it into the provided data buffer in a format suitable
 * for CAN transmission.
 * 
 * @param data_buffer Pointer to the CAN message data buffer (8 bytes).
 * @param value Floating-point value to pack.
 * @param scale Scaling factor to apply to the value before packing.
 */
void pack_float_to_msg_data(uint8_t *data_buffer, float value, float scale);

/**
 * @brief Compute CRC16 checksum for serial data.
 * 
 * @attention THIS IS CRC FOR COMUNICATION WITH ESP
 * 
 * Calculates the CRC16 checksum for the provided data buffer using
 * a standard polynomial. Useful for validating integrity of serial
 * or CAN message payloads.
 * 
 * @param data Pointer to the data buffer to calculate CRC for.
 * @param len Length of the data buffer in bytes.
 * @return Computed CRC16 value.
 */
uint16_t crc16_for_serial(const uint8_t *data, uint16_t len);

/**
 * @brief Build a complete UART packet for VESC communication.
 *  
 * @attention THIS IS UART PACKET FOR COMUNICATION WITH ESP
 * 
 * Constructs a UART packet including header, payload, and CRC for transmission.
 * 
 * @param packet Pointer to the buffer where the packet will be stored.
 * @param data Pointer to the payload data to include in the packet.
 * @param len Length of the payload data in bytes.
 * @param status_id Message type or command ID for the packet.
 * @return Number of bytes written into the packet buffer.
 */
int fill_uart_packet(uint8_t *packet, const uint8_t *data, uint8_t len, uint8_t status_id);

/**
 * @brief Calculate CRC-16 checksum for a data buffer.
 * 
 * @attention THIS CRC FOR COMUNICATION WITH ESP
 * 
 * Computes a 16-bit CRC used to verify the integrity of a packet.
 * 
 * @param data Pointer to the data buffer.
 * @param len Length of the data buffer in bytes.
 * @return Computed CRC-16 value.
 */
uint16_t crc16(const uint8_t *data, uint16_t len);

/**
 * @brief Initialize a VESC parser structure.
 * 
 * Resets internal state variables of the parser to prepare for new incoming data.
 * 
 * @param p Pointer to the vesc_parser_t structure to initialize.
 */
void vesc_parser_init(vesc_parser_t *p);

/**
 * @brief Build a UART packet with custom data.
 *
 * Creates a VESC UART packet with the given data and status ID.
 *
 * @param packet    Output buffer for the built packet.
 * @param data      Pointer to payload data to include.
 * @param len       Length of payload data.
 * @param status_id Identifier of the command/status type.
 * @return Number of bytes written into the packet.
 */
int fill_uart_packet(uint8_t *packet, const uint8_t *data, uint8_t len, uint8_t status_id);

/**
 * @brief Build a UART packet to request VESC values.
 *
 * Generates a COMM_GET_VALUES packet used to query motor and battery data.
 *
 * @param out_buf Output buffer for the request packet.
 * @return Number of bytes written into the buffer.
 */
int vesc_build_get_values(uint8_t *out_buf);

/**
 * @brief Extract a 16-bit floating-point value from a buffer.
 *
 * Reads two bytes from the buffer, scales the value, and increments the index.
 *
 * @param buf   Pointer to the source buffer.
 * @param scale Scale factor to apply to the extracted value.
 * @param index Pointer to the current buffer index, updated after reading.
 * @return Extracted floating-point value.
 */
float buffer_get_float16(const uint8_t *buf, float scale, int *index);

/**
 * @brief Extract a 32-bit floating-point value from a buffer.
 *
 * Reads four bytes from the buffer, scales the value, and increments the index.
 *
 * @param buf   Pointer to the source buffer.
 * @param scale Scale factor to apply to the extracted value.
 * @param index Pointer to the current buffer index, updated after reading.
 * @return Extracted floating-point value.
 */
float buffer_get_float32(const uint8_t *buf, float scale, int *index);

/**
 * @brief Extract a 32-bit integer from a buffer.
 *
 * Reads four bytes from the buffer in big-endian order and increments the index.
 *
 * @param buf   Pointer to the source buffer.
 * @param index Pointer to the current buffer index, updated after reading.
 * @return Extracted 32-bit integer.
 */
int32_t buffer_get_int32(const uint8_t *buf, int *index);

/**
 * @brief Parse VESC response packets for COMM_GET_VALUES.
 *
 * Processes incoming UART bytes and updates the values structure when a full
 * response packet is received.
 *
 * @param p       Pointer to the parser state structure.
 * @param byte    Incoming UART byte to process.
 * @param values  Pointer to structure to store parsed VESC values.
 * @return true if a full and valid packet has been parsed, false otherwise.
 */
bool vesc_parser_process_get_values(vesc_parser_t *p, uint8_t byte, vesc_values_t *values);

/**
 * @brief Parse VESC STATUS_1 CAN message (duty, current, eRPM).
 * 
 * Decodes an incoming CAN message containing VESC status packet type 1.
 * Extracts duty cycle (×1000), total current (×10), and electrical RPM.
 * 
 * @param msg    Pointer to received CAN message structure.
 * @param status Pointer to LisumVescStatus1 structure to populate.
 */
void parse_vesc_status_1_from_can(can_hal_msg_t *msg, 
                         messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 *status);

/**
 * @brief Parse VESC STATUS_2 CAN message (amp hours).
 * 
 * Decodes an incoming CAN message containing VESC status packet type 2.
 * Extracts amp hours consumed and amp hours charged (both ×10000).
 * 
 * @param msg    Pointer to received CAN message structure.
 * @param status Pointer to LisumVescStatus2 structure to populate.
 */
void parse_vesc_status_2_from_can(can_hal_msg_t *msg, 
                         messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 *status);

/**
 * @brief Parse VESC STATUS_3 CAN message (watt hours).
 * 
 * Decodes an incoming CAN message containing VESC status packet type 3.
 * Extracts watt hours consumed and watt hours charged (both ×10000).
 * 
 * @param msg    Pointer to received CAN message structure.
 * @param status Pointer to LisumVescStatus3 structure to populate.
 */
void parse_vesc_status_3_from_can(can_hal_msg_t *msg, 
                         messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 *status);

/**
 * @brief Parse VESC STATUS_4 CAN message (temperatures, position, current in).
 * 
 * Decodes an incoming CAN message containing VESC status packet type 4.
 * Extracts FET temperature (×10), motor temperature (×10), input current (×10),
 * and PID position (×50).
 * 
 * @param msg    Pointer to received CAN message structure.
 * @param status Pointer to LisumVescStatus4 structure to populate.
 */
void parse_vesc_status_4_from_can(can_hal_msg_t *msg, 
                         messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 *status);

/**
 * @brief Parse VESC STATUS_5 CAN message (voltage, tachometer).
 * 
 * Decodes an incoming CAN message containing VESC status packet type 5.
 * Extracts tachometer value (÷6) and input voltage (×10).
 * 
 * @param msg    Pointer to received CAN message structure.
 * @param status Pointer to LisumVescStatus5 structure to populate.
 */
void parse_vesc_status_5_from_can(can_hal_msg_t *msg, 
                         messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 *status);

/**
 * @brief Parse VESC STATUS_6 CAN message (ADC channels, PPM).
 * 
 * Decodes an incoming CAN message containing VESC status packet type 6.
 * Extracts ADC1, ADC2, ADC3, and PPM values (all ×1000).
 * 
 * @param msg    Pointer to received CAN message structure.
 * @param status Pointer to LisumVescStatus6 structure to populate.
 */
void parse_vesc_status_6_from_can(can_hal_msg_t *msg, 
                         messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 *status);


#ifdef __cplusplus
}
#endif

#endif /* VESC_HELPER_H */