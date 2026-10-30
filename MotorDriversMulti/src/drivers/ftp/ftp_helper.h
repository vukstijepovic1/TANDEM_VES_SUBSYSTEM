/**
 * @file    ftp_helper.h
 * @brief   Helper functions and utilities for MAVLink FTP protocol handling with LittleFS and SD card.
 *
 * This header provides essential helper functions for managing FTP sessions,
 * preparing and parsing MAVLink FTP protocol messages, file and directory operations,
 * and CRC calculations on files stored in the LittleFS filesystem, typically
 * used with SD cards in embedded systems.
 *
 * It includes utilities for:
 * - Saving and retransmitting FTP responses,
 * - Preparing FTP message payloads,
 * - Extracting file and directory paths from messages,
 * - Managing FTP sessions,
 * - Recursive file/directory removal,
 * - Computing CRC32 checksums for files.
 *
 * Designed to support robust FTP communication over MAVLink in resource-constrained environments.
 *
 * @version 1.0.0
 * @date    16.06.2025
 * @author  BetaTehPro
 */

#ifndef FTP_HELPER_H
#define FTP_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "ftp_common.h"

/* Drivers */
#include "sd.h"

/* Middlewares */
#include "lfs.h"
#include "mav.h"

#include "FreeRTOS.h"
#include "queue.h"


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
 * API
 ******************************************************************************/

/**
 * @brief Saves the last processed FTP response and its sequence number.
 *
 * This function extracts the sequence number from the provided MAVLink FTP
 * message and stores it along with a copy of the entire message. This saved
 * response can be reused later if a duplicate message with the same sequence
 * number is received, enabling retransmission of the last valid response.
 *
 * @param data Pointer to the MAVLink FTP message containing the response to save.
 * @param last_seq_number Pointer to the variable where the last sequence number will be stored.
 * @param last_response Pointer to the buffer where the last response message will be copied.
 */

void  ftp_save_last_response(mavlink_file_transfer_protocol_t *data, uint16_t *last_seq_number, mavlink_file_transfer_protocol_t *last_response);


/** 
* @brief This implement logic fot time out on blackbox side (only for op = burst read file)
*
* This function serves to resend the last packet containing the burst_complete = 1 flag, 
* so that the ground station can be certain that 
* the data transmission has been successfully received.
*
* @param queue_member It reads and copies the data in case a response from the drone has been received.
* @param mav_gw_sky_handle Handler to sending to GW Sky via Mavlink
* @param tx_msg Message to send
* @param time2try Resend value
* @param delay Delay beetween 2 messages [ms]
*/

void ftp_timeout(QueueSetHandle_t queue_member, mav_t *mav_gw_sky_handle, mavlink_message_t *tx_msg, uint8_t time2try, uint8_t delay);

/**
 * @brief Extracts a 32-bit unsigned integer from a 4-byte little-endian buffer.
 *
 * This function reads 4 bytes from the given byte array and reconstructs
 * a 32-bit unsigned integer assuming little-endian byte order.
 *
 * @param data Pointer to a buffer containing 4 bytes (least significant byte first).
 * @return Extracted 32-bit unsigned integer.
 */

static inline uint32_t ftp_extract_u32(const uint8_t *data){
    uint32_t extract;
    
    extract = ((uint32_t)data[0] << 0 )|
              ((uint32_t)data[1] << 8 )|
              ((uint32_t)data[2] << 16)|
              ((uint32_t)data[3] << 24);

    return extract;
}

/**
 * @brief Inserts a 32-bit unsigned integer into a 4-byte buffer in little-endian format.
 *
 * This function writes the provided 32-bit unsigned integer into a buffer as 4 bytes,
 * storing the least significant byte first.
 *
 * @param dest Pointer to a buffer where the value will be stored (must have at least 4 bytes).
 * @param value The 32-bit unsigned integer to be stored.
 */

