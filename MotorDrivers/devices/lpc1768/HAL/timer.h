/**
 *	@file     timer.h
 *  @brief    HAL Timer library.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "common.h"
#include <stdint.h>

#ifdef DEV_CONFIG
#include "dev_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef HAL_TIMER_NVIC_PRIORITY
/*! NVIC Priority */
#define HAL_TIMER_NVIC_PRIORITY 9U
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief Timer instance */
typedef enum {
	TIM_HAL_INSTANCE_0 = 0, //!< Timer 0
	TIM_HAL_INSTANCE_1,		//!< Timer 1
	TIM_HAL_INSTANCE_2,		//!< Timer 2
	TIM_HAL_INSTANCE_3		//!< Timer 3
} tim_hal_instance_t;

/*! @brief Prescale type */
typedef enum {
	TIM_HAL_PRESCALE_TICKS = 0, //!< Prescale in Ticks
	TIM_HAL_PRESCALE_US			//!< Prescale in microseconds [us]
} tim_hal_prescale_t;

/*! @brief Timer channel */
typedef enum {
	TIM_HAL_CH_0 = 0, //!< Timer channel 0 (match or capture)
	TIM_HAL_CH_1,	  //!< Timer channel 1 (match or capture)
	TIM_HAL_CH_2,	  //!< Timer channel 2 (only match)
	TIM_HAL_CH_3	  //!< Timer channel 3 (only match)
} tim_hal_ch_t;

/*! @brief Timer interrupt type */
typedef enum {
	TIM_HAL_INT_TYPE_MATCH = 0,
	TIM_HAL_INT_TYPE_CAPTURE
} tim_hal_int_type_t;

/*! @brief Timer config struct */
typedef struct tim_hal_cfg_t {
	tim_hal_prescale_t Prescale; //!< Prescale Type
	uint32_t PrescaleValue;		 //!< Prescale Value
} tim_hal_cfg_t;

/*! @brief Timer match config */
typedef struct tim_hal_match_cfg_t {
	tim_hal_ch_t MatchChannel;		 //!< Match Channel
	uint32_t MatchValue;			 //!< Match Value
	lFunctionalState_t StopOnMatch;	 //!< Should timer stop on match
	lFunctionalState_t ResetOnMatch; //!< Should timer reset on match
} tim_hal_match_cfg_t;

typedef struct tim_hal_capture_cfg_t {
	tim_hal_ch_t captureChannel;	 //!< Capture Channel
	lFunctionalState_t risingEdge;	 //!< Rising Edge
	lFunctionalState_t fallingEdge;	 //!< Falling Edge
	lFunctionalState_t intOnCaption; //!< Interrupt on Caption
} tim_hal_capture_cfg_t;

typedef void (*tim_hal_callback_t)(tim_hal_ch_t ch, tim_hal_int_type_t type);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 *  @brief Initialize timer.
 *
 *  @param[in] instance Timer Instance.
 *  @param[in] cfg Timer Config.
 *  @return None
 */
void HAL_TIM_Init(tim_hal_instance_t instance, tim_hal_cfg_t *cfg);

/**
 *  @brief Deinitialize timer.
 *
 *  @param[in] instance Timer Instance.
 *  @return None
 */
void HAL_TIM_DeInit(tim_hal_instance_t instance);

/**
 *  @brief Configure Timer match value.
 *
 *  @param[in] instance Timer Instance.
 *  @param[in] cfg Timer match value config.
 *  @return None
 */
void HAL_TIM_MatchCfg(tim_hal_instance_t instance, tim_hal_match_cfg_t *cfg);

/**
 *  @brief Enable Timer interrupt and configure callback.
 *
 *  @param[in] instance Timer Instance.
 *  @param[in] cb User callback.
 *  @return None
 */
void HAL_TIM_EnableInterrupt(tim_hal_instance_t instance,
							 tim_hal_callback_t cb);

/**
 *  @brief Get Timer value.
 *
 *  @param[in] instance Timer Instance.
 *  @return Timer Counter Value.
 */
uint32_t HAL_TIM_GetVal(tim_hal_instance_t instance);

/**
 *  @brief Configure Timer Capture
 *
 *  @param[in] instance   Timer instance
 *  @param[in] captureCfg Timer capture config
 */
void HAL_TIM_ConfigCapture(tim_hal_instance_t instance,
						   tim_hal_capture_cfg_t *captureCfg);

/**
 *  @brief Get Capture value
 *
 *  @param[in] instance Timer instance
 *  @param[in] channel  Timer channel
 */
uint32_t HAL_TIM_GetCaptureValue(tim_hal_instance_t instance,
								 tim_hal_ch_t channel);

/**
 *  @brief Reset Timer
 *
 *  @param[in] instance Timer instance
 */
void HAL_TIM_ResetCounter(tim_hal_instance_t instance);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H */