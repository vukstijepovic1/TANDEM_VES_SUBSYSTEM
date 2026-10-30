/**
 * @file    task_bridge.c
 * @brief   Library used as template.
 * @version 0.0.0
 * @date    12.12.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "task_bridge.h"
#include "task_work.h"
#include "task_mav.h"
#include "types.h"

/* HAL Drivers */
#include "timer.h"
#include "vesc.h"
#include "vesc_helper.h"

/* Middleware */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "types.h"

/*FreeRTOS object*/ 

// Queueset 
extern QueueSetHandle_t queueset_mav;

// Queues
extern QueueHandle_t queue_command;
extern QueueHandle_t queue_response;
extern QueueHandle_t queue_status_1;
extern QueueHandle_t queue_status_2;
extern QueueHandle_t queue_status_3;
extern QueueHandle_t queue_status_4;
extern QueueHandle_t queue_status_5;
extern QueueHandle_t queue_status_6;

// Handlers
extern TaskHandle_t xTaskBridgeHandle;

/*Temporary varaibles*/
int len = 0;
uint8_t uart_buf[7];

// Static Messages
static messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 msg_1;
static messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 msg_2;
static messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 msg_3;
static messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 msg_4;
static messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 msg_5;
static messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 msg_6;
static messages_cyphal_uavcan_lisum_LisumVescPing_1_0 pong_resp;

#if VESC_CONNECTION_TYPE

// VESC Status Variables
static vesc_parser_t vesc_parser;
static vesc_values_t current_values;

static bool parser_initialized = false;
static bool vesc_uart_comm_initialized = false;

// UART RX Interupt Variables
extern uart_hal_handle_t vesc_uart_handle;
static uint8_t uart_rx_buff[128];
static volatile uint8_t uart_rx_index = 0;

#else

static lStatus_t status;

float duty      = 0;
float currtot   = 0;
float erpm      = 0;
float ah        = 0;
float ahc       = 0;
float wh        = 0;
float whc       = 0;
float currin    = 0;
float tempfet   = 0;
float tempmotor = 0;
float pos       = 0;
float tacho     = 0;
float vin       = 0;
float adc1      = 0;
float adc2      = 0;
float adc3      = 0;
float ppm       = 0;

uint8_t response_id = 0;
vesc_command_message_t response_msg;

#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void timer_1khz_cb(tim_hal_ch_t ch, tim_hal_int_type_t type);

static inline lState_t ProcessCommand(vesc_command_message_t *cmd);

static inline lState_t ReSendResponse(vesc_command_message_t *cmd);

#if VESC_CONNECTION_TYPE

static void vesc_uart_comm_init(void);

static lStatus_t uart_status_bridge_repack (
                uint8_t vesc_id,
                messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 *msg_1,
                messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 *msg_2,
                messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 *msg_3,
                messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 *msg_4,
                messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 *msg_5,
                messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 *msg_6,
                vesc_values_t *values);

static void vesc_uart_callback(uart_hal_instance_t instance, 
                uart_hal_handle_t *handle, 
                lStatus_t status, void *userData);

