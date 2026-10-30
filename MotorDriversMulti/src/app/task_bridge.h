/**
 * @file    task_bridge.h
 * @brief   Library used as template.
 * @version 0.0.0
 * @date    12.12.2025
 * @author  LisumLab
 */

#ifndef TASK_BRIDGE_H
#define TASK_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include "types.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define MAX_VESC_COUNT 8

/*******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef struct{
    /* --- Interface --- */
    uint8_t vesc_id;
    bool active;

    /* --- Status values populated by request_vesc_status_X() --- */
    float duty;
    float currtot;
    float erpm;
    float ah;
    float ahc;
    float wh;
    float whc;
    float currin;
    float tempfet;
    float tempmotor;
    float pos;
    float tacho;
    float vin;
    float adc1;
    float adc2;
    float adc3;
    float ppm;

    /* --- Outgoing CAN message structs --- */
    messages_cyphal_uavcan_lisum_LisumVescStatus1_1_0 msg_1;
    messages_cyphal_uavcan_lisum_LisumVescStatus2_1_0 msg_2;
    messages_cyphal_uavcan_lisum_LisumVescStatus3_1_0 msg_3;
    messages_cyphal_uavcan_lisum_LisumVescStatus4_1_0 msg_4;
    messages_cyphal_uavcan_lisum_LisumVescStatus5_1_0 msg_5;
    messages_cyphal_uavcan_lisum_LisumVescStatus6_1_0 msg_6;

    /* --- Per-instance queue handles --- */
    QueueHandle_t queue_status_1;
    QueueHandle_t queue_status_2;
    QueueHandle_t queue_status_3;
    QueueHandle_t queue_status_4;
    QueueHandle_t queue_status_5;
    QueueHandle_t queue_status_6;

    /* --- QueueSet handle --- */
    QueueSetHandle_t queueset;

}vesc_instance_t;

extern vesc_instance_t vesc_fleet[MAX_VESC_COUNT];

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

void init_vesc_fleet(void);

void task_bridge(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* TASK_BRIDGE_H */