static inline void ftp_insert_u32(uint8_t *dest, uint32_t value) {
    dest[0] = (uint8_t)(value >> 0);
    dest[1] = (uint8_t)(value >> 8);
    dest[2] = (uint8_t)(value >> 16);
    dest[3] = (uint8_t)(value >> 24);
}

/**
 * @brief Prepares the FTP protocol message payload by setting the main header fields.
 *
 * This function initializes the fields in the FTP message payload: opcode, request opcode,
 * data size, and the starting position of the data within the payload.
 *
 * @param data           Pointer to the MAVLink FTP protocol message to be prepared.
 * @param opcode         Opcode of the main FTP protocol operation.
 * @param request_opcode Opcode of the specific FTP request operation.
 * @param size           Size of the data in the payload.
 * @param data_start     Index within the payload where the data begins.
 * @param offset         Offset of the data in the payload.
 */

static inline void ftp_payload_prepare(mavlink_file_transfer_protocol_t *data,
                                       uint8_t opcode    , uint8_t request_opcode,
                                       uint8_t session_id, uint8_t size,
                                       uint8_t data_start, uint32_t offset)
{
    data->payload[SESSION]        = session_id;
    data->payload[OPCODE]         = opcode;
    data->payload[SIZE]           = size;
    data->payload[REQUEST_OPCODE] = request_opcode;

    data->payload[OFFSET + 0] = (uint8_t)((offset >>  0) & 0xFF); // LSB
    data->payload[OFFSET + 1] = (uint8_t)((offset >>  8) & 0xFF);
    data->payload[OFFSET + 2] = (uint8_t)((offset >> 16) & 0xFF);
    data->payload[OFFSET + 3] = (uint8_t)((offset >> 24) & 0xFF); // MSB

    if ((request_opcode != OP_READ_FILE        && 
         request_opcode != OP_LIST_DIRECTORY   && 
         request_opcode != OP_CALC_FILE_CRC32  &&
         request_opcode != OP_BURST_READ_FILE) || 
         (opcode == OP_RESPONSE_NAK))
    {
        data->payload[DATA_START] = data_start;
        /* Others zero */
        memset(&data->payload[DATA_START + 1], 0, 251 - (DATA_START + 1));
    }
}


/**
 * @brief Extracts the directory path string from an FTP protocol message payload.
 *
 * This function reads the length of the directory path from the payload,
 * then copies that many bytes from the payload into the provided directory path buffer,
 * ensuring the copied string is null-terminated and does not overflow the buffer.
 *
 * @param ftp_msg      Pointer to the MAVLink FTP protocol message.
 * @param dirpath      Buffer where the extracted directory path string will be stored.
 * @param dirpath_size Size of the directory path buffer.
 */

static inline void ftp_extract_dirpath(const mavlink_file_transfer_protocol_t *ftp_msg, char *dirpath, uint16_t dirpath_size) {
    
	/* Copy len of payload-a */
	const uint8_t *payload = ftp_msg->payload;

    /* Take the len of payload */
    size_t copy_len = payload[SIZE];
    if (copy_len >= dirpath_size) {
        copy_len = dirpath_size - 1;
    }

    /* Copy string from payload ftp */
    memcpy(dirpath, &payload[DATA_START], copy_len);
    dirpath[copy_len] = '\0';
}

/**
 * @brief Extracts the file path string from an FTP protocol message payload.
 *
 * This function reads the length of the filepath from the payload,
 * then copies that many bytes from the payload into the provided filepath buffer,
 * ensuring the copied string is null-terminated and does not overflow the buffer.
 *
 * @param ftp_msg       Pointer to the MAVLink FTP protocol message.
 * @param filepath      Buffer where the extracted filepath string will be stored.
 * @param filepath_size Size of the filepath buffer.
 */

