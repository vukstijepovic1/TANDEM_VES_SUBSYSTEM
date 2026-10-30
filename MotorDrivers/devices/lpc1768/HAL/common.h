/**
 *	@file     common.h
 *  @brief    Hardware Abstraction Layer common types.
 *  @details  v1.0
 *  @author   LisumLab
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "LPC17xx.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief Lisum Status. */
typedef enum {
	lStatus_Success = 0,
	lStatus_Fail,
	lStatus_Busy,
	lStatus_InProgress,
	lStatus_Timeout
} lStatus_t;

/*! @brief Lisum State. */
typedef enum { lState_Reset = 0, lState_Set } lState_t;

/*! @brief Lisum Function State. */
typedef enum {
	lFunctionalState_Disable = 0,
	lFunctionalState_Enable
} lFunctionalState_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 *  @brief Set Interrupt Priority.
 *
 *  @param[in] IRQn IRQ interrupt source.
 *  @param[in] priNum Interrupt priority.
 *  @return None
 */
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priNum);

/**
 *  @brief Enable Interrupt.
 *
 *  @param[in] IRQn IRQ interrupt source.
 *  @return None
 */
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);

/**
 *  @brief Disable Interrupt.
 *
 *  @param[in] IRQn IRQ interrupt source.
 *  @return None
 */
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */