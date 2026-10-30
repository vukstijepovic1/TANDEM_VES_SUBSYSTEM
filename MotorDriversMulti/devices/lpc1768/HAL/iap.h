/**
 *	@file     iap.h
 *  @brief    HAL IAP library.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef IAP_H
#define IAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/
/*! @brief IAP sector number */
typedef enum {
	IAP_HAL_SECTOR_NUM_0 = 0,
	IAP_HAL_SECTOR_NUM_1,
	IAP_HAL_SECTOR_NUM_2,
	IAP_HAL_SECTOR_NUM_3,
	IAP_HAL_SECTOR_NUM_4,
	IAP_HAL_SECTOR_NUM_5,
	IAP_HAL_SECTOR_NUM_6,
	IAP_HAL_SECTOR_NUM_7,
	IAP_HAL_SECTOR_NUM_8,
	IAP_HAL_SECTOR_NUM_9,
	IAP_HAL_SECTOR_NUM_10,
	IAP_HAL_SECTOR_NUM_11,
	IAP_HAL_SECTOR_NUM_12,
	IAP_HAL_SECTOR_NUM_13,
	IAP_HAL_SECTOR_NUM_14,
	IAP_HAL_SECTOR_NUM_15,
	IAP_HAL_SECTOR_NUM_16,
	IAP_HAL_SECTOR_NUM_17,
	IAP_HAL_SECTOR_NUM_18,
	IAP_HAL_SECTOR_NUM_19,
	IAP_HAL_SECTOR_NUM_20,
	IAP_HAL_SECTOR_NUM_21,
	IAP_HAL_SECTOR_NUM_22,
	IAP_HAL_SECTOR_NUM_23,
	IAP_HAL_SECTOR_NUM_24,
	IAP_HAL_SECTOR_NUM_25,
	IAP_HAL_SECTOR_NUM_26,
	IAP_HAL_SECTOR_NUM_27,
	IAP_HAL_SECTOR_NUM_28,
	IAP_HAL_SECTOR_NUM_29
} iap_hal_sector_num_t;

/*! @brief IAP sector start address */
typedef enum {
	IAP_HAL_SECTOR_0_ADDR = 0x00000000,
	IAP_HAL_SECTOR_1_ADDR = 0x00001000,
	IAP_HAL_SECTOR_2_ADDR = 0x00002000,
	IAP_HAL_SECTOR_3_ADDR = 0x00003000,
	IAP_HAL_SECTOR_4_ADDR = 0x00004000,
	IAP_HAL_SECTOR_5_ADDR = 0x00005000,
	IAP_HAL_SECTOR_6_ADDR = 0x00006000,
	IAP_HAL_SECTOR_7_ADDR = 0x00007000,
	IAP_HAL_SECTOR_8_ADDR = 0x00008000,
	IAP_HAL_SECTOR_9_ADDR = 0x00009000,
	IAP_HAL_SECTOR_10_ADDR = 0x0000A000,
	IAP_HAL_SECTOR_11_ADDR = 0x0000B000,
	IAP_HAL_SECTOR_12_ADDR = 0x0000C000,
	IAP_HAL_SECTOR_13_ADDR = 0x0000D000,
	IAP_HAL_SECTOR_14_ADDR = 0x0000E000,
	IAP_HAL_SECTOR_15_ADDR = 0x0000F000,
	IAP_HAL_SECTOR_16_ADDR = 0x00010000,
	IAP_HAL_SECTOR_17_ADDR = 0x00018000,
	IAP_HAL_SECTOR_18_ADDR = 0x00020000,
	IAP_HAL_SECTOR_19_ADDR = 0x00028000,
	IAP_HAL_SECTOR_20_ADDR = 0x00030000,
	IAP_HAL_SECTOR_21_ADDR = 0x00038000,
	IAP_HAL_SECTOR_22_ADDR = 0x00040000,
	IAP_HAL_SECTOR_23_ADDR = 0x00048000,
	IAP_HAL_SECTOR_24_ADDR = 0x00050000,
	IAP_HAL_SECTOR_25_ADDR = 0x00058000,
	IAP_HAL_SECTOR_26_ADDR = 0x00060000,
	IAP_HAL_SECTOR_27_ADDR = 0x00068000,
	IAP_HAL_SECTOR_28_ADDR = 0x00070000,
	IAP_HAL_SECTOR_29_ADDR = 0x00078000
} iap_hal_sector_addr_t;

/*! @brief IAP status code */
typedef enum {
	IAP_HAL_CMD_SUCCESS,
	// Command is executed successfully.
	IAP_HAL_INVALID_COMMAND,
	// Invalid command.
	IAP_HAL_SRC_ADDR_ERROR,
	// Source address is not on a word boundary.
	IAP_HAL_DST_ADDR_ERROR,
	// Destination address is not on a correct boundary.
	IAP_HAL_SRC_ADDR_NOT_MAPPED,
	// Source address is not mapped in the memory map.
	IAP_HAL_DST_ADDR_NOT_MAPPED,
	// Destination address is not mapped in the memory map.
	IAP_HAL_COUNT_ERROR,
	// Byte count is not multiple of 4 or is not a permitted value.
	IAP_HAL_INVALID_SECTOR,
	// Sector number is invalid.
	IAP_HAL_SECTOR_NOT_BLANK,
	// Sector is not blank.
	IAP_HAL_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
	// Command to prepare sector for write operation was not executed.
	IAP_HAL_COMPARE_ERROR,
	// Source and destination data is not same.
	IAP_HAL_BUSY
	// Flash programming hardware interface is busy.
} iap_hal_status_code_t;

