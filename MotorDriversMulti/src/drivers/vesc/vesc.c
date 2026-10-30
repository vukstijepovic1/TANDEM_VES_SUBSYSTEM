/**
 * @file    ftp_handler.c
 * @brief   Drivers for VESC.
 * @version	0.0.1
 * @date    12.08.2025
 * @author  BetaTehPro
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "vesc.h"
#include "types.h"
#include "can.h"
#include "mav.h"
#include "uart.h"
#include "util.h"


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
lStatus_t status;
lStatus_t status2;
/*******************************************************************************
 * Code
 ******************************************************************************/


lStatus_t vesc_set_duty(uint8_t vesc_id, float duty_cycle){

    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_DUTY, vesc_id, 4,
                        CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, duty_cycle, 100000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_set_current(uint8_t vesc_id, float current){

    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_CURRENT, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, current, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}


lStatus_t vesc_set_current_brake(uint8_t vesc_id, float current){

    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_CURRENT_BRAKE, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, current, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_set_rpm(uint8_t vesc_id, float rpm){

    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_RPM, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, rpm, 1.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_set_pos(uint8_t vesc_id, float pos){

    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_POS, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, pos, 1000000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_set_current_rel(uint8_t vesc_id, float current_rel){
    
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_CURRENT_REL, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, current_rel, 100000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;

}


lStatus_t vesc_set_current_brake_rel(uint8_t vesc_id, float current_rel){
    
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_CURRENT_BRAKE_REL, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, current_rel, 100000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;

}

lStatus_t vesc_set_current_handbrake(uint8_t vesc_id, float current){
    
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_CURRENT_HANDBRAKE, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, current, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;

}

lStatus_t vesc_set_current_handbrake_rel(uint8_t vesc_id, float current_rel){
    
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_SET_CURRENT_HANDBRAKE_REL, vesc_id, 4,
                    CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME); 

    pack_float_to_msg_data(msg.data, current_rel, 100000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;

}



lStatus_t request_vesc_status_1(uint8_t vesc_id, 
                                float  *duty_cycle,
                                float  *total_current,
                                float  *rpm){

    can_hal_msg_t response_msg;

    uint32_t start_time = HAL_GetTimeUS();

    while ( (HAL_GetTimeUS() - start_time) < W8_FOR_STATUS_US ) { 
        status2 = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10); 
        if (status2 == lStatus_Success) {

            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT &&
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)9 << 8)| vesc_id)&&
                response_msg.length) { 
                *duty_cycle    = (int16_t)((response_msg.data[6] << 8) | response_msg.data[7]) / 1000.0f;
                *total_current = (int16_t)((response_msg.data[4] << 8) | response_msg.data[5]) / 10.0f;
                *rpm           = (float)((response_msg.data[0] << 24) |
                                         (response_msg.data[1] << 16) |
                                         (response_msg.data[2] << 8)  |
                                          response_msg.data[3]);    
                return lStatus_Success;
            }
        }
    }


    return lStatus_Timeout;
}

lStatus_t request_vesc_status_2(uint8_t vesc_id, 
                                float  *amp_hours_charged,
                                float  *amp_hours) {

    can_hal_msg_t response_msg;

    uint32_t start_time = HAL_GetTimeUS();

    while ((HAL_GetTimeUS() - start_time) < W8_FOR_STATUS_US) { 
        status = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10);  
        if (status == lStatus_Success) {

            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT &&
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)CAN_PACKET_STATUS_2 << 8) | vesc_id) &&
                response_msg.length == 8){
                *amp_hours         = (float)((response_msg.data[0] << 24) |
                                             (response_msg.data[1] << 16) |
                                             (response_msg.data[2] << 8)  |
                                              response_msg.data[3])/10000.0f;

                *amp_hours_charged = (float)((response_msg.data[4] << 24) |
                                             (response_msg.data[5] << 16) |
                                             (response_msg.data[6] << 8)  |
                                              response_msg.data[7])/10000.0f;
                return lStatus_Success;
            }
        }
    }

    return lStatus_Timeout;
}