static inline void ftp_extract_filepath(const mavlink_file_transfer_protocol_t *ftp_msg, char *filepath, uint16_t filepath_size){
	/* Copy len of payload-a */
	const uint8_t *payload = ftp_msg->payload;

    /* Take the len of payload */
    size_t copy_len = payload[SIZE];
    if (copy_len >= filepath_size) {
        copy_len = filepath_size - 1;
    }

    /* Copy string from payload ftp */
    memcpy(filepath, &payload[DATA_START], copy_len);
    filepath[copy_len] = '\0';
}

/**
 * @brief Checks if filepath is empty.
 *
 * It returns End of string if the file does not exists.
 *
 * @param filepath       The full file path string.
 */

static inline bool ftp_filepath_is_empty(const uint8_t *filepath) {
    return filepath[0] == '\0';
}

/**
 * @brief Extracts the directory path from a full file path.
 *
 * This function copies the input file path into the provided directory path buffer,
 * then truncates the string at the last slash '/' character to isolate the directory path.
 * If no slash is found, the directory path is set to an empty string.
 *
 * @param filepath       The full file path string.
 * @param filepath_size  The size of the filepath buffer.
 * @param dirpath       Buffer where the extracted directory path will be stored.
 * @param dirpath_size  The size of the dirpath buffer.
 */

static inline void ftp_get_dirpath_from_filepath( char *filepath, uint16_t filepath_size, char *dirpath, uint16_t dirpath_size){

    /* Copy string from filepath to dirpath */
    strncpy(dirpath, filepath, filepath_size);
    dirpath[dirpath_size - 1] = '\0';

    /* Find last slash and put the end of char there */
    char *last_slash = strrchr(dirpath, '/');
    if (last_slash) {
        *last_slash = '\0';
    } else {
        dirpath[0] = '\0';
    }

}


/**
 * @brief Allocates a free session and optionally assigns a file path to it.
 *
 * This function scans through the session array to find a free session slot.
 * Once found, it marks the session as in use and copies the provided file path
 * into the session's filepath buffer, ensuring null-termination.
 *
 * @param sessions  Array of session_t structures representing active sessions.
 * @param filepath  Optional file path to assign to the allocated session; can be NULL.
 * @return int      Index of the allocated session on success, or -1 if no free session is available.
 */

int ftp_allocate_session(session_t* sessions, char *filepath);

/**
 * @brief Frees (closes) a file session and marks it as available.
 *
 * This function closes the open file associated with the given session
 * and marks the session as not in use. It safely handles the file handle
 * by making a local copy to avoid issues with packed struct alignment.
 *
 * @param lfs          Pointer to the LittleFS instance.
 * @param sessions     Array of session_t structures representing active sessions.
 * @param session_id   Index of the session to be freed.
 */

static inline void ftp_free_session(lfs_t* lfs, session_t* sessions, int session_id) {
    /* Goes through every session */
    if (session_id >= 0 && session_id < FTP_MAX_SESSIONS) {
        lfs_file_close(lfs, &sessions[session_id].file);
        sessions[session_id].in_use = false;
    }
}

/**
 * @brief Recursively removes a file or directory and all its contents from the filesystem.
 *
 * @param lfs  Pointer to the LittleFS instance.
 * @param path Path to the file or directory to be removed.
 * @return int 0 on success, or a negative error code on failure.
 */

int ftp_remove_recursive(lfs_t *lfs, char *path);

/**
 * @brief Calculates CRC32 checksum for a given file using LittleFS.
 *
 * @param lfs       Pointer to the LittleFS instance.
 * @param path      Path to the file for which CRC32 should be calculated.
 * @param cfg       Optional file configuration (can be NULL if not used).
 * @param err_code  Pointer to an integer to store the error code.
 *                  0 on success, negative value on error.
 * @return uint32_t Computed CRC32 value, or 0 if an error occurs.
 */

uint32_t ftp_calculate_crc32_for_file(lfs_t *lfs,
                                             const char *path, 
                                             const struct lfs_file_config *cfg, 
                                             int *err_code);

#ifdef __cplusplus
}
#endif

#endif /* FTP_HELPER_H */