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

#include "cyphal_utility.h"
#include "lfs.h"
#include "main.h"
#include "mav.h"
#include "mavlink/messages_cyphal_uavcan/common/AdsbVehicle_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/AisVessel_1_0.h"
#include "mavlink/messages_cyphal_uavcan/common/NamedValueFloat_1_0.h"
#include "types.h"
#include "uavcan/uavcan/node/GetInfo_1_0.h"
#include "uavcan/uavcan/node/Heartbeat_1_0.h"
#include <stdint.h>

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
static inline uint32_t parse_version_string(const char* version_str)
{
	int major = 0, minor = 0, patch = 0;

	if (version_str[0] == 'v' || version_str[0] == 'V') {
		version_str++; // preskoči 'v'
	}

	sscanf(version_str, "%d.%d.%d", &major, &minor, &patch);

	return (((uint32_t)'v' & 0xFF) << 24) | ((major & 0xFF) << 16) |
		   ((minor & 0xFF) << 8) | ((patch & 0xFF) << 0);
}

static inline mavlink_component_information_basic_t make_comp_info_basic(void)
{
	mavlink_component_information_basic_t data;
	/* Initialized structure */
	memset(&data, 0, sizeof(data));

	strncpy(data.vendor_name, vendor_name, sizeof(data.vendor_name));

	strncpy(data.model_name, model_name, sizeof(data.model_name));

	strncpy(data.software_version, software_version,
			sizeof(data.software_version));

	strncpy(data.hardware_version, hardware_version,
			sizeof(data.hardware_version));

	strncpy(data.serial_number, serial_number, sizeof(data.serial_number));

	data.capabilities =
		MAV_PROTOCOL_CAPABILITY_MAVLINK2 | MAV_PROTOCOL_CAPABILITY_FTP;

	data.time_manufacture_s = BUILD_UNIX_TIMESTAMP;

	/* Conversion to ms */
	data.time_boot_ms = boot_time_ms;

	return data;
}

static inline mavlink_autopilot_version_t make_autopilot_version(void)
{
	mavlink_autopilot_version_t data = {0};

	// Capabillities is functionality of hardware (Mavlink version, FTP
	// protocol...)
	data.capabilities =
		MAV_PROTOCOL_CAPABILITY_MAVLINK2 | MAV_PROTOCOL_CAPABILITY_COMMAND_INT;

	// TODO UID
	data.uid = 0x0102030405060708ULL;

	// Board type higher bits are product ID and board revision is hardware
	// version like V0.2
	data.board_version = (BOARD_TYPE << 16) | BOARD_REVISION;

	// Vendor ID is inicialy of our company
	data.vendor_id = VENDOR_ID;

	// Product ID is marked on ETHERNET PORT
	data.product_id = PRODUCT_ID;

	// Flight version is commit tag
	data.flight_sw_version = parse_version_string(FLIGHT_CUSTOM_VERSION);

	// OS version is freeRTOS version
	data.os_sw_version = parse_version_string(OS_CUSTOM_VERSION);

	// Middleware version is LFS version
	data.middleware_sw_version = ((uint32_t)'v' << 24) |
								 ((LFS_VERSION_MAJOR & 0xFF) << 16) |
								 ((LFS_VERSION_MINOR & 0xFF) << 8) | (0 & 0xFF);

	// Custom versions is commit hash
	for (int i = 0; i < PAYLOAD_LENGTH; i++) {
		data.flight_custom_version[i] = FLIGHT_CUSTOM_HASH[i];
	}
	for (int i = 0; i < PAYLOAD_LENGTH; i++) {
		data.middleware_custom_version[i] = MIDDLEWARE_CUSTOM_HASH[i];
	}
	for (int i = 0; i < PAYLOAD_LENGTH; i++) {
		data.os_custom_version[i] = OS_CUSTOM_HASH[i];
	}

	// TODO UID2
	memset(data.uid2, 0, sizeof(data.uid2));
	strncpy((char*)data.uid2, serial_number, sizeof(data.uid2));

	return data;
}

/* Helper: convert small unsigned (0-255) to ASCII decimal into dest (no null
 * terminator added automatically). */