#else 
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/
void task_bridge(void *arg)
{
#if VESC_CONNECTION_TYPE

    vesc_uart_comm_init();
    len = vesc_build_get_values(uart_buf);

#else
#endif

    vesc_command_message_t cmd;
    uint32_t ulNotificationBits;

    memset(&msg_1, 0, sizeof(msg_1));
    memset(&msg_2, 0, sizeof(msg_2));
    memset(&msg_3, 0, sizeof(msg_3));
    memset(&msg_4, 0, sizeof(msg_4));
    memset(&msg_5, 0, sizeof(msg_5));
    memset(&msg_6, 0, sizeof(msg_6));

    /* TimerInit */
    tim_hal_cfg_t tim_cfg = {
        .Prescale = TIM_HAL_PRESCALE_US,
        .PrescaleValue = 1
    };

    HAL_TIM_Init(TIM_HAL_INSTANCE_0, &tim_cfg);

    tim_hal_match_cfg_t match_cfg = {
        .MatchChannel = TIM_HAL_CH_0,
        .MatchValue = 10000,    
        .ResetOnMatch = lFunctionalState_Enable,
        .StopOnMatch = lFunctionalState_Disable
    };  

    HAL_TIM_MatchCfg(TIM_HAL_INSTANCE_0, &match_cfg);
    HAL_TIM_EnableInterrupt(TIM_HAL_INSTANCE_0, timer_1khz_cb);

    for (;;){
        if(xQueueReceive(queue_command, &cmd, 0)==pdPASS){

            status = ProcessCommand(&cmd);
            if(status == lStatus_Success){
                ReSendResponse(&cmd);
            }

        }
        ulNotificationBits = ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

        if(ulNotificationBits & NOTIFY_TIMER_BIT){

           // vesc_set_duty(124, 0.3);

#if VESC_CONNECTION_TYPE 
#else          
            request_vesc_status_1(58, &duty, &currtot, &erpm);

            msg_1.duty           = duty; 
            msg_1._total_current = currtot;
            msg_1.erpm           = erpm;

            if(xQueueSend(queue_status_1, &msg_1, 0) != pdPASS){
                
            }

            request_vesc_status_2(58, &ahc, &ah);

            msg_2.amp_hours_charged = ahc;
            msg_2.amp_hours         = ah;

            if(xQueueSend(queue_status_2, &msg_2, 0) != pdPASS){
                
            }

            request_vesc_status_3(58, &whc, &wh);

            msg_3.watt_hours_charged = whc;
            msg_3.watt_hours         = wh;

            if(xQueueSend(queue_status_3, &msg_3, 0) != pdPASS){
                
            }    
 
            request_vesc_status_4(58, &pos, &currin, &tempmotor, &tempfet);

            msg_4.pos        = pos;
            msg_4.current_in = currin;
            msg_4.motor_temp = tempmotor;
            msg_4.fet_temp   = tempfet;

            if(xQueueSend(queue_status_4, &msg_4, 0) != pdPASS){
                
            }

            request_vesc_status_5(58, &vin, &tacho);

            msg_5.voltage    = vin;
            msg_5.tachometar = tacho;

            if(xQueueSend(queue_status_5, &msg_5, 0) != pdPASS){
                
            }

            request_vesc_status_6(58, &adc1, &adc2, &adc3, &ppm);

            msg_6.adc1 = adc1;
            msg_6.adc2 = adc2;
            msg_6.adc3 = adc3;
            msg_6.ppm  = ppm;

            if(xQueueSend(queue_status_6, &msg_6, 0) != pdPASS){
                
            }
#endif

        }
        if(ulNotificationBits & NOTIFY_VESC_STATUS_BIT){

#if VESC_CONNECTION_TYPE 
        status = 
            uart_status_bridge_repack(cmd.vesc_id, &msg_1, &msg_2, &msg_3,      
                                     &msg_4, &msg_5, &msg_6, &current_values);


            if(xQueueSend(queue_status_1, &msg_1, 0) != pdPASS){
                
            }
            if(xQueueSend(queue_status_2, &msg_2, 0) != pdPASS){
                
            }
            if(xQueueSend(queue_status_3, &msg_3, 0) != pdPASS){
                
            }
            if(xQueueSend(queue_status_4, &msg_4, 0) != pdPASS){
                
            }
            if(xQueueSend(queue_status_5, &msg_5, 0) != pdPASS){
                
            }
            if(xQueueSend(queue_status_6, &msg_6, 0) != pdPASS){
                
            }
#endif

        }
        taskYIELD();
    }

}

/****************************** static functions ******************************/



