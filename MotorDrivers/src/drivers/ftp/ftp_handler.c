/**
 * @file    ftp_handler.c
 * @brief   FTP handler for MavLink. It's not classical driver, here is just 
 * functions that we have in code where we use ftp. The reason is simple, we 
 * don't want to have large files without concepts.
 * @version	1.0.0
 * @date    20.06.2025
 * @author  BetaTehPro
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "ftp_handler.h"
#include "ftp_helper.h"

/*******************************************************************************6
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static mavlink_file_transfer_protocol_t last_response;
static uint16_t last_seq_number = 0xFFFF;
static uint16_t write_counter   = 0x00;
static uint32_t sync_counter    = 0x00;
static uint8_t  sync_flag       = 0x00;

/*******************************************************************************
 * Code
 ******************************************************************************/

bool ftp_process_opcode(mavlink_file_transfer_protocol_t *data, 
                        lfs_t *lfs,
                        session_t *sessions, 
                        struct lfs_file_config *file_cfg,
                        sd_t *sd, 
                        mav_t *mav_gw_sky_handle) 
{
    /* Extract sequentional number */
    uint16_t current_seq_number = ((data->payload[SEQ_NUMBER + 0] << 0)|
                                   (data->payload[SEQ_NUMBER + 1] << 8));

    if (data->payload[OPCODE] == OP_RESET_SEQ_NUM){
        last_seq_number = 0;
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_RESET_SEQ_NUM, 0, VALIDATE, NONE, 0);
        return true;
    }

    /* Handle repeated or outdated sequence numbers */
    if (current_seq_number == last_seq_number){
        memcpy(data, &last_response, sizeof(mavlink_file_transfer_protocol_t));
        return true;
    }
    else if((current_seq_number < last_seq_number) && (last_seq_number != 0xFFFF)){
        memcpy(data, &last_response, sizeof(mavlink_file_transfer_protocol_t));
        return false;
    }

    /* Track opcode and execute the operation */
    switch (data->payload[OPCODE]) {
        case OP_NONE:
            ftp_handle_op_none(data);
            break;

        case OP_TERMINATE_SESSION:
            ftp_handle_op_terminate_session(data, lfs, sessions);
            break;

        case OP_RESET_SESSIONS:
            ftp_handle_op_reset_session(data, lfs, sessions);
            break;

        case OP_LIST_DIRECTORY:
            ftp_handle_op_list_directory(data, lfs, sessions, sd);
            break;

        case OP_OPEN_FILE_RO:
            ftp_handle_op_open_file_ro(data, lfs, file_cfg, sessions);
            break;

        case OP_READ_FILE:
            ftp_handle_op_read_file(data, lfs, sessions, sd);
            break;

        case OP_CREATE_FILE:
            ftp_handle_op_create_file(data, lfs, file_cfg, sessions);
            break;

        case OP_WRITE_FILE:
            ftp_handle_op_write_file(data, lfs, sessions, file_cfg, sd);
            break;

        case OP_REMOVE_FILE:
            ftp_handle_op_remove_file(data, lfs, sessions);
            break;

        case OP_CREATE_DIRECTORY:
            ftp_handle_op_create_directory(data, lfs);
            break;

        case OP_REMOVE_DIRECTORY:
            ftp_handle_op_remove_directory(data, lfs, sessions, sd);
            break;

        case OP_OPEN_FILE_WO:
            ftp_handle_op_open_file_wo(data, lfs, file_cfg, sessions);
            break;

        case OP_TRUNCATE_FILE:
            ftp_handle_op_truncate_file(data, lfs, file_cfg, sessions);
            break;

        case OP_RENAME:
            ftp_handle_op_rename(data, lfs);
            break;

        case OP_CALC_FILE_CRC32:
            ftp_handle_op_calc_file_crc32(data, lfs, file_cfg);
            break;

        case OP_BURST_READ_FILE:
            ftp_handle_op_burst_read_file(data, lfs, file_cfg, sessions, mav_gw_sky_handle);
            break;

        default:
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_NONE, 0, ERROR, UNKNOWN_COMMAND, 0);
            break;
    }

    /* Save last valid response once after processing */
    ftp_save_last_response(data, &last_seq_number, &last_response);

    return true;
}


void ftp_handle_op_none(mavlink_file_transfer_protocol_t* data){

    /* It's ping message */
    data->payload[OPCODE]         = OP_RESPONSE_ACK;
    data->payload[REQUEST_OPCODE] = OP_NONE;
        
    return;
}