static inline size_t u8_to_dec(uint8_t v, char* dest, size_t max)
{
	char tmp[3]; /* 255 max */
	uint8_t hundreds = v / 100;
	uint8_t tens = (v / 10) % 10;
	uint8_t ones = v % 10;
	size_t idx = 0;
	if (hundreds)
		tmp[idx++] = (char)('0' + hundreds);
	if (hundreds || tens)
		tmp[idx++] = (char)('0' + tens);
	tmp[idx++] = (char)('0' + ones);
	if (idx > max)
		idx = max; /* truncate if needed */
	for (size_t i = 0; i < idx; i++)
		dest[i] = tmp[i];
	return idx;
}

/* Fill MAVLink component_information_basic fields from Cyphal GetInfo response
 */
static inline void fill_mavlink_component_info_from_cyphal_getinfo(
	const uavcan_node_GetInfo_Response_1_0* resp,
	mavlink_component_information_basic_t* out)
{
	if (!resp || !out)
		return;
	memset(out->hardware_version, 0, sizeof(out->hardware_version));
	memset(out->software_version, 0, sizeof(out->software_version));
	memset(out->serial_number, 0, sizeof(out->serial_number));
	memset(out->model_name, 0, sizeof(out->model_name));

	/* Hardware version string: DEV-Board V<maj><min> */
	const char prefix_hw[] = "DEV-Board V"; /* 11 chars */
	size_t pos = 0;
	for (size_t i = 0;
		 i < sizeof(prefix_hw) - 1 && pos < sizeof(out->hardware_version);
		 i++) {
		out->hardware_version[pos++] = prefix_hw[i];
	}
	if (pos < sizeof(out->hardware_version))
		pos += u8_to_dec(resp->hardware_version.major,
						 (char*)&out->hardware_version[pos],
						 sizeof(out->hardware_version) - pos);
	if (pos < sizeof(out->hardware_version))
		out->hardware_version[pos++] = '.';
	if (pos < sizeof(out->hardware_version))
		pos += u8_to_dec(resp->hardware_version.minor,
						 (char*)&out->hardware_version[pos],
						 sizeof(out->hardware_version) - pos);

	/* Software version string: v<maj>.<min>.0 */
	size_t spos = 0;
	if (spos < sizeof(out->software_version))
		out->software_version[spos++] = 'v';
	if (spos < sizeof(out->software_version))
		spos += u8_to_dec(resp->software_version.major,
						  (char*)&out->software_version[spos],
						  sizeof(out->software_version) - spos);
	if (spos < sizeof(out->software_version))
		out->software_version[spos++] = '.';
	if (spos < sizeof(out->software_version))
		spos += u8_to_dec(resp->software_version.minor,
						  (char*)&out->software_version[spos],
						  sizeof(out->software_version) - spos);
	if (spos + 2 <= sizeof(out->software_version)) {
		out->software_version[spos++] = '.';
		out->software_version[spos++] = '0';
	}

	/* Serial number: S_N: <two-digit from unique_id[0]> */
	const char sn_prefix[] = "S_N: ";
	size_t snp = 0;
	for (size_t i = 0;
		 i < sizeof(sn_prefix) - 1 && snp < sizeof(out->serial_number); i++) {
		out->serial_number[snp++] = sn_prefix[i];
	}
	if (snp < sizeof(out->serial_number)) {
		uint8_t val = resp->unique_id[0];
		out->serial_number[snp++] = (char)('0' + ((val / 10) % 10));
		if (snp < sizeof(out->serial_number))
			out->serial_number[snp++] = (char)('0' + (val % 10));
	}

	/* Model name selection: example mapping using first unique_id byte */
	const char* model = "UNKNOWN"; /* default */
	switch (resp->unique_id[0]) {
	case CYPHAL_BLACK_BOX_ID:
		model = "HW_BLACK_BOX";
		break;
	case CYPHAL_INS_ID:
		model = "HW_GW_INS";
		break;

	default:
		break;
	}
	size_t mlen = strlen(model);
	if (mlen > sizeof(out->model_name))
		mlen = sizeof(out->model_name);
	memcpy(out->model_name, model, mlen);
}

void task_work(void* arg);

#ifdef __cplusplus
}
#endif

#endif /* TASK_WORK_H */