__attribute__((optimize("O3")))
void timer_1khz_cb(tim_hal_ch_t ch, tim_hal_int_type_t type)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (type == TIM_HAL_INT_TYPE_MATCH && ch == TIM_HAL_CH_0) {
        if(xTaskBridgeHandle != NULL){
#if VESC_CONNECTION_TYPE
            if (HAL_UART_Send(UART_HAL_INSTANCE_1, uart_buf, len, 10) != lStatus_Success) {
                 return;
             } 
#else
            xTaskNotifyFromISR(xTaskBridgeHandle, 
                               NOTIFY_TIMER_BIT,
                               eSetBits,
                               &xHigherPriorityTaskWoken);
#endif
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

#if VESC_CONNECTION_TYPE
static void vesc_uart_callback(uart_hal_instance_t instance,
                               uart_hal_handle_t *handle,
                               lStatus_t status,
                               void *userData) 
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (instance != UART_HAL_INSTANCE_1) {
        return;
    }
    
    if (status == lStatus_Success && handle->rxState == 0) {
        
        for (uint32_t i = 0; i < handle->rxDataSize; i++) {
            uint8_t rx_byte = handle->rxData[i];
            
            if (vesc_parser_process_get_values(&vesc_parser, rx_byte, &current_values)) {
         
                if (xTaskBridgeHandle != NULL) {
                    xTaskNotifyFromISR(xTaskBridgeHandle,
                                      NOTIFY_VESC_STATUS_BIT,
                                      eSetBits,
                                      &xHigherPriorityTaskWoken);
                }
                
                break;
            }
        }
        
        HAL_UART_ReceiveISR(UART_HAL_INSTANCE_1,
                           handle,
                           uart_rx_buff,
                           sizeof(uart_rx_buff));
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


static void vesc_uart_comm_init(void) {

    if (vesc_uart_comm_initialized) {
        return; 
    }
    

    if (!parser_initialized) {
        vesc_parser_init(&vesc_parser);
        parser_initialized = true;

    }

    vesc_uart_handle.callback = vesc_uart_callback;
    
    HAL_UART_ReceiveISR(UART_HAL_INSTANCE_1,
                       &vesc_uart_handle,
                       uart_rx_buff,
                       sizeof(uart_rx_buff));
    
    vesc_uart_comm_initialized = true;

}
static lStatus_t uart_status_bridge_repack (
                uint8_t vesc_id,
                messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 *msg_1,
                messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 *msg_2,
                messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 *msg_3,
                messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 *msg_4,
                messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 *msg_5,
                messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 *msg_6,
                vesc_values_t *values){

    if (msg_1 == NULL || msg_2 == NULL || msg_3 == NULL || msg_4 == NULL || 
        msg_5 == NULL || msg_6 == NULL || values == NULL) {
        return lStatus_Fail;
    }

    *msg_1 = (messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0){    
        .duty               = values->duty_now * 1000,
        ._total_current     = values->current_motor * 10,
        .erpm               = values->rpm * 1, 
        .vesc_id            = vesc_id    
    };
       
    *msg_2 = (messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0){
        .amp_hours_charged  = values->amp_hours_charged * 10000,  
        .amp_hours          = values->amp_hours * 10000,    
        .vesc_id            = vesc_id                    
    };

    *msg_3 = (messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0){
        .watt_hours_charged = values->watt_hours_charged * 10000,  
        .watt_hours         = values->watt_hours * 10000,         
        .vesc_id            = vesc_id                   
    };

    *msg_4 = (messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0){
        .pos                = values->pid_pos * 50, 
        .current_in         = values->current_in * 10,
        .motor_temp         = values->temp_motor * 10,
        .fet_temp           = values->temp_mos * 10,     
        .vesc_id            = vesc_id                   
    };

    *msg_5 = (messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0){
        .voltage            = values->v_in * 10,   
        .tachometar         = values->tachometer * 6,    
        .vesc_id            = vesc_id                   
    };

    *msg_6 = (messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0){
        .ppm                = 0,
        .adc1               = 0,
        .adc2               = 0,
        .adc3               = 0,      
        .vesc_id            = vesc_id                    
    };

    return lStatus_Success;    
}
#endif

__attribute__((always_inline))
static inline lState_t ProcessCommand(vesc_command_message_t *cmd){
    switch(cmd->commmand_ID){
        case CAN_PACKET_SET_DUTY:
            status = vesc_set_duty(cmd->vesc_id, cmd->command_pharam);
            return status;
            
        case CAN_PACKET_SET_CURRENT:
            status = vesc_set_current(cmd->vesc_id, cmd->command_pharam);
            return status;
            
        case CAN_PACKET_SET_CURRENT_BRAKE:
            status = vesc_set_current_brake(cmd->vesc_id, cmd->command_pharam);
            return status;
            
        case CAN_PACKET_SET_RPM:
            status = vesc_set_rpm(cmd->vesc_id, cmd->command_pharam);
            return status;
            
        case CAN_PACKET_SET_POS:
            status = vesc_set_pos(cmd->vesc_id, cmd->command_pharam);
            return status;
            
        case CAN_PACKET_PING:
            status = vesc_ping(cmd->vesc_id, 0, &response_id);
            return status;
            
        default: return lStatus_Fail;
            
    }
}

__attribute__((always_inline))
static inline lState_t ReSendResponse(vesc_command_message_t *cmd){
    switch(cmd->commmand_ID){
        case CAN_PACKET_SET_DUTY:
            return lStatus_Success;

        case CAN_PACKET_SET_CURRENT:
            return lStatus_Success;

        case CAN_PACKET_SET_CURRENT_BRAKE:
            return lStatus_Success;

        case CAN_PACKET_SET_RPM:
            return lStatus_Success;

        case CAN_PACKET_SET_POS:
            return lStatus_Success;

        case CAN_PACKET_PING:
            response_msg.commmand_ID = CAN_PACKET_PONG;
            response_msg.vesc_id = 0;
            response_msg.command_pharam= response_id;
            if(xQueueSend(queue_response, &response_msg, 0) != pdPASS){
            };
            return lStatus_Success;

        default: return lStatus_Fail;     
    }  

}

/********************************* End Of File ********************************/