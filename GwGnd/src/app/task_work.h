/**
 * @file    task_work.h
 * @brief   Task work - process background tasks
 * @version 1.0.0
 * @date    09.04.2025
 * @author  LisumLab
 */

#ifndef TASK_WORK_H
#define TASK_WORK_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include "types.h"
#include "main.h"
#include "mav.h"
#include "lfs.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern const char vendor_name[];
extern const char model_name[];
extern const char software_version[];
extern const char hardware_version[];
extern const char serial_number[];

/* Extern temporary variables */
extern uint64_t boot_time_ms;

/*******************************************************************************
 * API
 ******************************************************************************/

// Inline parsing function for extracting string in format "vX.Y.Z"
static inline uint32_t parse_version_string(const char* version_str) {
    int major = 0, minor = 0, patch = 0;

    if (version_str[0] == 'v' || version_str[0] == 'V') {
        version_str++; // preskoči 'v'
    }

    sscanf(version_str, "%d.%d.%d", &major, &minor, &patch);

    return (((uint32_t)'v' & 0xFF) << 24)|
                   ((major & 0xFF) << 16)|
                   ((minor & 0xFF) << 8 )|
                   ((patch & 0xFF) << 0 );
    
}

static inline mavlink_component_information_basic_t make_comp_info_basic(void)
{
    mavlink_component_information_basic_t data;
    memset(&data, 0, sizeof(data)); 

    strncpy(data.vendor_name, vendor_name, sizeof(data.vendor_name));
    strncpy(data.model_name, model_name, sizeof(data.model_name));
    strncpy(data.software_version, software_version, sizeof(data.software_version));
    strncpy(data.hardware_version, hardware_version, sizeof(data.hardware_version));
    strncpy(data.serial_number, serial_number, sizeof(data.serial_number));

    data.capabilities = MAV_PROTOCOL_CAPABILITY_MAVLINK2 | MAV_PROTOCOL_CAPABILITY_FTP;
    data.time_manufacture_s = BUILD_UNIX_TIMESTAMP;
    data.time_boot_ms = boot_time_ms;

    return data;
}

static inline mavlink_autopilot_version_t make_autopilot_version(void)
{
    mavlink_autopilot_version_t data;
    memset(&data, 0, sizeof(data));

    data.capabilities = MAV_PROTOCOL_CAPABILITY_MAVLINK2 | MAV_PROTOCOL_CAPABILITY_FTP;
    data.uid = 0x0102030405060708ULL;
    data.board_version = (BOARD_TYPE << 16) | BOARD_REVISION;
    data.vendor_id  = VENDOR_ID;
    data.product_id = PRODUCT_ID;
    data.flight_sw_version = parse_version_string(FLIGHT_CUSTOM_VERSION);
    data.os_sw_version = parse_version_string(OS_CUSTOM_VERSION);
    data.middleware_sw_version = (((uint32_t)'v' & 0xFF) << 24)|
                                 ((LFS_VERSION_MAJOR & 0xFF) << 16)|
                                 ((LFS_VERSION_MINOR & 0xFF) << 8 )|
                                 (0 & 0xFF);

    for(int i = 0; i < PAYLOAD_LENGTH; i++){
        data.flight_custom_version[i] = FLIGHT_CUSTOM_HASH[i];
        data.middleware_custom_version[i] = MIDDLEWARE_CUSTOM_HASH[i];
        data.os_custom_version[i] = OS_CUSTOM_HASH[i];
    }

    memset(data.uid2, 0, sizeof(data.uid2));
    strncpy((char*)data.uid2, serial_number, sizeof(data.uid2));

    return data;
}

void task_work(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* TASK_WORK_H */