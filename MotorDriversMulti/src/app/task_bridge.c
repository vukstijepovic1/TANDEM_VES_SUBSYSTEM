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
#include "loadcell6.h"

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

extern QueueHandle_t queue_vesc_rx; 

// Handlers
extern TaskHandle_t xTaskBridgeHandle;


static uint8_t  debug_avg_ready_flag = 0;
static int32_t  debug_avg_result = 0;
static uint32_t debug_sample_count = 0;
static int64_t  debug_sample_sum = 0;

#define DEBUG_NUM_SAMPLES 1024

/* Temporary varaibles */
int len = 0;
uint8_t uart_buf[7];
static uint32_t vesc_rx_lost_counter = 0;

/* Load cell context */
static loadcell6_t loadcell_ctx;


#define LOADCELL_OFFSET   (-1017)         
#define LOADCELL_K     (0.0018089f)  


/* Instance every device */
vesc_instance_t vesc_fleet[MAX_VESC_COUNT]={
    { .vesc_id = 58,  .active = true },
    { .vesc_id = 59,  .active = false },
    { .vesc_id = 60,  .active = false },
    { .vesc_id = 61,  .active = false },    
    { .vesc_id = 124, .active = true },
    { .vesc_id = 125, .active = false },
    { .vesc_id = 126, .active = false },
    { .vesc_id = 127, .active = false },
    
};

// Static Messages
static messages_cyphal_uavcan_lisum_LisumVescPing_1_0 pong_resp;

static lStatus_t status;

uint8_t response_id = 0;

vesc_command_message_t response_msg;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void timer_1khz_cb(tim_hal_ch_t ch, tim_hal_int_type_t type);

static inline lState_t ProcessCommand(vesc_command_message_t *cmd);

static inline lState_t ReSendResponse(vesc_command_message_t *cmd);

static vesc_instance_t *find_vesc(uint8_t id);

static void poll_vesc_status(vesc_instance_t *v);

