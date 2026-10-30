/**
 *	@file     iap.c
 *  @brief    HAL IAP library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "iap.h"
#include "lpc17xx_iap.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
iap_hal_sector_num_t HAL_IAP_GetSectorNumber(iap_hal_sector_addr_t address)
{
	return GetSecNum(address);
}

iap_hal_status_code_t
HAL_IAP_PrepareSector(const iap_hal_sector_num_t startSector,
					  const iap_hal_sector_num_t endSector)
{
	return PrepareSector(startSector, endSector);
}

iap_hal_status_code_t HAL_IAP_CopyRAM2Flash(uint8_t *dest, uint8_t *source,
											const iap_hal_write_size_t size)
{
	return CopyRAM2Flash(dest, source, size);
}

iap_hal_status_code_t
HAL_IAP_EraseSector(const iap_hal_sector_num_t startSector,
					const iap_hal_sector_num_t endSector)
{
	return EraseSector(startSector, endSector);
}

iap_hal_status_code_t
HAL_IAP_BlankCheckSector(const iap_hal_sector_num_t startSector,
						 const iap_hal_sector_num_t endSector,
						 uint32_t *firstNotBlankLoc, uint32_t *firstNotBlankVal)
{
	return BlankCheckSector(startSector, endSector, firstNotBlankLoc,
							firstNotBlankVal);
}

iap_hal_status_code_t HAL_IAP_ReadPartID(uint32_t *partID)
{
	return ReadPartID(partID);
}

iap_hal_status_code_t HAL_IAP_ReadBootCodeVer(uint8_t *major, uint8_t *minor)
{
	return ReadBootCodeVer(major, minor);
}

iap_hal_status_code_t HAL_IAP_ReadDeviceSerialNum(uint32_t *uid)
{
	return ReadDeviceSerialNum(uid);
}

iap_hal_status_code_t HAL_IAP_Compare(uint8_t *addr1, uint8_t *addr2,
									  const uint32_t size)
{
	return Compare(addr1, addr2, size);
}

void HAL_IAP_ReInvokeISP() { InvokeISP(); }

/************************************ IRQs ************************************/

/****************************** static functions ******************************/

/* --------------------------------- End Of File -----------------------------*/