/**********************************************************************
* $Id$		lpc17xx_libcfg_default.h				2010-05-21
*//**
* @file		lpc17xx_libcfg_default.h
* @brief	Default Library configuration header file
* @version	2.0
* @date		21. May. 2010
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2010, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

/* Library Configuration group ----------------------------------------------------------- */
/** @defgroup LIBCFG_DEFAULT LIBCFG_DEFAULT (Default Library Configuration)
 * @ingroup LPC1700CMSIS_FwLib_Drivers
 * @{
 */

#ifndef LPC17XX_LIBCFG_DEFAULT_H_
#define LPC17XX_LIBCFG_DEFAULT_H_

/* Includes ------------------------------------------------------------------- */
#include "lpc_types.h"
#include <assert.h>
#include <string.h>

/* Public Macros -------------------------------------------------------------- */
/** @defgroup LIBCFG_DEFAULT_Public_Macros LIBCFG_DEFAULT Public Macros
 * @{
 */

/************************** GLOBAL/PUBLIC MACRO DEFINITIONS *********************************/

/*******************************************************************************
* @brief		The CHECK_PARAM macro is used for function's parameters check.
* 				It is used only if the library is compiled in DEBUG mode.
* @param[in]	expr - If expr is false, stops execution
* @return		None
*******************************************************************************/
#define CHECK_PARAM(expr) assert(expr)


/**
 * @}
 */

#endif /* LPC17XX_LIBCFG_DEFAULT_H_ */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