void ftp_handle_op_terminate_session(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, session_t* sessions){
    /* Takes ID of a session*/
    uint8_t session_id = data->payload[SESSION];
    
    /* Protection for sessions */
    if(session_id > FTP_MAX_SESSIONS)
    {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_TERMINATE_SESSION, session_id, ERROR, NO_SESSION_AVAILABLE, 0);
        return;
    }

    /* Check if it's in use */
    if (sessions[session_id].in_use) {
        ftp_free_session(lfs, sessions, session_id);
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_TERMINATE_SESSION, session_id, VALIDATE, NONE, 0);
    }
    else{
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_TERMINATE_SESSION, session_id, ERROR, INVALID_SESSION, 0);
    }
    
    return;
}

void ftp_handle_op_reset_session(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, session_t* sessions) {
    /* Iterate through all available FTP sessions */
    for (int session_id = 0; session_id < FTP_MAX_SESSIONS; session_id++) {
        if (sessions[session_id].in_use) {
            ftp_free_session(lfs, sessions, session_id);
        }
    }

    ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_RESET_SESSIONS, 0, VALIDATE, NONE, 0);
    return;
}

void ftp_handle_op_list_directory(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, session_t* sessions, sd_t *sd) {
    char dirpath[FTP_FILE_PATH_SIZE];
    lfs_dir_t dir;
    struct lfs_info lfs_info;
    int err = 0;

    /* Extract dirpath */
    ftp_extract_dirpath(data, dirpath, sizeof(dirpath));

    /* Protection of and empty payload */
    if(ftp_filepath_is_empty((uint8_t *)dirpath)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* Protection for data size */
    if (strlen(dirpath) >= FTP_FILE_PATH_SIZE) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* Protection of misstaken try to list root directory '.' */
    if(strcmp(dirpath, "..") == 0){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* Try to open directory to read files/dirs */
    err = lfs_dir_open(lfs, &dir, dirpath);
    if (err < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* Shifting the current index to align with the requested offset */
    int current_index = 0;
    while (current_index < data->payload[OFFSET]) {
        /* Wait until the SD card is no longer busy */
        while (sd_busy(sd)) {
            for (volatile int i = 0; i < 1000; i++);
        }

        err = lfs_dir_read(lfs, &dir, &lfs_info);
        
        /* Checks error for read directory */
        if (err <= 0) {
            lfs_dir_close(lfs, &dir);
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 0, ERROR, END_OF_FILE, 0);
            return;
        }
        current_index++;
    }

    /* Setting flags and starting positions */
    int payload_index = DATA_START;
    int entries_added = 0;
    bool end_of_directory = false;

    lfs_off_t offset = ftp_extract_u32(&data->payload[OFFSET]);
    
    while (1) {        
        /* Wait until the SD card is no longer busy */
        while (sd_busy(sd)) {
            for (volatile int i = 0; i < 1000; i++);
        }

        /* Try reading the file */
        err = lfs_dir_read(lfs, &dir, &lfs_info);

        if (err == 0) {
            /* End of directory */
            end_of_directory = true;
            break;
        } 
        else if (err < 0) {
            /* Error in system */
            lfs_dir_close(lfs, &dir);
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 0, ERROR, FAIL, 0);
            return;
        }

        /* Take the type file or directory */
        char type = (lfs_info.type == LFS_TYPE_DIR) ? 'D' : 'F';

        /* Pre-calculation of the length that snprintf would produce */
        int entry_len = snprintf(NULL, 0,                       /* Use NULL as the destination to measure length only */
                                "%c%s\t%lu",                    /* Format: type character (D/F), file name, tab, file size */
                                type,                           /* File type: 'D' for directory, 'F' for regular file */
                                lfs_info.name,                  /* The name of the file or directory */
                                (unsigned long)lfs_info.size);  /* File size (usually 0 for directories) */

        /* Add +1 to include the null terminator of sprintf function */
        entry_len += 1;

        /* If there is not enough space for the entire input, the operation is aborted without adding it */
        if ((payload_index + entry_len) >= sizeof(data->payload)) {
            break;
        }

        /* Now we safely write */
        snprintf((char*)&data->payload[payload_index],           /* Destination buffer: where the entry string will be written */
                 sizeof(data->payload) - payload_index,          /* Maximum number of bytes we can write (to prevent overflow) */
                 "%c%s\t%lu",                                    /* Format string: type (D/F), name, tab, size */
                 type,                                           /* Type of entry: 'D' for directory, 'F' for file */
                 lfs_info.name,                                  /* Name of the file or directory */
                 (unsigned long)lfs_info.size);                  /* Size of the file (0 for directories) */

        /* Increment indexes */
        payload_index += entry_len;
        entries_added++;
    }

    /* Close directory, previously we openede it */
    lfs_dir_close(lfs, &dir);

    /* Respond is ACK everything i fine, we still have data left in payload */
    if (entries_added > 1 ) {
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_LIST_DIRECTORY, 0, payload_index - DATA_START, 0, offset + entries_added-1);
        return;
    } 
    else if (end_of_directory) {
        /* We arrived to the end of directory */
        data->payload[DATA_START] = END_OF_FILE;
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 1, ERROR, END_OF_FILE, offset + entries_added-1);
        return;
    } 
    else {
        /* Fallbak of system */
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_LIST_DIRECTORY, 0, ERROR, FAIL, 0);
        return;
    }

    return;
}

