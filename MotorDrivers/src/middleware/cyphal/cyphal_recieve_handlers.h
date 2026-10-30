/**
 * @file    cyphal_utility.h
 * @brief   Cyphal/libcanard utility API: deterministic memory, TX/RX helpers
 * @version 1.0.0
 * @date    20.11.2025
 *
 */

#ifndef CYPHAL_RECEIVE_HANDLERS_H
#define CYPHAL_RECEIVE_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "canard.h"
#include <stddef.h>
#include <stdint.h>


// #include "FreeRTOS.h"
// #include "queue.h"
// #include "semphr.h"
// #include "task.h"
// #include "timers.h"



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

//void handle_vesc_comm(const struct CanardRxTransfer* tr, QueueHandle_t queue);

#ifdef __cplusplus
}
#endif

#endif /* CYPHAL_RECEIVE_HANDLERS_H */