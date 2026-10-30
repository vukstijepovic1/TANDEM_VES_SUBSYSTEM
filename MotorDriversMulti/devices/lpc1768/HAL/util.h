/**
 *	@file     util.h
 *  @brief    Library which contains utility functions.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "timer.h"

#ifdef DEV_CONFIG
#include "dev_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef HAL_TIM_ROLLOVER_CHANNEL
#define HAL_TIM_ROLLOVER_CHANNEL TIM_HAL_CH_0
#endif

/*! @brief Convert miliseconds [ms] to microseconds [us] */
#define HAL_MS_TO_US(x) (x * 1000)

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
 *  @brief Initialize device time.
 *  @note  Device time and delays require usage of dedicated timer.
 *
 *  @param[in] tim timer.
 */
void HAL_DevTimeInit(tim_hal_instance_t tim);

/**
 *  @brief Delay in miliseconds.
 *
 *  @param[in] miliseconds delay period in miliseconds [ms].
 */
void HAL_DelayMS(uint32_t miliseconds);

/**
 *  @brief Delay in microseconds.
 *
 *  @param[in] microseconds delay period in microseconds [us].
 */
void HAL_DelayUS(uint64_t microseconds);

/**
 *  @brief Get device time in microseconds [us].
 *
 *  @return device time in microseconds [us] since calling HAL_DevTimeInit
 *          function.
 */
uint64_t HAL_GetTimeUS(void);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */