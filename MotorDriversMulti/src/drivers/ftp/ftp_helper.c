/**
 * @file    ftp_helper.c
 * @brief   Helper functions for MAVLink FTP protocol operations using LittleFS.
 *
 * This source file implements utility functions that assist in managing
 * FTP sessions, preparing and parsing MAVLink FTP messages, handling
 * file and directory operations on the LittleFS filesystem, and computing
 * CRC32 checksums for file integrity verification.
 *
 * The helpers are designed to keep the FTP-related code modular and maintainable,
 * avoiding bloated monolithic files by separating common FTP functionalities.
 *
 * Key features include:
 * - Saving and retransmitting the last FTP response
 * - Allocating and freeing FTP sessions
 * - Recursive removal of files and directories
 * - Computing CRC32 checksums for files
 *
 * @version 1.0.0
 * @date    20.06.2025
 * @author  BetaTehPro
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "ftp_helper.h"

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
 * Code
 ******************************************************************************/



void ftp_save_last_response(mavlink_file_transfer_protocol_t *data, uint16_t *last_seq_number, mavlink_file_transfer_protocol_t *last_response) {
    *last_seq_number = ((data->payload[SEQ_NUMBER + 0] << 0) |
                        (data->payload[SEQ_NUMBER + 1] << 8));
                        
    memcpy(last_response, data, sizeof(mavlink_file_transfer_protocol_t));
}

void ftp_timeout(QueueHandle_t queue,
                 mav_t *mav_gw_sky_handle,
                 mavlink_message_t *tx_msg,
                 uint8_t time2try,
                 uint8_t delay)
{
    mavlink_file_transfer_protocol_t data;

    for (int i = 0; i < time2try; i++) {
        mav_send(mav_gw_sky_handle, tx_msg);
        
        if (xQueuePeek(queue, &data, pdMS_TO_TICKS(delay))) {
            // Answer received, break it
            break;
        }
    }
} 

int ftp_allocate_session(session_t* sessions, char *filepath) {
    /* Goes through the every session */
    for (int i = 0; i < FTP_MAX_SESSIONS; i++) {
        /* Check if it's available */
        if (!sessions[i].in_use) {
            sessions[i].in_use = true;
            /* Check if it's available */
            if (filepath) {
                strncpy(sessions[i].filepath, filepath, FTP_FILE_PATH_SIZE - 1);
                sessions[i].filepath[FTP_FILE_PATH_SIZE - 1] = '\0';  
            } else {
                sessions[i].filepath[0] = '\0';
            }
            return i;  /* Return ID of taken session */
        }
    }
    return -1;
}

int ftp_remove_recursive(lfs_t *lfs, char *path)
{
    struct lfs_info info;
    int err;

    /* Get file/directory info for the given path */
    err = lfs_stat(lfs, path, &info);
    if (err < 0) {
        /* Path does not exist or error occurred  */
        return err;
    }

    /* If it's a regular file, remove it */
    if (info.type == LFS_TYPE_REG) {
        return lfs_remove(lfs, path);
    }

    /* If it's a directory, recursively remove its contents */
    if (info.type == LFS_TYPE_DIR) {
        lfs_dir_t dir;
        err = lfs_dir_open(lfs, &dir, path);
        if (err < 0) return err;

        while (true) {
            err = lfs_dir_read(lfs, &dir, &info);
            if (err <= 0) break; /* No more entries or error occurred */

            /* Skip "." and ".." entries */
            if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
                continue;
            }

            /* Build the full path for the child entry */
            char fullpath[256];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, info.name);

            /* Recursively remove the entry */
            err = ftp_remove_recursive(lfs, fullpath);
            if (err < 0) break;
        }

        lfs_dir_close(lfs, &dir);
        if (err < 0) return err;

        /* Remove the now-empty directory itself */
        return lfs_remove(lfs, path);
    }

    /* Return success if nothing was removed (shouldn't happen in normal cases) */
    return 0;
}



uint32_t ftp_calculate_crc32_for_file(lfs_t *lfs, const char *path, const struct lfs_file_config *cfg, int *err_code) {
    lfs_file_t file;
    uint8_t buffer[64];
    lfs_ssize_t bytes_read;
    
    /* MAVLink specific start value */
    uint32_t crc = CRC32_INIT;

    *err_code = lfs_file_opencfg(lfs, &file, path, LFS_O_RDONLY, cfg);
    if (*err_code < 0) {
        return 0;
    }

    while ((bytes_read = lfs_file_read(lfs, &file, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            crc ^= buffer[i];
            for (int j = 0; j < 8; j++) {
                if (crc & 1)
                    crc = (crc >> 1) ^ CRC32_POLYNOM_REFLECTED;
                else
                    crc >>= 1;
            }
        }
    }

    if (bytes_read < 0) {
        *err_code = bytes_read;
        lfs_file_close(lfs, &file);
        return 0;
    }

    /* No final XOR for MAVLink! */
    lfs_file_close(lfs, &file);
    *err_code = 0;
    
    return crc;  
}