void ftp_handle_op_open_file_ro(mavlink_file_transfer_protocol_t* data, lfs_t *lfs, struct lfs_file_config *file_cfg, session_t* sessions) {
    char filepath[FTP_FILE_PATH_SIZE];
    char dirpath[FTP_FILE_PATH_SIZE];
    lfs_soff_t file_size;
    struct lfs_info lfs_info;
    int lfs_stat_result;
    int session_id;
    int err;

    /* 1. Extract full file path and directory path from the payload */
    ftp_extract_filepath(data, filepath, sizeof(filepath));

    /* Protection of and empty payload */
    if(ftp_filepath_is_empty((uint8_t *)filepath)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_RO, session_id, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    ftp_get_dirpath_from_filepath(filepath, sizeof(filepath), dirpath, sizeof(dirpath));

    /* 2. Check length of both paths */
    if (strlen(filepath) >= LFS_NAME_MAX || strlen(dirpath) >= LFS_NAME_MAX) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_RO, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* 3. If it's root directory, use '.' */
    if (dirpath[0] == '\0') {
        dirpath[0] = '.';
        dirpath[1] = '\0';
    }

    /* 4. Verify that the directory exists */
    if (lfs_stat(lfs, dirpath, &lfs_info) != LFS_ERR_OK || lfs_info.type != LFS_TYPE_DIR) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_RO, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 5. Check if the file exists and is regular */
    lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);
    if (lfs_stat_result != LFS_ERR_OK || lfs_info.type != LFS_TYPE_REG) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_RO, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 6. Allocate session */
    session_id = ftp_allocate_session(sessions, filepath);
    if (session_id < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_RO, 0, ERROR, NO_SESSION_AVAILABLE, 0);
        return;
    }

    /* Set the mode of the file */
    sessions[session_id].mode = READ_ONLY;

    /* 7. Open file in read-only mode */
    err = lfs_file_opencfg(lfs, &sessions[session_id].file, filepath, LFS_O_RDONLY, file_cfg);
    if (err == 0) {
        
        file_size = lfs_file_size(lfs, &sessions[session_id].file);
        if (file_size <= 0) {
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_RO, 0, ERROR, FAIL_ERRNO, 0);
            return;
        }

        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_OPEN_FILE_RO, session_id, 4, NONE, 0);

        ftp_insert_u32(&data->payload[DATA_START], file_size);
    } 
    else {
        ftp_free_session(lfs, sessions, session_id);
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_RO, 0, ERROR, FAIL, 0);
    }
    
    return;
}

void ftp_handle_op_read_file(mavlink_file_transfer_protocol_t *data, lfs_t *lfs, session_t* sessions, sd_t *sd) {
    uint8_t session_id;
    char filepath[FTP_FILE_PATH_SIZE];
    struct lfs_info lfs_info;
    int lfs_stat_result;
    int file_size;
    lfs_off_t offset;
    int seek_result;

    /* 1. Session determination */
    session_id = data->payload[SESSION];
    if (!sessions[session_id].in_use) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, INVALID_SESSION, 0);
        return;
    }

    /* 2. Extract path from session */
    memcpy(filepath, sessions[session_id].filepath, FTP_FILE_PATH_SIZE);

    /* 3. Checking if the file is regular */
    lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);
    if (lfs_stat_result != LFS_ERR_OK || lfs_info.type != LFS_TYPE_REG) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 4. Determining file size */
    file_size = lfs_file_size(lfs, &sessions[session_id].file);
        if (file_size <= 0) {
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, INVALID_DATA_SIZE, 0);
            return;
        }

    /* 5. Extract 4-byte offset from the payload (little-endian format) */
    offset = ftp_extract_u32(&data->payload[OFFSET]);

    seek_result = lfs_file_seek(lfs, &sessions[session_id].file, offset, LFS_SEEK_SET);
    if (seek_result < 0) {
        lfs_file_close(lfs, &sessions[session_id].file);
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, END_OF_FILE, 0);

        return;
    }

    /* 6. Check if it is read only */
    if (sessions[session_id].mode != READ_ONLY) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, FILE_PROTECTED, 0);
        return; 
    }

    /* 7. Buffer allocation */
    uint8_t bytes_to_read = data->payload[SIZE];

    if(bytes_to_read == 0 || bytes_to_read > (251-12)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }
    uint8_t* read_data = &data->payload[DATA_START];

    while (sd_busy(sd)){
        for (volatile int i = 0; i < 1000; i++);
    }
    /* 8. Reading a file */
    int read_result = lfs_file_read(lfs, &sessions[session_id].file, read_data, bytes_to_read);
    if (read_result < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, FAIL, 0);
        return;
    }
    else if((offset == file_size) && (read_result == 0)){
        /*end of file*/
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_READ_FILE, session_id, ERROR, END_OF_FILE, offset + read_result);
        return;
    }

    /* 9. Prepere response */
    ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_READ_FILE, session_id, read_result, 0, offset + read_result);
    return;
}

