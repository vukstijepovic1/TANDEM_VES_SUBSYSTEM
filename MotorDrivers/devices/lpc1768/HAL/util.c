/**
 *	@file     util.c
 *  @brief    Library which contains utility functions.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "util.h"

#include "common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

tim_hal_instance_t prvUTIL_TIM_INSTANCE;
uint64_t dev_time_us;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void prvUTIL_TIM_CALLBACK(tim_hal_ch_t ch, tim_hal_int_type_t type);

/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_DevTimeInit(tim_hal_instance_t tim)
{
	tim_hal_cfg_t tim_cfg;
	tim_hal_match_cfg_t match_cfg;

	/** @todo: add TIMER Restart */

	prvUTIL_TIM_INSTANCE = tim;
	dev_time_us = 0;

	tim_cfg.Prescale = TIM_HAL_PRESCALE_US;
	tim_cfg.PrescaleValue = 1;

	// initialize timer
	HAL_TIM_Init(prvUTIL_TIM_INSTANCE, &tim_cfg);

	match_cfg.MatchChannel = HAL_TIM_ROLLOVER_CHANNEL;
	match_cfg.StopOnMatch = lFunctionalState_Disable;
	match_cfg.ResetOnMatch = lFunctionalState_Enable;
	match_cfg.MatchValue = 0xFFFFFFFFU;

	// configure callback on timer rollover
	HAL_TIM_MatchCfg(prvUTIL_TIM_INSTANCE, &match_cfg);
	HAL_TIM_EnableInterrupt(prvUTIL_TIM_INSTANCE, prvUTIL_TIM_CALLBACK);
}

void HAL_DelayMS(uint32_t miliseconds)
{
	HAL_DelayUS(HAL_MS_TO_US(miliseconds));
}

void HAL_DelayUS(uint64_t microseconds)
{
	uint64_t time0 = HAL_GetTimeUS();
	while (HAL_GetTimeUS() - time0 < microseconds)
		;
}

uint64_t HAL_GetTimeUS(void)
{
	return (HAL_TIM_GetVal(prvUTIL_TIM_INSTANCE) + dev_time_us);
}

/****************************** static functions ******************************/

/**
 *  @brief Device timer callback, used for timer rollover handling.
 *
 *  @param[in] ch match channel.
 */
void prvUTIL_TIM_CALLBACK(tim_hal_ch_t ch, tim_hal_int_type_t type)
{
	if (type == TIM_HAL_INT_TYPE_MATCH) {
		switch (ch) {
		case HAL_TIM_ROLLOVER_CHANNEL:
			dev_time_us += 0xFFFFFFFFU;

			break;
		default:
			break;
		}
	}
}

/********************************* End Of File ********************************/