/*! @brief IAP write size */
typedef enum {
	IAP_HAL_WRITE_256 = 256,
	IAP_HAL_WRITE_512 = 512,
	IAP_HAL_WRITE_1024 = 1024,
	IAP_HAL_WRITE_4096 = 4096
} iap_hal_write_size_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
/**
 * @brief Get sector number
 *
 * @param[in] address: Sector address
 * @return Sector number
 */
iap_hal_sector_num_t HAL_IAP_GetSectorNumber(iap_hal_sector_addr_t address);

/**
 * @brief Prepare sector(s) for write operation
 *
 * @param[in] startSector: The number of start sector
 * @param[in] endSector: The number of end sector
 * @return Status code: IAP_HAL_CMD_SUCCESS
 *                      IAP_HAL_BUSY
 *                      IAP_HAL_INVALID_SECTOR
 */
iap_hal_status_code_t
HAL_IAP_PrepareSector(const iap_hal_sector_num_t startSector,
					  const iap_hal_sector_num_t endSector);

/**
 * @brief Copy RAM to Flash
 *
 * @param[in] dest: Destination address (in Flash)
 * @param[in] source: Source address (in RAM)
 * @param[in] size: Write size
 * @return Status code: IAP_HAL_CMD_SUCCESS
 *                      IAP_HAL_SRC_ADDR_ERROR
 *                      IAP_HAL_DST_ADDR_ERROR
 *                      IAP_HAL_SRC_ADDR_NOT_MAPPED
 *                      IAP_HAL_DST_ADDR_NOT_MAPPED
 *                      IAP_HAL_COUNT_ERROR
 *                      IAP_HAL_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
 *                      IAP_HAL_BUSY
 */
iap_hal_status_code_t HAL_IAP_CopyRAM2Flash(uint8_t *dest, uint8_t *source,
											const iap_hal_write_size_t size);

/**
 * @brief Erase sector(s)
 *
 * @param[in] startSector: The number of start sector
 * @param[in] endSector: The number of end sector
 * @return Status code: IAP_HAL_CMD_SUCCESS
 *                      IAP_HAL_INVALID_SECTOR
 *                      IAP_HAL_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
 *                      IAP_HAL_BUSY
 */
iap_hal_status_code_t
HAL_IAP_EraseSector(const iap_hal_sector_num_t startSector,
					const iap_hal_sector_num_t endSector);

/**
 * @brief Blank check sector(s)
 *
 * @param[in] startSector: The number of start sector
 * @param[in] endSector: The number of end sector
 * @param[out] firstNotBlankLoc: The offset of the first non-blank word
 *  @param[out] firstNotBlankVal: The value of the first non-blank word
 * @return Status code: IAP_HAL_CMD_SUCCESS
 *                      IAP_HAL_INVALID_SECTOR
 *                      IAP_HAL_SECTOR_NOT_BLANK
 *                      IAP_HAL_BUSY
 */
iap_hal_status_code_t
HAL_IAP_BlankCheckSector(const iap_hal_sector_num_t startSector,
						 const iap_hal_sector_num_t endSector,
						 uint32_t *firstNotBlankLoc,
						 uint32_t *firstNotBlankVal);

/**
 * @brief Read part identification number
 *
 * @param[out] partID: Part ID
 * @return Status code: IAP_HAL_CMD_SUCCESS
 */
iap_hal_status_code_t HAL_IAP_ReadPartID(uint32_t *partID);

/**
 * @brief Read boot code version.
 *        The version is interpreted as <major>.<minor>
 *
 * @param[out] major: The major
 * @param[out] minor:  The minor
 * @return Status code: IAP_HAL_CMD_SUCCESS
 */
iap_hal_status_code_t HAL_IAP_ReadBootCodeVer(uint8_t *major, uint8_t *minor);

/**
 * @brief Read device serial number
 *
 * @param[out] uid: Serial number
 * @return Status code: IAP_HAL_CMD_SUCCESS
 */
iap_hal_status_code_t HAL_IAP_ReadDeviceSerialNum(uint32_t *uid);

/**
 * @brief Compare the memory contents at two locations
 *
 * @param[in] addr1: The first address
 * @param[in] addr2: The second address
 * @param[in] size: Number of bytes to be compared, must be a multiple of 4
 * @return Status code: IAP_HAL_CMD_SUCCESS
 *                      IAP_HAL_COMPARE_ERROR
 *                      IAP_HAL_COUNT_ERROR (Byte count is not a multiple of 4)
 *                      IAP_HAL_ADDR_ERROR
 *                      IAP_HAL_ADDR_NOT_MAPPED
 */
iap_hal_status_code_t HAL_IAP_Compare(uint8_t *addr1, uint8_t *addr2,
									  const uint32_t size);

/**
 * @brief Reinvoke ISP
 *
 * @return None
 */
void HAL_IAP_ReInvokeISP();

#ifdef __cplusplus
}
#endif

#endif /* IAP_H */