lStatus_t request_vesc_status_3(uint8_t vesc_id, 
                                float  *watt_hours_charged,
                                float  *watt_hours) {
    //can_hal_msg_t msg;
    can_hal_msg_t response_msg;

    /*prepare_tx_can_msg(&msg, CAN_PACKET_STATUS_3, vesc_id, 0,
                        CAN_HAL_ID_FORMAT_EXT, CAN_HAL_REMOTE_FRAME);                 

    /*lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    if (status != lStatus_Success) {
        return status; 
    }*/

    uint32_t start_time = HAL_GetTimeUS();

    while ((HAL_GetTimeUS() - start_time) < W8_FOR_STATUS_US) {
        status = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10);
        if (status == lStatus_Success) {
            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT &&
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)CAN_PACKET_STATUS_3 << 8) | vesc_id) &&
                response_msg.length == 8) {
                *watt_hours         = (float)((response_msg.data[0] << 24) |
                                              (response_msg.data[1] << 16) |
                                              (response_msg.data[2] << 8)  |
                                               response_msg.data[3])/10000.0f;

                *watt_hours_charged = (float)((response_msg.data[4] << 24) |
                                              (response_msg.data[5] << 16) |
                                              (response_msg.data[6] << 8)  |
                                               response_msg.data[7])/10000.0f;
                return lStatus_Success;
            }
        }
    }

    return lStatus_Timeout;
}

lStatus_t request_vesc_status_4(uint8_t vesc_id, 
                                float  *pid_pos,
                                float  *total_current_in,
                                float  *motor_temp,
                                float  *fet_temp) {
    //can_hal_msg_t msg;
    can_hal_msg_t response_msg;

    /*prepare_tx_can_msg(&msg, CAN_PACKET_STATUS_4, vesc_id, 0,
                        CAN_HAL_ID_FORMAT_EXT, CAN_HAL_REMOTE_FRAME);                   

    /*lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    if (status != lStatus_Success) {
        return status; 
    }*/

    uint32_t start_time = HAL_GetTimeUS();

    while ((HAL_GetTimeUS() - start_time) < W8_FOR_STATUS_US) {
        status = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10);
        if (status == lStatus_Success) {
            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT &&
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)CAN_PACKET_STATUS_4 << 8) | vesc_id) &&
                response_msg.length == 8) {
                *fet_temp         = ((uint16_t)((response_msg.data[0] << 8)| response_msg.data[1])) / 10.0f;
                *motor_temp       = ((int16_t)((response_msg.data[2] << 8) | response_msg.data[3])) / 10.0f;
                *total_current_in = ((int16_t)((response_msg.data[4] << 8) | response_msg.data[5])) / 10.0f;
                *pid_pos          = ((int16_t)((response_msg.data[6] << 8) | response_msg.data[7])) / 50.0f;

                return lStatus_Success;

            }
        }
    }

    return lStatus_Timeout;
}

lStatus_t request_vesc_status_5(uint8_t vesc_id, 
                                float  *input_voltage,
                                float  *tachometer_value) {
    //can_hal_msg_t msg;
    can_hal_msg_t response_msg;

    /*prepare_tx_can_msg(&msg, CAN_PACKET_STATUS_5, vesc_id, 0,
                        CAN_HAL_ID_FORMAT_EXT, CAN_HAL_REMOTE_FRAME);                    

    /*lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    if (status != lStatus_Success) {
        return status; 
    }*/

    uint32_t start_time = HAL_GetTimeUS();

    while ((HAL_GetTimeUS() - start_time) < W8_FOR_STATUS_US) {
        status = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10);
        if (status == lStatus_Success) {
           
            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT &&
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)CAN_PACKET_STATUS_5 << 8) | vesc_id) &&
                response_msg.length == 8) {
                *input_voltage    = ((uint16_t)((response_msg.data[4] << 8) | response_msg.data[5]))/10.0f;
                *tachometer_value = ((int32_t)((response_msg.data[0] << 24) |
                                               (response_msg.data[1] << 16) |
                                               (response_msg.data[2] << 8)  |
                                                response_msg.data[3]))/6.0f;
                return lStatus_Success;
            }
        }
    }

    return lStatus_Timeout;
}