static void loadcell_init_with_pins(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
void task_bridge(void *arg)
{

    vesc_command_message_t cmd;
    can_hal_msg_t rx;
    
    loadcell_init_with_pins();

    for (;;){
        if(xQueueReceive(queue_command, &cmd, 0)==pdPASS){

            status = ProcessCommand(&cmd);
            if(status == lStatus_Success){
                ReSendResponse(&cmd);
            }
        }

        if (xQueueReceive(queue_vesc_rx, &rx, 0)==pdPASS) {
            uint8_t command = (rx.messageId >> 8) & 0xFF;
            uint8_t vesc_id = rx.messageId & 0xFF;

            vesc_instance_t *v = find_vesc(vesc_id);
            if (!v) continue;

            switch (command)
            {
                case CAN_PACKET_STATUS:
                    parse_vesc_status_1_from_can(&rx, &v->msg_1);
                    v->msg_1.vesc_id = vesc_id;
                    xQueueSend(v->queue_status_1, &v->msg_1, 0);
                    break;

                case CAN_PACKET_STATUS_2:
                    parse_vesc_status_2_from_can(&rx, &v->msg_2);
                    v->msg_2.vesc_id = vesc_id;
                    xQueueSend(v->queue_status_2, &v->msg_2, 0);
                    break;

                case CAN_PACKET_STATUS_3:
                    parse_vesc_status_3_from_can(&rx, &v->msg_3);
                    v->msg_3.vesc_id = vesc_id;
                    xQueueSend(v->queue_status_3, &v->msg_3, 0);
                    break;

                case CAN_PACKET_STATUS_4:
                    parse_vesc_status_4_from_can(&rx, &v->msg_4);
                    v->msg_4.vesc_id = vesc_id;
                    xQueueSend(v->queue_status_4, &v->msg_4, 0);
                    break;

                case CAN_PACKET_STATUS_5:
                    parse_vesc_status_5_from_can(&rx, &v->msg_5);
                    v->msg_5.vesc_id = vesc_id;
                    xQueueSend(v->queue_status_5, &v->msg_5, 0);
                    break;
                case CAN_PACKET_STATUS_6: {
                    parse_vesc_status_6_from_can(&rx, &v->msg_6);
                    v->msg_6.vesc_id = vesc_id;

                    uint32_t raw_adc = 0;
                    if (loadcell6_read_adc(&loadcell_ctx, &raw_adc) == LOADCELL6_OK) {

                        int32_t signed_adc = (int32_t)(raw_adc << 8) >> 8;

                        float M = LOADCELL_K * (float)(signed_adc - (int32_t)LOADCELL_OFFSET);

                        int16_t M_scaled = (int16_t)(M * 2000.0f); 

                        v->msg_6.adc1 = (uint16_t)M_scaled; 
                    }

                    xQueueSend(v->queue_status_6, &v->msg_6, 0);
                    break;
                }

                // case CAN_PACKET_STATUS_6: {
                //     parse_vesc_status_6_from_can(&rx, &v->msg_6);
                //     v->msg_6.vesc_id = vesc_id;

                //     uint32_t raw_adc = 0;
                //     if (loadcell6_read_adc(&loadcell_ctx, &raw_adc) == LOADCELL6_OK) {

                //         int32_t signed_adc = (int32_t)(raw_adc << 8) >> 8;

                //         /* --- Debug usrednjavanje na 100 odbiraka (za kalibraciju offseta/k) --- */
                //         if (debug_sample_count < DEBUG_NUM_SAMPLES) {
                //             debug_sample_sum += signed_adc;
                //             debug_sample_count++;
                //         }

                //         if (debug_sample_count == DEBUG_NUM_SAMPLES && debug_avg_ready_flag == 0) {
                //             debug_avg_result = (int32_t)(debug_sample_sum / DEBUG_NUM_SAMPLES);
                //             debug_avg_ready_flag = 1;   // <-- OVDE stavi breakpoint
                //         }
                //         /* --- kraj debug bloka --- */

                //         float M = LOADCELL_K * (float)(signed_adc - (int32_t)LOADCELL_OFFSET);

                //         int16_t M_scaled = (int16_t)(M * 1000.0f); // mNm preciznost, staje u 16 bita

                //         v->msg_6.adc1 = (uint16_t)M_scaled; // ceo moment u adc1
                //     }

                //     xQueueSend(v->queue_status_6, &v->msg_6, 0);
                //     break;
                // }
                                

                default:
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        //taskYIELD();
    }

}

/****************************** static functions ******************************/

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

void init_vesc_fleet(void){
    for(int i = 0; i< MAX_VESC_COUNT; i++){

        vesc_instance_t *v= &vesc_fleet[i];

        if (!v->active) {
            continue;
        }

    /* Clear all message structs */
    memset(&v->msg_1, 0, sizeof(v->msg_1));
    memset(&v->msg_2, 0, sizeof(v->msg_2));
    memset(&v->msg_3, 0, sizeof(v->msg_3));
    memset(&v->msg_4, 0, sizeof(v->msg_4));
    memset(&v->msg_5, 0, sizeof(v->msg_5));
    memset(&v->msg_6, 0, sizeof(v->msg_6));

    /* Create a dedicated queue for each status message type */
    v->queue_status_1 = xQueueCreate(15, sizeof(v->msg_1));
    v->queue_status_2 = xQueueCreate(15, sizeof(v->msg_2));
    v->queue_status_3 = xQueueCreate(15, sizeof(v->msg_3));
    v->queue_status_4 = xQueueCreate(15, sizeof(v->msg_4));
    v->queue_status_5 = xQueueCreate(15, sizeof(v->msg_5));
    v->queue_status_6 = xQueueCreate(15, sizeof(v->msg_6));

    v->queueset = xQueueCreateSet(40);

    xQueueAddToSet(v->queue_status_1, v->queueset);
    xQueueAddToSet(v->queue_status_2, v->queueset);
    xQueueAddToSet(v->queue_status_3, v->queueset);
    xQueueAddToSet(v->queue_status_4, v->queueset);
    xQueueAddToSet(v->queue_status_5, v->queueset);
    xQueueAddToSet(v->queue_status_6, v->queueset);
    
    }
}

static vesc_instance_t *find_vesc(uint8_t id){
    for (int i = 0; i < MAX_VESC_COUNT; i++) {
        if (vesc_fleet[i].active && vesc_fleet[i].vesc_id == id) {
            return &vesc_fleet[i];
        }
    }
    return NULL;
}

void vesccan_cb(void* usr_arg, uint32_t int_status)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (int_status & CAN_HAL_INT_MASK_RI) {
        can_hal_msg_t rx;
        
        while (HAL_CAN_ReceiveMessage(VESC_CAN_HAL_INSTANCE, &rx, 0) == lStatus_Success) {

            if (xQueueSendFromISR(queue_vesc_rx, &rx,
                                 &xHigherPriorityTaskWoken) != pdPASS)
            {
                vesc_rx_lost_counter++;
            }
        }
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void loadcell_init_with_pins(void){

    // spi params
    loadcell_ctx.spi_instance = SPI_HAL_INSTANCE_0;
    loadcell_ctx.spi_clock    = 1000000;
    
    // rst port params
    loadcell_ctx.rst_port = GPIO_HAL_INSTANCE_0;
    loadcell_ctx.rst_pin  = 6;

    // rdy port params
    loadcell_ctx.rdy_port = GPIO_HAL_INSTANCE_0;
    loadcell_ctx.rdy_pin  = 5;

    // cs  port params
    loadcell_ctx.cs_port = GPIO_HAL_INSTANCE_0;
    loadcell_ctx.cs_pin  = 7;

    // syn port params
    loadcell_ctx.syn_port = GPIO_HAL_INSTANCE_0;
    loadcell_ctx.syn_pin  = 4;

    // mode and comms params
    loadcell_ctx.data_bits = SPI_HAL_DATABIT_8;
    loadcell_ctx.cpha      = SPI_HAL_CPHA_SECOND;
    loadcell_ctx.cpol      = SPI_HAL_CPOL_LO;
    loadcell_ctx.mode      = SPI_HAL_MASTER_MODE;

    if(loadcell6_init(&loadcell_ctx) != LOADCELL6_OK){
        return;
    }

    if(loadcell6_default_cfg(&loadcell_ctx) != LOADCELL6_OK){
        return;
    }
}

/********************************* End Of File ********************************/