void ftp_handle_op_create_file(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, struct lfs_file_config* file_cfg, session_t* sessions) {
    char filepath[FTP_FILE_PATH_SIZE];
    char dirpath[FTP_FILE_PATH_SIZE];
    struct lfs_info lfs_info;
    int session_id;
    int lfs_stat_result;
    int err;

    /* 1. Extract full file path and directory path from the payload */
    ftp_extract_filepath(data, filepath, sizeof(filepath));

    /* Protection of and empty payload */
    if(ftp_filepath_is_empty((uint8_t *)filepath)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_FILE, session_id, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }
    
    ftp_get_dirpath_from_filepath(filepath, sizeof(filepath), dirpath, sizeof(dirpath));

    /* 2. Check length of both paths */
    if (strlen(filepath) >= LFS_NAME_MAX || strlen(dirpath) >= LFS_NAME_MAX) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_FILE, session_id, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* 3. If it's root directory truncate it with '.' */
    if (dirpath[0] == '\0') {
        dirpath[0] = '.';
        dirpath[1] = '\0';
    }

    err = lfs_stat(lfs, dirpath, &lfs_info);
    /* 4. Verify that the directory exists and is valid */
    if (lfs_stat(lfs, dirpath, &lfs_info) != LFS_ERR_OK) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_FILE, session_id, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 5. Check if the file already exists */
    lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);
    if (lfs_stat_result == LFS_ERR_OK && lfs_info.type == LFS_TYPE_REG) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_FILE, session_id, ERROR, FILE_EXISTS, 0);
        return;
    }

    /* 6. Attempt to allocate a session for file transfer */
    session_id = ftp_allocate_session(sessions, filepath);
    if (session_id < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_FILE, session_id, ERROR, NO_SESSION_AVAILABLE, 0);
        return;
    }
    sessions[session_id].mode = WRITE_ONLY;

    /* 7. If file does not exist, attempt to create it */
    if (lfs_stat_result == LFS_ERR_NOENT) {
        err = lfs_file_opencfg(lfs, &sessions[session_id].file, filepath, LFS_O_CREAT | LFS_O_WRONLY, file_cfg);

        if (err == 0) {
            /* File successfully created */
            ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_CREATE_FILE, session_id, VALIDATE, NONE, 0);
            return;
        } else {
            /* File creation failed */
            ftp_free_session(lfs, sessions, session_id);
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_FILE, session_id, ERROR, FAIL, 0);
            return;
        }
    }

    return;
}

void ftp_handle_op_write_file(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, session_t* sessions, struct lfs_file_config *file_cfg, sd_t *sd) {
    /*
    This function handles the FTP write file operation.
    It expects:
    - session_id: ID of the active session used to identify the file being written.
    - offset (4 bytes): starting position in the file for the write operation.
    - size: number of bytes to write.
    - payload: actual data bytes to be written into the file.
    */
    uint8_t trigger_sync = 0;
    int err = 0;

    /* 1. Extract the session ID */
    uint8_t session_id = data->payload[SESSION];

    /* 2. Validate the session */
    if (!sessions[session_id].in_use) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_WRITE_FILE, session_id, ERROR, INVALID_SESSION, 0);
        return;
    }

    /* Check if the file is protected */
    if (sessions[session_id].mode != WRITE_ONLY) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_WRITE_FILE, session_id, ERROR, FILE_PROTECTED, 0);
        return; 
    }

    /* Wait while SD card is busy */
    /* On first busy detection, set sync_flag and prepare to trigger sync */
    while(sd_busy(sd)){
        for (volatile int i = 0; i < 1000; i++);
        trigger_sync = 1;
    }
    
    /* 3. Retrieve the file path from the session */
    char filepath[FTP_FILE_PATH_SIZE];
    memcpy(filepath, sessions[session_id].filepath, FTP_FILE_PATH_SIZE);

    /* 4. Check if the file still exists and is a regular file */
    struct lfs_info lfs_info;
    int lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);
    if (lfs_stat_result != LFS_ERR_OK || lfs_info.type != LFS_TYPE_REG) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_WRITE_FILE, session_id, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 5. Extract 4-byte offset from the payload (little-endian format) */
    lfs_off_t offset = ftp_extract_u32(&data->payload[OFFSET]);

    /* 6. Seek to the desired offset in the file */
    int seek_result = lfs_file_seek(lfs, &sessions[session_id].file, offset, LFS_SEEK_SET);
    if (seek_result < 0) {
        lfs_file_close(lfs, &sessions[session_id].file);
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_WRITE_FILE, session_id, ERROR, END_OF_FILE, 0);
        return;
    }

    /* 7. Extract number of bytes to write */
    uint8_t bytes_to_write = data->payload[SIZE];

    /* 8. Extract pointer to data to be written (starts at DATA_START index) */
    uint8_t* write_data = &data->payload[DATA_START];

    /* 9. Perform the write operation */
    int write_result = lfs_file_write(lfs, &sessions[session_id].file, write_data, bytes_to_write);
    if (write_result < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_WRITE_FILE, session_id, ERROR, FAIL, 0);
        return;
    }

    /* 10. Trigger sync -> flash from RAM memory to FLASH memory data */
    if (trigger_sync = 1){
        err = lfs_file_sync(lfs, &sessions[session_id].file);
        trigger_sync = 0;
    }
    if (err < 0){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_WRITE_FILE, session_id, ERROR, FAIL, 0);
    }

    /* 11. Send ACK response to confirm success */
    ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_WRITE_FILE, session_id, VALIDATE, NONE, 0);
    return;
}


void ftp_handle_op_remove_file(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, session_t* sessions) {
    char filepath[FTP_FILE_PATH_SIZE];
    char dirpath[FTP_FILE_PATH_SIZE];
    struct lfs_info lfs_info;

    /* 1. Extract full file and dir path */
    ftp_extract_filepath(data, filepath, sizeof(filepath));

    /* Protection of and empty payload */
    if(ftp_filepath_is_empty((uint8_t *)filepath)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_FILE, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    ftp_get_dirpath_from_filepath(filepath, sizeof(filepath), dirpath, sizeof(dirpath));
    
    /* If the file is in the root add dirpath as './' */
    if (dirpath[0] == '\0') {
        dirpath[0] = '.';
        dirpath[1] = '\0';
    }

    /* 2. Check if directory exists */
    int err = lfs_stat(lfs, dirpath, &lfs_info);
    if (err < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_FILE, 0, ERROR,FAIL_ERRNO, 0);
        return;
    }
    
    /* 3. Check if file exists */
    err = lfs_stat(lfs, filepath, &lfs_info);
    if (err < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_FILE, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    }
    
    /* 4. Check if it's a file */
    if (lfs_info.type != LFS_TYPE_REG) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_FILE, 0, ERROR, FILE_EXISTS , 0);
        return;
    }

    /* 5. Free sessions related to this file */
    for (int session_id = 0; session_id < FTP_MAX_SESSIONS; session_id++) {
        if (sessions[session_id].in_use &&
            strcmp(sessions[session_id].filepath, filepath) == 0) {
            ftp_free_session(lfs, sessions, session_id);
        }
    }

    /* 6. Try to remove the file */
    err = lfs_remove(lfs, filepath);
    if (err == 0) {
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_REMOVE_FILE, 0, VALIDATE, NONE, 0);
        return;
    } else {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_FILE, 0, ERROR, FAIL, 0);
        return;
    }

    return;
}

void ftp_handle_op_create_directory(mavlink_file_transfer_protocol_t* data, lfs_t* lfs) {
    char filepath[FTP_FILE_PATH_SIZE];
    struct lfs_info lfs_info;
    int lfs_stat_result;
    int err;

    /* 1. Extract full file path and directory path from the payload */
    ftp_extract_filepath(data, filepath, sizeof(filepath));

    /* Protection of and empty payload */
    if(ftp_filepath_is_empty((uint8_t *)filepath)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_DIRECTORY, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);

    /* 2. Check if directory already exists */
    if (lfs_stat_result == LFS_ERR_OK && lfs_info.type == LFS_TYPE_DIR) {
        /* Directory already exists -> send NAK */
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_DIRECTORY, 0, ERROR, FILE_EXISTS, 0);
        return;
    }

    /* 3. Directory doesn't exist -> try to create it */
    err = lfs_mkdir(lfs, filepath);
    if (err == 0) {
        /* Successfully created directory */
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_CREATE_DIRECTORY, 0, VALIDATE, NONE, 0);
        return;
    } else {
        /* Failed to create directory */
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CREATE_DIRECTORY, 0, ERROR, FAIL, 0);
        return;
    }

    return;
}