lStatus_t request_vesc_status_6(uint8_t vesc_id, 
                                float  *adc1,
                                float  *adc2,
                                float  *adc3,
                                float  *ppm) {
    //can_hal_msg_t msg;
    can_hal_msg_t response_msg;

    /*prepare_tx_can_msg(&msg, CAN_PACKET_STATUS_6, vesc_id, 0,
                        CAN_HAL_ID_FORMAT_EXT, CAN_HAL_REMOTE_FRAME);                    

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    /*if (status != lStatus_Success) {
        return status; 
    }*/

    uint32_t start_time = HAL_GetTimeUS();

    while ((HAL_GetTimeUS() - start_time) < W8_FOR_STATUS_US) {
        status = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10);
        if (status == lStatus_Success) {
           
            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT &&
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)CAN_PACKET_STATUS_6 << 8) | vesc_id) &&
                response_msg.length == 8) {
                *adc1 = ((int16_t)((response_msg.data[0] << 8) | response_msg.data[1])) / 1000.0f;
                *adc2 = ((int16_t)((response_msg.data[2] << 8) | response_msg.data[3])) / 1000.0f;
                *adc3 = ((int16_t)((response_msg.data[4] << 8) | response_msg.data[5])) / 1000.0f;
                *ppm  = ((int16_t)((response_msg.data[6] << 8) | response_msg.data[7])) / 1000.0f;
                return lStatus_Success;
            }
        }
    }

    return lStatus_Timeout;
}

lStatus_t vesc_ping(uint8_t vesc_id, uint8_t mine_id, uint8_t *response_id) {
    can_hal_msg_t msg;
    can_hal_msg_t response_msg;


    uint8_t buffer[1];
    buffer[0] = mine_id; 
    prepare_tx_can_msg(&msg, CAN_PACKET_PING, vesc_id, 1,
                        CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    memcpy(msg.data, buffer, 1);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    if (status != lStatus_Success) {
        return status; 
    }

    uint32_t start_time = HAL_GetTimeUS();

    while ((HAL_GetTimeUS() - start_time) < 4000) {
        status = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10);
        if (status == lStatus_Success) {
            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT && 
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)CAN_PACKET_PONG << 8) | MINE_CAN_ID) &&
                response_msg.length >= 2) 
            {
                *response_id = response_msg.data[0];
                return lStatus_Success;
            }
        }
    }

    return lStatus_Timeout;
}

