/**
 *	@file     rit.h
 *  @brief    HAL RIT library.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef RIT_H
#define RIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef HAL_RIT_NVIC_PRIORITY
/*! NVIC Priority */
#define HAL_RIT_NVIC_PRIORITY 6
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief RIT Instance. */
typedef enum { RIT_HAL_INSTANCE_0 = 0 } rit_hal_instance_t;

typedef void (*rit_hal_callback_t)(rit_hal_instance_t instance);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Function for RIT initialization
 *
 * @param instance: RIT instance
 * @return None
 */
void HAL_RIT_Init(rit_hal_instance_t instance);

/**
 * @brief Function for RIT deinitialization
 *
 * @param instance: RIT instance
 * @return None
 */
void HAL_RIT_DeInit(rit_hal_instance_t instance);

/**
 * @brief Function for RIT timer configuration
 *
 * @param instance: RIT instance
 * @param interval: Timer interval value, unit ms
 * @return None
 */
void HAL_RIT_TimerConfig(rit_hal_instance_t instance, uint32_t interval);

/**
 * @brief Function for Interrupt enabling
 *
 * @param instance: RIT instance
 * @param callback: Pointer to the callback function
 * @return None
 */
void HAL_RIT_EnableInterrupt(rit_hal_instance_t instance,
							 rit_hal_callback_t callback);

/**
 * @brief Function for RIT timer enable/disable
 *
 * @param instance: RIT instance
 * @param state: New state
 * @return None
 */
void HAL_RIT_Cmd(rit_hal_instance_t instance, lFunctionalState_t state);

/**
 * @brief Function for getting interrupt status
 *
 * @param instance: RIT instance
 * @retval ::lState_t
 */
lState_t HAL_Rit_GetIntStatus(rit_hal_instance_t instance);

#ifdef __cplusplus
}
#endif

#endif /* RIT_H */