void ftp_handle_op_remove_directory(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, session_t* sessions, sd_t *sd) {
    char dirpath[FTP_FILE_PATH_SIZE];
    struct lfs_info lfs_info;
    int err;

    /* 1. Extract the directory path from the payload */
    ftp_extract_dirpath(data, dirpath, sizeof(dirpath));

    /* Protection of and empty payload */
    if(ftp_filepath_is_empty((uint8_t *)dirpath)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* 2. Check if the directory exists */
    err = lfs_stat(lfs, dirpath, &lfs_info);
    if (err < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 3. Ensure that the path points to a directory */
    if (lfs_info.type != LFS_TYPE_DIR) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, FILE_EXISTS, 0);
        return;
    }

    lfs_dir_t dir;
    
    /* 4. If trying to delete the root directory */
    if (strcmp(dirpath, ".") == 0){
        
        err = lfs_dir_open(lfs, &dir, ".");
        
        if(err != LFS_ERR_OK){
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, END_OF_FILE, 0);
            return;
        }
        int res;
        while ((res = lfs_dir_read(lfs, &dir, &lfs_info)) > 0)
        {
            if (strcmp(lfs_info.name, ".") == 0 ||
                strcmp(lfs_info.name, "..") == 0 ||
                strcmp(lfs_info.name, "boot_count") == 0) 
            {
                continue;
            }
                    
            if (lfs_info.type == LFS_TYPE_REG) {
                
                int retries = 2;
                
                do {
                    err = lfs_remove(lfs, lfs_info.name);
                    if (err == 0) break;  // uspešno obrisano

                    // Ako je greška, čekaj da kartica nije busy pre ponovnog pokušaja
                    while (sd_busy(sd)){
                        for (int i = 0; i < 1000; i++);
                    }
                } while (--retries > 0);

                if (err != LFS_ERR_OK) {
                    ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, FAIL_ERRNO, 0);
                }
            }
            else if (lfs_info.type == LFS_TYPE_DIR) {
                ftp_remove_recursive(lfs, lfs_info.name);
            }
        }
        if (res < 0) {
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, FAIL, 0);
            return;
        }                                                               

        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_REMOVE_DIRECTORY, 0, VALIDATE, NONE, 0);
        lfs_dir_close(lfs, &dir);
        return;
    }
    else if(strcmp(dirpath, "..") == 0){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, FAIL_ERRNO, 0);
        return;
    }
    else if(strcmp(dirpath, "boot_count") == 0){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, FAIL_ERRNO, 0);
        return;
    }


    /* 5. Free any sessions using files within this directory */
    for (int session_id = 0; session_id < FTP_MAX_SESSIONS; session_id++) {
        if (sessions[session_id].in_use) {
            if (strncmp(sessions[session_id].filepath, dirpath, sizeof(dirpath)) == 0 &&
                sessions[session_id].filepath[sizeof(dirpath)] == '/') {
                ftp_free_session(lfs, sessions, session_id);
            }
        }
    }

    /* 6. Try to recursively remove the directory */
    err = ftp_remove_recursive(lfs, dirpath);
    if (err == 0) {
        /* Successfully removed */
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_REMOVE_DIRECTORY, 0, VALIDATE, NONE, 0);
    } else {
        /* Failed to remove */
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_REMOVE_DIRECTORY, 0, ERROR, FAIL, 0);
    }

    return;
}

void ftp_handle_op_open_file_wo(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, struct lfs_file_config* file_cfg, session_t* sessions) {
    char filepath[FTP_FILE_PATH_SIZE];
    char dirpath[FTP_FILE_PATH_SIZE];
    struct lfs_info lfs_info;
    int lfs_stat_result;
    int session_id;
    int err;

    /* 1. Extract full file path and directory path from the payload */
    ftp_extract_filepath(data, filepath, sizeof(filepath));

    /* Protection of and empty payload */
    if(ftp_filepath_is_empty((uint8_t *)filepath)){
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_WO, session_id, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    ftp_get_dirpath_from_filepath(filepath, sizeof(filepath), dirpath, sizeof(dirpath));

    /* 2. Check length of both paths */
    if (strlen(filepath) >= LFS_NAME_MAX || strlen(dirpath) >= LFS_NAME_MAX) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_WO, session_id, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* If it's root directory, use '.' */
    if (dirpath[0] == '\0') {
        dirpath[0] = '.';
        dirpath[1] = '\0';
    }

    /* 3. Verify that the directory exists and is valid */
    if (lfs_stat(lfs, dirpath, &lfs_info) != LFS_ERR_OK || lfs_info.type != LFS_TYPE_DIR) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_WO, session_id, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 4. Check if the file exists */
    lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);
    if (lfs_stat_result == LFS_ERR_NOENT) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_WO, session_id, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 5. Attempt to allocate a session for file transfer */
    session_id = ftp_allocate_session(sessions, filepath);
    if (session_id < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_WO, session_id, ERROR, NO_SESSION_AVAILABLE, 0);
        return;
    }
    sessions[session_id].mode = WRITE_ONLY;

    /* 6. If path is valid and file exists, open it for write-only */
    if (lfs_stat_result == LFS_ERR_OK && lfs_info.type == LFS_TYPE_REG) {
        err = lfs_file_opencfg(lfs, &sessions[session_id].file, filepath, LFS_O_WRONLY, file_cfg);
        if (err == 0) {
            ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_OPEN_FILE_WO, session_id, VALIDATE, NONE, 0);
            return;
        } else {
            ftp_free_session(lfs, sessions, session_id);
            ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_WO, session_id, ERROR, FAIL, 0);
            return;
        }
    }

    /* 7. Path exists but is not a regular file */
    ftp_free_session(lfs, sessions, session_id);
    ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_OPEN_FILE_WO, session_id, ERROR, FAIL, 0);
    
    return;
}