lStatus_t vesc_pong(uint8_t vesc_id, uint8_t mine_id){

    can_hal_msg_t msg;

    uint8_t buffer[1] = { mine_id };

    prepare_tx_can_msg(&msg, CAN_PACKET_PONG, vesc_id, sizeof(buffer),
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    memcpy(msg.data, buffer, sizeof(buffer));

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    
    return status;
}

lStatus_t vesc_conf_set_current_limits(uint8_t vesc_id, float min, float max){
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_CONF_CURRENT_LIMITS, vesc_id, 8,
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    int32_t index = 0;
    pack_float_to_msg_data(&msg.data[index], min, 1000.0f);

    index += 4;
    pack_float_to_msg_data(&msg.data[index], max, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_conf_set_current_limits_store(uint8_t vesc_id, float min, float max){
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_CONF_STORE_CURRENT_LIMITS, vesc_id, 8,
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    int32_t index = 0;
    pack_float_to_msg_data(&msg.data[index], min, 1000.0f);

    index += 4;
    pack_float_to_msg_data(&msg.data[index], max, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_conf_set_current_in_limits(uint8_t vesc_id, float min, float max){
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_CONF_CURRENT_LIMITS_IN, vesc_id, 8,
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    int32_t index = 0;
    pack_float_to_msg_data(&msg.data[index], min, 1000.0f);

    index += 4;
    pack_float_to_msg_data(&msg.data[index], max, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_conf_set_current_in_limits_store(uint8_t vesc_id, float min, float max){
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_CONF_STORE_CURRENT_LIMITS_IN, vesc_id, 8,
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    int32_t index = 0;
    pack_float_to_msg_data(&msg.data[index], min, 1000.0f);
    
    index += 4;
    pack_float_to_msg_data(&msg.data[index], max, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_conf_foc_erpm(uint8_t vesc_id, float foc_openloop_rpm, float foc_sl_erpm){
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_CONF_FOC_ERPM, vesc_id, 8,
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    int32_t index = 0;
    pack_float_to_msg_data(&msg.data[index], foc_openloop_rpm, 1000.0f);

    index += 4;
    pack_float_to_msg_data(&msg.data[index],foc_sl_erpm, 1000.0f);
    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_conf_foc_erpm_store(uint8_t vesc_id, float foc_openloop_rpm, float foc_sl_erpm){
    can_hal_msg_t msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_CONF_STORE_FOC_ERPM, vesc_id, 8,
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    int32_t index = 0;
    pack_float_to_msg_data(&msg.data[index], foc_openloop_rpm, 1000.0f);

    index += 4;
    pack_float_to_msg_data(&msg.data[index],foc_sl_erpm, 1000.0f);
    
    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    return status;
}

lStatus_t vesc_detect_apply_all_foc(uint8_t vesc_id, bool active_status_msgs, float max_power_loss){
    can_hal_msg_t msg;
    can_hal_msg_t response_msg;

    prepare_tx_can_msg(&msg, CAN_PACKET_DETECT_APPLY_ALL_FOC, vesc_id, 6,
                       CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

    msg.data[0]= MINE_CAN_ID;
    msg.data[1]= active_status_msgs;

    int32_t index = 2;
    pack_float_to_msg_data(&msg.data[index], max_power_loss, 1000.0f);

    lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
    if (status!=lStatus_Success){
        return status;
    }
        
    uint32_t start_time = HAL_GetTimeUS();
    if (!active_status_msgs)
    {
        return lStatus_Success;
    }
    while ((HAL_GetTimeUS() - start_time) < 30000) {
        status = HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &response_msg, 10);
        if (status == lStatus_Success) {
            if (response_msg.idFormat == CAN_HAL_ID_FORMAT_EXT && 
                response_msg.messageId == (((uint32_t)0 << 18) | ((uint32_t)CAN_PACKET_DETECT_APPLY_ALL_FOC_RES<< 8) | MINE_CAN_ID)) 
            {
                if(response_msg.data[0]!=0)
                {
                    return lStatus_Fail;
                }

                return lStatus_Success;
            }
        }
    }

    return lStatus_Timeout;

}

lStatus_t vesc_send_buffer(uint8_t vesc_id, uint8_t *data, uint16_t len, uint8_t send){
    can_hal_msg_t msg;
    uint8_t send_buffer[8];

    if(data == NULL || len == 0){
        return lStatus_Fail;
    }

    if(len <= 6){
        uint8_t index = 0;
        send_buffer[index++] = MINE_CAN_ID;
        send_buffer[index++] = send;
        memcpy(send_buffer + index, data, len);
        index += len;
        
        prepare_tx_can_msg(&msg, CAN_PACKET_PROCESS_SHORT_BUFFER, vesc_id,
                            index, CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);
        
        memcpy(msg.data, send_buffer, index);

        return HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);

    }
    else{
        uint8_t send_len = 7;
        uint16_t end_of_block = 0;

		for (uint16_t i = 0;i < len;i += 7) {

			end_of_block = i + 7;
            send_len = 7;
			send_buffer[0] = i;

			if ((i + 7) <= len) {
				memcpy(send_buffer + 1, data + i, send_len);
			} else {
				send_len = len - i;
				memcpy(send_buffer + 1, data + i, send_len);
			}

            prepare_tx_can_msg(&msg, CAN_PACKET_FILL_RX_BUFFER, vesc_id,
                                send_len+ 1, CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);
            
            memcpy(msg.data, send_buffer, send_len + 1);

            lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
            if(status != lStatus_Success) {
                return status;
            }
		}

        for (unsigned int i = end_of_block;i < len;i += 6) {
			uint8_t send_len = 6;
			send_buffer[0] = i >> 8;
			send_buffer[1] = i & 0xFF;

			if ((i + 6) <= len) {
				memcpy(send_buffer + 2, data + i, send_len);
			} else {
				send_len = len - i;
				memcpy(send_buffer + 2, data + i, send_len);
			}

            prepare_tx_can_msg(&msg, CAN_PACKET_FILL_RX_BUFFER_LONG , vesc_id,
                                send_len+ 2, CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

            memcpy(msg.data, send_buffer, send_len + 2);

            lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
            if(status != lStatus_Success) {
                return status;
            }
		}

        uint32_t index = 0;
		send_buffer[index++] = MINE_CAN_ID;
		send_buffer[index++] = send;
		send_buffer[index++] = len >> 8;
		send_buffer[index++] = len & 0xFF;
		unsigned short crc = crc16_for_serial(data, len);
		send_buffer[index++] = (uint8_t)(crc >> 8);
		send_buffer[index++] = (uint8_t)(crc & 0xFF);

        prepare_tx_can_msg(&msg, CAN_PACKET_PROCESS_RX_BUFFER  , vesc_id,
                                index++, CAN_HAL_ID_FORMAT_EXT, CAN_HAL_DATA_FRAME);

        memcpy(msg.data, send_buffer, index);

        lStatus_t status = HAL_CAN_SendMessage(VESC_CAN_HAL_INSTANCE, &msg, 500000);
        return lStatus_Success;

    }
}

