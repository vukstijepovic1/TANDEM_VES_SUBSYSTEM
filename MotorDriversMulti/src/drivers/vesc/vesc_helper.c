/**
 * @file    vesc_helper.h
 * @brief   Helper functions for vesc library.
 * @version	0.0.1
 * @date    13.08.2025
 * @author  BetaTehPro
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "vesc_helper.h"
#include <string.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void prepare_tx_can_msg (can_hal_msg_t *msg, 
                        command_numbers_e command_number,
                        uint8_t vesc_id,
                        uint8_t length,
                        can_hal_id_format_t idFormat,
                        can_hal_frame_t frameType){

    memset(msg,0,sizeof(*msg));
    msg->messageId = ((uint32_t)0 << 18) | ((uint32_t)command_number   << 8) | vesc_id;
    msg->idFormat = idFormat;
    msg->frameType = frameType;
    msg->length = length;

}

void pack_float_to_msg_data(uint8_t *data_buffer, float value, float scale){
    int32_t scaled = (int32_t)(value * scale);

    data_buffer[0] = (uint8_t)((scaled >> 24) & 0xFF); 
    data_buffer[1] = (uint8_t)((scaled >> 16) & 0xFF);
    data_buffer[2] = (uint8_t)((scaled >> 8) & 0xFF);
    data_buffer[3] = (uint8_t)(scaled & 0xFF); 
}

uint16_t crc16_for_serial(const uint8_t *data, uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (uint16_t j = 0; j < len; j++) {
        crc ^= data[j] << 8;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}


int fill_uart_packet(uint8_t *packet, const uint8_t *data, uint8_t len, uint8_t status_id){
    packet[0]=0x24;
    packet[1]=0x07;

    packet[2]=len;
    packet[3]=status_id;


    memcpy(&packet[4], data, len);

    uint16_t crc = crc16_for_serial(&packet[2], len + 1); 
    packet[len+4] = (crc >> 8) & 0xFF;  
    packet[len+5] =  crc & 0xFF;      

    return len+6;

}
const unsigned short crc16_tab[] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084,
		0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad,
		0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7,
		0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a,
		0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672,
		0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719,
		0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7,
		0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948,
		0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50,
		0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b,
		0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
		0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97,
		0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe,
		0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca,
		0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3,
		0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d,
		0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214,
		0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c,
		0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3,
		0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d,
		0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806,
		0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e,
		0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1,
		0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b,
		0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0,
		0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
		0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0 };

uint16_t crc16(const uint8_t *data, uint16_t len) {

    uint16_t i=0;
    uint16_t crc=0;

    for(i=0; i<len; i++){
        crc=crc16_tab[(((crc>>8)^data[i]) &0xFF)]^(crc<<8);
    }

    return crc;
}

void vesc_parser_init(vesc_parser_t *p) {
    p->state = STATE_WAIT_START;
    p->payload_len = 0;
    p->payload_idx = 0;
    p->crc_calc = 0;
    p->crc_rx = 0;
}

int vesc_build_get_values(uint8_t *out_buf) {

    uint8_t payload[1];
    uint8_t len = 0x01;   
    payload[0] = COMM_GET_VALUES; 
    uint16_t crc = crc16(payload,1);

    int idx = 0;
    out_buf[idx++] = 0x02;      
    out_buf[idx++] = len;       
    out_buf[idx++] = payload[0];
    out_buf[idx++] = (crc >> 8) & 0xFF; 
    out_buf[idx++] = crc & 0xFF;       
    out_buf[idx++] = 0x03;  
    out_buf[idx] = 0x00; 

    return idx; 
}

float buffer_get_float16(const uint8_t *buf, float scale, int *index) {
    uint16_t val = (buf[*index] << 8) | buf[*index + 1];
    *index += 2;
    
    int16_t sval = (int16_t)val;
    return sval / scale;
}


float buffer_get_float32(const uint8_t *buf, float scale, int *index) {
    uint32_t val = (buf[*index] << 24) | (buf[*index + 1] << 16) |
                   (buf[*index + 2] << 8) | buf[*index + 3];
    *index += 4;
    int32_t sval = (int32_t)val;
    return sval / scale;
}


int32_t buffer_get_int32(const uint8_t *buf, int *index) {
    uint32_t val = (buf[*index] << 24) | (buf[*index + 1] << 16) |
                   (buf[*index + 2] << 8) | buf[*index + 3];
    *index += 4;
    return (int32_t)val;
}

bool vesc_parser_process_get_values(vesc_parser_t *p, uint8_t byte, vesc_values_t *values) {
    switch (p->state) {
        case STATE_WAIT_START:
            if (byte == 0x02) p->state = STATE_LEN;
            break;

        case STATE_LEN:
            p->payload_len = byte;
            p->payload_idx = 0;
            p->state = STATE_PAYLOAD;
            break;

        case STATE_PAYLOAD:
            p->payload[p->payload_idx++] = byte;
            if (p->payload_idx >= p->payload_len) p->state = STATE_CRC_HIGH;
            break;

        case STATE_CRC_HIGH:
            p->crc_rx = (uint16_t)byte << 8;
            p->state = STATE_CRC_LOW;
            break;

        case STATE_CRC_LOW:
            p->crc_rx |= byte;
            p->crc_calc = crc16(p->payload, p->payload_len);
            p->state = STATE_END;
            break;

        case STATE_END:
            if (byte == 0x03 && p->crc_rx == p->crc_calc) {
                if (p->payload[0] == 0x04) { // COMM_GET_VALUES
                    int idx = 1;
                    values->temp_mos             = buffer_get_float16(p->payload, 1e1, &idx);
                    values->temp_motor           = buffer_get_float16(p->payload, 1e1, &idx);
                    values->current_motor        = buffer_get_float32(p->payload, 1e2, &idx);
                    values->current_in           = buffer_get_float32(p->payload, 1e2, &idx);
                    values->id                   = buffer_get_float32(p->payload, 1e2, &idx);
                    values->iq                   = buffer_get_float32(p->payload, 1e2, &idx);
                    values->duty_now             = buffer_get_float16(p->payload, 1e3, &idx);
                    values->rpm                  = buffer_get_float32(p->payload, 1e0, &idx);
                    values->v_in                 = buffer_get_float16(p->payload, 1e1, &idx);
                    values->amp_hours            = buffer_get_float32(p->payload, 1e4, &idx);
                    values->amp_hours_charged    = buffer_get_float32(p->payload, 1e4, &idx);
                    values->watt_hours           = buffer_get_float32(p->payload, 1e4, &idx);
                    values->watt_hours_charged   = buffer_get_float32(p->payload, 1e4, &idx);
                    values->tachometer           = buffer_get_int32(p->payload, &idx);
                    values->tachometer_abs       = buffer_get_int32(p->payload, &idx);
                    values->fault_code           = p->payload[idx++];

                    if (idx < p->payload_len) values->pid_pos = buffer_get_float32(p->payload, 1e6, &idx);
                    else values->pid_pos = 0.0f;

                    if (idx < p->payload_len) values->vesc_id = p->payload[idx++];
                    else values->vesc_id = 255;

                    vesc_parser_init(p); // reset parser
                    return true; 
                }
            }
            vesc_parser_init(p); 
            break;
    }
    return false;
}

void parse_vesc_status_1_from_can(can_hal_msg_t *msg, messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 *status)
{
    status->erpm = (float)((int32_t)((msg->data[0] << 24) | (msg->data[1] << 16) | 
                                      (msg->data[2] << 8) | msg->data[3]));
    status->_total_current = (int16_t)((msg->data[4] << 8) | msg->data[5]) / 10.0f;
    status->duty = (int16_t)((msg->data[6] << 8) | msg->data[7]) / 1000.0f;
}

void parse_vesc_status_2_from_can(can_hal_msg_t *msg, messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 *status)
{
    status->amp_hours = (float)((int32_t)((msg->data[0] << 24) | (msg->data[1] << 16) | 
                                           (msg->data[2] << 8) | msg->data[3])) / 10000.0f;
    status->amp_hours_charged = (float)((int32_t)((msg->data[4] << 24) | (msg->data[5] << 16) | 
                                                   (msg->data[6] << 8) | msg->data[7])) / 10000.0f;
}

void parse_vesc_status_3_from_can(can_hal_msg_t *msg, messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 *status)
{
    status->watt_hours = (float)((int32_t)((msg->data[0] << 24) | (msg->data[1] << 16) | 
                                            (msg->data[2] << 8) | msg->data[3])) / 10000.0f;
    status->watt_hours_charged = (float)((int32_t)((msg->data[4] << 24) | (msg->data[5] << 16) | 
                                                    (msg->data[6] << 8) | msg->data[7])) / 10000.0f;
}

void parse_vesc_status_4_from_can(can_hal_msg_t *msg, messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 *status)
{
    status->fet_temp = (uint16_t)((msg->data[0] << 8) | msg->data[1]) / 10.0f;
    status->motor_temp = (int16_t)((msg->data[2] << 8) | msg->data[3]) / 10.0f;
    status->current_in = (int16_t)((msg->data[4] << 8) | msg->data[5]) / 10.0f;
    status->pos = (int16_t)((msg->data[6] << 8) | msg->data[7]) / 50.0f;
}

void parse_vesc_status_5_from_can(can_hal_msg_t *msg, messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 *status)
{
    status->tachometar = (int32_t)((msg->data[0] << 24) | (msg->data[1] << 16) | 
                                    (msg->data[2] << 8) | msg->data[3]) / 6.0f;  
    status->voltage = (uint16_t)((msg->data[4] << 8) | msg->data[5]) / 10.0f;
}

void parse_vesc_status_6_from_can(can_hal_msg_t *msg, messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 *status)
{
    status->adc1 = (int16_t)((msg->data[0] << 8) | msg->data[1]) / 1000.0f;
    status->adc2 = (int16_t)((msg->data[2] << 8) | msg->data[3]) / 1000.0f;
    status->adc3 = (int16_t)((msg->data[4] << 8) | msg->data[5]) / 1000.0f;
    status->ppm = (int16_t)((msg->data[6] << 8) | msg->data[7]) / 1000.0f;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/