void ftp_handle_op_truncate_file(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, struct lfs_file_config* file_cfg, session_t* sessions) {
    char filepath[FTP_FILE_PATH_SIZE];
    struct lfs_info lfs_info;
    int lfs_stat_result;
    int session_id;
    lfs_off_t offset;
    int err;

    /* 1. Extract the session ID */
    session_id = data->payload[SESSION];

    /* 2. Validate the session */
    if (!sessions[session_id].in_use) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_TRUNCATE_FILE, session_id, ERROR, INVALID_SESSION, 0);
        return;
    }
        if (sessions[session_id].mode != WRITE_ONLY) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_TRUNCATE_FILE, session_id, ERROR, FILE_PROTECTED, 0);
        return; 
    }

    /* 3. Retrieve the file path from the session */
    filepath[FTP_FILE_PATH_SIZE];
    memcpy(filepath, sessions[session_id].filepath, FTP_FILE_PATH_SIZE);

    /* 4. Check if the file still exists and is a regular file */
    lfs_info;
    lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);
    if (lfs_stat_result != LFS_ERR_OK || lfs_info.type != LFS_TYPE_REG) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_TRUNCATE_FILE, session_id, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 5. Extract 4-byte offset from the payload (little-endian format) */
    offset = ftp_extract_u32(&data->payload[OFFSET]);

    /* 7. Truncate the file at offset */
    err = lfs_file_truncate(lfs, &sessions[session_id].file, offset);
    if (err == 0) {
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_TRUNCATE_FILE, session_id, VALIDATE, NONE, 0);
        return;
    } else {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_TRUNCATE_FILE, session_id, ERROR, FAIL, 0);
        return;
    }

    return;
}

void ftp_handle_op_rename(mavlink_file_transfer_protocol_t* data, lfs_t* lfs) {
    char *old_path = (char *)&data->payload[DATA_START];
    char *new_path = old_path + strlen(old_path) + 1;
    int err;

    /* 1. If old and new path are the same */
    if (strcmp(old_path, new_path) == 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_RENAME, 0, ERROR, FAIL_ERRNO, 0);
        return;
    }

    /* 2. Validate lengths */
    if (strlen(old_path) == 0            || 
        strlen(new_path) == 0            ||
        strlen(old_path) >= LFS_NAME_MAX || 
        strlen(new_path) >= LFS_NAME_MAX) 
    {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_RENAME, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* 3. Try to rename */
    err = lfs_rename(lfs, old_path, new_path);

    /* 4. Handle error cases */
    if (err == LFS_ERR_NOENT) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_RENAME, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    } else if (err == LFS_ERR_OK) {
        ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_RENAME, 0, VALIDATE, NONE, 0);
        return;
    } else {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_RENAME, 0, ERROR, FAIL, 0);
        return;
    }

    return;
}

void ftp_handle_op_calc_file_crc32(mavlink_file_transfer_protocol_t *data, lfs_t *lfs, struct lfs_file_config* file_cfg) {
    const char *path = (const char *)&data->payload[DATA_START];
    uint32_t crc;
    int crc_err;

    /* 1. Provera dužine putanje */
    if (strlen(path) == 0 || strlen(path) >= LFS_NAME_MAX) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CALC_FILE_CRC32, 0, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* 2. Izračunavanje CRC32 */
    crc = ftp_calculate_crc32_for_file(lfs, path, file_cfg, &crc_err);

    /* 3. Provera grešaka */
    if (crc_err == LFS_ERR_NOENT) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CALC_FILE_CRC32, 0, ERROR, FILE_NOT_FOUND, 0);
        return;
    } else if (crc_err < 0) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_CALC_FILE_CRC32, 0, ERROR, FAIL, 0);
        return;
    }

    ftp_insert_u32(&data->payload[DATA_START], crc);

    /* 5. Odgovor uspeha */
    ftp_payload_prepare(data, OP_RESPONSE_ACK, OP_CALC_FILE_CRC32, 0, VALIDATE, NONE, 0);
    
    return;
}

