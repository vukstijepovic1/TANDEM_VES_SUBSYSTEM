/**
 * @file    types.h
 * @brief   Project common defines and types.
 * @version 1.0.0
 * @date    19.03.2025
 * @author  LisumLab
 */

#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* blinky period [ms] */
#define BLINKY_PERIOD_MS 500


/* ------------------------ Mission planner UART ---------------------------- */

/* MISSION_PLANNER UART instance */
#define MISSION_PLANNER_UART_INSTANCE UART_HAL_INSTANCE_1
/* MISSION_PLANNER UART baud rate */
#define MISSION_PLANNER_UART_BAUD_RATE 57600
/* MISSION_PLANNER UART receive queue length in bytes */
#define MISSION_PLANNER_RX_QUEUE_LEN 1024
/* MISSION_PLANNER UART transmit queue length in bytes */
#define MISSION_PLANNER_TX_QUEUE_LEN 1024

/* --------------------------- Telemetry UART ------------------------------- */

/* Telemetry UART instance */
#define TELEMETRY_UART_INSTANCE UART_HAL_INSTANCE_2
/* Telemetry UART baud rate */
#define TELEMETRY_UART_BAUD_RATE 230400
/* Telemetry UART receive queue length in bytes */
#define TELEMETRY_RX_QUEUE_LEN 1024
/* Telemetry UART transmit queue length in bytes */
#define TELEMETRY_TX_QUEUE_LEN 1024

/* ------------------------- Ardupilot Versrion ----------------------------- */

/* DATA FOR ARDUPILOT VERSION MESSAGE */
#define PAYLOAD_LENGTH 8

/* Read from src/middleware/FreeRTOS/manifest.yml */
#define OS_CUSTOM_VERSION "v10.5.1"            

/* BlackBox Board Version */ 
#define BOARD_TYPE 6          // Read ethernet port marked by marker
#define BOARD_REVISION 2       // This is Hardware Version V0.2

/* Vendor and product IDs*/
#define VENDOR_ID  0x4254  // 'B' 'T' Inicialize of company
#define PRODUCT_ID 6      // Number of model

#define VENDOR_NAME        "BetaTehPro"
#define MODEL_NAME         "HW_GW_GND"
#define SOFTWARE_VERSION   FLIGHT_CUSTOM_VERSION
#define HARDWARE_VERSION   "DEV-Board V0.2"
#define SERIAL_NUMBER      "S_N: 006"

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* TYPES_H */