/**
 * @file    canopen_def.h
 * @brief   CANopen Library defines.
 * @version 1.1.0
 * @date    06.03.2025
 * @author  LisumLab
 */

#ifndef CANOPEN_DEF_H
#define CANOPEN_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#ifdef CANOPEN_CONFIG
#include "canopen_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define CANOPEN_DEF_MAX_NODE_ID 0x7FU //!< Max Node ID

#define CANOPEN_DEF_FCN_CODE_MASK (0b1111 << 7)	  //!< Function Code Mask
#define CANOPEN_DEF_NODE_ID_MASK (0b1111111 << 0) //!< Node ID Mask

#define CANOPEN_DEF_SYNC_MSG_SIZE 0
#define CANOPEN_DEF_ERROR_MSG_SIZE 1
#define CANOPEN_DEF_NMT_MSG_SIZE 2
#define CANOPEN_DEF_SDO_MSG_SIZE 8

// Download/Upload initiate data size
#define CANOPEN_DEF_SDO_INIT_DATA_SIZE 4
// Download/Upload segment data size
#define CANOPEN_DEF_SDO_SEG_DATA_SIZE 7

// Download segment request client command specifier
#define CANOPEN_DEF_SDO_CCS_DOWNLOAD_SEG_REQ 0
// Download initiate request client command specifier
#define CANOPEN_DEF_SDO_CCS_DOWNLOAD_INIT_REQ 1
// Upload initiate request client command specifier
#define CANOPEN_DEF_SDO_CCS_UPLOAD_INIT_REQ 2
// Upload segment request client command specifier
#define CANOPEN_DEF_SDO_CCS_UPLOAD_SEG_REQ 3

// Upload segment response server command specifier
#define CANOPEN_DEF_SDO_SCS_UPLOAD_SEG_RESP 0
// Download segment response server command specifier
#define CANOPEN_DEF_SDO_SCS_DOWNLOAD_SEG_RESP 1
// Upload initiate response server command specifier
#define CANOPEN_DEF_SDO_SCS_UPLOAD_INIT_RESP 2
// Download initiate response server command specifier
#define CANOPEN_DEF_SDO_SCS_DOWNLOAD_INIT_RESP 3

// Abort transfer command specifier
#define CANOPEN_DEF_SDO_CS_ABORT_TRANSFER 4

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

#endif /* CANOPEN_DEF_H */