void ftp_handle_op_burst_read_file(mavlink_file_transfer_protocol_t* data, lfs_t* lfs, struct lfs_file_config* file_cfg, session_t* sessions, mav_t* mav_gw_sky_handle){
    uint8_t session_id;
    struct lfs_info lfs_info;
    char filepath[FTP_FILE_PATH_SIZE];
    int lfs_stat_result;
    uint8_t size;
    lfs_off_t offset;
    int seek_result;

    /* 1. Checking forwarded session */
    session_id = data->payload[SESSION];
    if (!sessions[session_id].in_use) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_BURST_READ_FILE, session_id, ERROR, INVALID_SESSION, 0);
        return;
    }
    
    /* 2. Checking forwarded session */ 
    size= data->payload[SIZE];

    if (size > FTP_MAX_DATA_SIZE) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_BURST_READ_FILE, session_id, ERROR, INVALID_DATA_SIZE, 0);
        return;
    }

    /* 3. Checking if a file exists */
    memcpy(filepath, sessions[session_id].filepath, FTP_FILE_PATH_SIZE);
    lfs_stat_result = lfs_stat(lfs, filepath, &lfs_info);
    if (lfs_stat_result != LFS_ERR_OK || lfs_info.type != LFS_TYPE_REG) {
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_BURST_READ_FILE, session_id, ERROR, FILE_NOT_FOUND, 0);
        return;
    }

    /* 4. Extract 4-byte offset from the payload (little-endian format) */
    offset = ftp_extract_u32(&data->payload[OFFSET]);

    /* 5. Seek this file */
    seek_result = lfs_file_seek(lfs, &sessions[session_id].file, offset, LFS_SEEK_SET);
    if ((seek_result < 0) && (seek_result != LFS_ERR_INVAL )) {
        lfs_file_close(lfs, &sessions[session_id].file);
        ftp_payload_prepare(data, OP_RESPONSE_NAK, OP_BURST_READ_FILE, session_id, ERROR, FAIL_ERRNO, 0);
        return;
    }

    /* 5. Allocate buffer*/
    uint32_t bytes_to_read = lfs_info.size - offset;
    uint8_t* read_data = &data->payload[DATA_START];
    uint32_t total_read = 0;
    uint32_t chunk_size = 0;

    /* 6. Preparing parameters for mavlink sending */
    static const uint8_t dev_mav_sysid = 0;
    static const uint8_t dev_mav_compid = MAV_COMP_ID_USER3;
    mavlink_message_t tx_msg;

    int read_result = 0;

    /* 7. Send loop */
    while (total_read <= bytes_to_read) {

        /* 7.1. Reading the chunk */
        chunk_size = (bytes_to_read - total_read > size) ? size : (bytes_to_read - total_read);
        
        read_result = lfs_file_read(lfs,
                                    &sessions[session_id].file,
                                    &read_data[0],
                                    chunk_size
        );

        /* 7.2. Something is wrong */
        if (read_result < 0) {
            ftp_payload_prepare(data,
                                OP_RESPONSE_NAK, 
                                OP_BURST_READ_FILE, 
                                session_id, 
                                ERROR, 
                                FAIL, 
                                0
            );
            return;
        }
        else if (read_result == 0) {
            ftp_payload_prepare(data, 
                                OP_RESPONSE_ACK, 
                                OP_BURST_READ_FILE, 
                                session_id, 
                                VALIDATE, 
                                0, 
                                total_read + read_result
            );
            data->payload[BURST_COMPLETE] = 1;
            /* 7.END -- LAST CHUNK  */
            break;
        }
        else if (read_result < size) {
            int start_index = DATA_START + read_result + 1;
            memset(&data->payload[start_index], 0, 251 - start_index);
        }

        /* Delay the system */
        for (volatile int i = 0; i < 5000; i++); 

        //For Testing
        if (total_read >= 50000 && total_read <= 51000) {
            total_read += read_result;
            continue;
        }

        /* 7.3. Send chunk via MavLink*/
        ftp_payload_prepare(data, 
                            OP_RESPONSE_ACK,
                            OP_BURST_READ_FILE,
                            session_id,
                            read_result,
                            0,
                            total_read + read_result
        );
        mavlink_msg_file_transfer_protocol_encode_chan(dev_mav_sysid,
                                                       dev_mav_compid,
                                                       MAVLINK_COMM_0,
                                                       &tx_msg,
                                                       data
        );      
        mav_send(mav_gw_sky_handle, &tx_msg);
        total_read += read_result;
    }
    return;
}
