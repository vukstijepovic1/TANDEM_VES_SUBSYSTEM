/**
 *	@file     adc.h
 *  @brief    HAL ADC library.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef ADC_H
#define ADC_H

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

#ifndef HAL_ADC_NVIC_PRIORITY
/*! NVIC Priority */
#define HAL_ADC_NVIC_PRIORITY 6
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief ADC instance */
typedef enum { ADC_HAL_INSTANCE_0 = 0 } adc_hal_instance_t;

/*! @brief Channel */
typedef enum {
	ADC_HAL_CHANNEL_0 = 0,
	ADC_HAL_CHANNEL_1 = 1,
	ADC_HAL_CHANNEL_2 = 2,
	ADC_HAL_CHANNEL_3 = 3,
	ADC_HAL_CHANNEL_4 = 4,
	ADC_HAL_CHANNEL_5 = 5,
	ADC_HAL_CHANNEL_6 = 6,
	ADC_HAL_CHANNEL_7 = 7
} adc_hal_channel_t;

/*! @brief Start mode */
typedef enum {
	ADC_HAL_START_MODE_CONTINUOUS = 0,
	ADC_HAL_START_MODE_NOW = 1
} adc_hal_start_mode_t;

/*! @brief Data status */
typedef enum {
	ADC_HAL_DATA_STATUS_BURST = 0,
	ADC_HAL_DATA_STATUS_DONE = 1
} adc_hal_data_status_t;

typedef void (*adc_hal_callback_t)(adc_hal_instance_t instance);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Function for ADC initialization
 *
 * @param instance: ADC instance
 * @param rate: ADC conversion rate
 * @return None
 */
void HAL_ADC_Init(adc_hal_instance_t instance, uint32_t rate);

/**
 * @brief Function for ADC deinitialization
 *
 * @param instance: ADC instance
 * @return None
 */
void HAL_ADC_DeInit(adc_hal_instance_t instance);

/**
 * @brief Function for ADC channel configuration
 *
 * @param instance: ADC instance
 * @param channel: ADC channel
 * @return None
 */
void HAL_ADC_ChannelConfig(adc_hal_instance_t instance,
						   adc_hal_channel_t channel);

/**
 * @brief Function for ADC channel enable/disable
 *
 * @param instance: ADC instance
 * @param channel: ADC channel
 * @param state: New state
 * @return None
 */
void HAL_ADC_ChannelCmd(adc_hal_instance_t instance, adc_hal_channel_t channel,
						lFunctionalState_t state);

/**
 * @brief Function for starting ADC conversion
 *
 * @param instance: ADC instance
 * @param startMode: Start mode
 * @return None
 */
void HAL_ADC_StartCmd(adc_hal_instance_t instance,
					  adc_hal_start_mode_t startMode);

/**
 * @brief Function for ADC burst mode enable/disable
 *
 * @param instance: ADC instance
 * @param channel: ADC channel
 * @param state: New state
 * @return None
 */
void HAL_ADC_BurstCmd(adc_hal_instance_t instance, adc_hal_channel_t channel,
					  lFunctionalState_t state);

/**
 * @brief Function for enabling interrupt
 *
 * @param instance: ADC instance
 * @param callback: Pointer to the callback function
 * @param channel: ADC channel
 * @return None
 */
void HAL_ADC_EnableInterrupt(adc_hal_instance_t instance,
							 adc_hal_callback_t callback,
							 adc_hal_channel_t channel);

/**
 * @brief Function for enable/disable interrupt
 *
 * @param instance: ADC instance
 * @param state: New state
 * @return None
 */
void HAL_ADC_IntCmd(adc_hal_instance_t instance, lFunctionalState_t state);

/**
 * @brief Function for fetching data
 *
 * @param instance: ADC instance
 * @param channel: ADC channel
 * @return ADC value
 */
uint16_t HAL_ADC_GetData(adc_hal_instance_t instance,
						 adc_hal_channel_t channel);

/**
 * @brief Function for getting channel status
 *
 * @param instance: ADC instance
 * @param channel: ADC channel
 * @param statusType: Status type
 * @return Channel status
 */
lState_t HAL_ADC_GetChannelStatus(adc_hal_instance_t instance,
								  adc_hal_channel_t channel,
								  adc_hal_data_status_t statusType);

/**
 * @brief Function for fetching data in polling mode with timeout
 *
 * @param instance: ADC instance
 * @param channel: ADC channel
 * @param data: Pointer to ADC data
 * @param timeoutMs: Transfer timeout [ms]
 * @return Transfer status
 */
lStatus_t HAL_ADC_PollData(adc_hal_instance_t instance,
						   adc_hal_channel_t channel, uint16_t *data,
						   uint32_t timeoutMs);

#ifdef __cplusplus
}
#endif

#endif /* ADC_H */