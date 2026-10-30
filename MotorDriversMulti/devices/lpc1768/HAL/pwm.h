/**
 *	@file     pwm.h
 *  @brief    HAL PWM library
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef PWM_H
#define PWM_H

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

#ifndef HAL_PWM_NVIC_PRIORITY
/*! NVIC Priority */
#define HAL_PWM_NVIC_PRIORITY 6
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief PWM instance */
typedef enum { PWM_HAL_INSTANCE_0 = 0 } pwm_hal_instance_t;

/*! @brief Channel */
typedef enum {
	PWM_HAL_CHANNEL_0 = 0, /*!< PWM channel 0 (match or capture) */
	PWM_HAL_CHANNEL_1 = 1, /*!< PWM channel 1 (match or capture) */
	PWM_HAL_CHANNEL_2 = 2, /*!< PWM channel 2 (only match) */
	PWM_HAL_CHANNEL_3 = 3, /*!< PWM channel 3 (only match) */
	PWM_HAL_CHANNEL_4 = 4, /*!< PWM channel 4 (only match) */
	PWM_HAL_CHANNEL_5 = 5, /*!< PWM channel 5 (only match) */
	PWM_HAL_CHANNEL_6 = 6  /*!< PWM channel 6 (only match) */
} pwm_hal_channel_t;

/*! @brief Interrupt type */
typedef enum {
	PWM_HAL_INT_TYPE_MATCH = 0,
	PWM_HAL_INT_TYPE_CAPTURE = 1
} pwm_hal_int_type_t;

/*! @brief PWM Match Config Struct */
typedef struct {
	pwm_hal_channel_t matchChannel;	 /*!< Match channel, should be
										  in range from 0..6 */
	lFunctionalState_t intOnMatch;	 /*!< Interrupt On match */
	lFunctionalState_t stopOnMatch;	 /*!< Stop On match */
	lFunctionalState_t resetOnMatch; /*!< Reset On match */
} pwm_hal_match_cfg_t;

/*! @brief PWM Capture Config Struct */
typedef struct {
	pwm_hal_channel_t captureChannel; /*!< Capture channel, should be
											in range from 0..1 */
	lFunctionalState_t risingEdge;	  /*!< Caption rising edge */
	lFunctionalState_t fallingEdge;	  /*!< Caption falling edge */
	lFunctionalState_t intOnCaption;  /*!< Interrupt On caption */
} pwm_hal_capture_cfg_t;

/** @brief PWM update type */
typedef enum {
	PWM_HAL_MATCH_UPDATE_NOW = 0,	  /*!< PWM Match Channel Update Now */
	PWM_HAL_MATCH_UPDATE_NEXT_RST = 1 /*!< PWM Match Channel Update on next
										   PWM Counter resetting */
} pwm_hal_match_update_t;

/** Interrupt flag for PWM match channel */
#define HAL_PWM_IR_PWMMRn(n) ((uint32_t)((n < 4) ? (1 << n) : (1 << (n + 4))))
/** Interrupt flag for capture input */
#define HAL_PWM_IR_PWMCAPn(n) ((uint32_t)(1 << (n + 4)))

/*! @brief Channel mode */
typedef enum {
	PWM_HAL_MODE_SINGLE_EDGE = 0,
	PWM_HAL_MODE_DUAL_EDGE = 1
} pwm_hal_channel_mode_t;

/*! @brief Prescale option */
typedef enum {
	PWM_HAL_PRESCALE_OPTION_TICKVAL = 0,
	PWM_HAL_PRESCALE_OPTION_USVAL = 1
} pwm_hal_prescale_option_t;

/*! @brief PWM Timer Config Struct */
typedef struct {
	pwm_hal_prescale_option_t prescaleOption;
	uint32_t prescaleValue;
} pwm_hal_cfg_t;

typedef void (*pwm_hal_callback_t)(pwm_hal_channel_t ch,
								   pwm_hal_int_type_t type);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 *  @brief Initialize PWM
 *
 *  @param[in] instance PWM Instance
 *  @param[in] pwmCfg PWM config struct
 *  @return None
 */
void HAL_PWM_Init(pwm_hal_instance_t instance, pwm_hal_cfg_t *pwmCfg);

/**
 *  @brief Deinitialize PWM
 *
 *  @param[in] instance PWM instance
 *  @return None
 */
void HAL_PWM_DeInit(pwm_hal_instance_t instance);

/**
 *  @brief Activate PWM
 *
 *  @param[in] instance PWM instance
 *  @return None
 */
void HAL_PWM_Activate(pwm_hal_instance_t instance);

/**
 *  @brief Initialize PWM Config struct to default values
 *
 *  @param[in] instance PWM instance
 *  @param[in] pwmCfg PWM config struct
 *  @return None
 */
void HAL_PWM_ConfigStructInit(pwm_hal_instance_t instance,
							  pwm_hal_cfg_t *pwmCfg);

/**
 *  @brief Configure PWM match
 *
 *  @param[in] instance PWM instance
 *  @param[in] matchCfg PWM match config struct
 *  @return None
 */
void HAL_PWM_ConfigMatch(pwm_hal_instance_t instance,
						 pwm_hal_match_cfg_t *matchCfg);

/**
 *  @brief Configure PWM capture
 *
 *  @param[in] instance PWM instance
 *  @param[in] captureCfg PWM capture config struct
 *  @return None
 */
void HAL_PWM_ConfigCapture(pwm_hal_instance_t instance,
						   pwm_hal_capture_cfg_t *captureCfg);

/**
 *  @brief Function for enabling interrupt
 *
 *  @param[in] instance PWM instance
 *  @param[in] callback Pointer to the callback function
 *  @return None
 */
void HAL_PWM_EnableInterrupt(pwm_hal_instance_t instance,
							 pwm_hal_callback_t callback);

/**
 * @brief Function for enable/disable PWM match channel
 *
 * @param[in] instance PWM instance
 * @param[in] channel Match channel
 * @param[in] state New state
 * @return None
 */
void HAL_PWM_MatchChannelCmd(pwm_hal_instance_t instance,
							 pwm_hal_channel_t channel,
							 lFunctionalState_t state);

/**
 *  @brief Configure PWM match channel
 *
 *  @param[in] instance PWM instance
 *  @param[in] channel PWM match channel
 * Note: PWM Channel 1 can not be selected for mode option (always single edge)
 *  @param[in] mode PWM channel mode
 *  @return None
 */
void HAL_PWM_ConfigMatchChannel(pwm_hal_instance_t instance,
								pwm_hal_channel_t channel,
								pwm_hal_channel_mode_t mode);

/**
 * @brief Function for resetting counter
 *
 * @param[in] instance PWM instance
 * @return None
 */
void HAL_PWM_ResetCounter(pwm_hal_instance_t instance);

/**
 *  @brief Update PWM match channel value
 *
 *  @param[in] instance PWM instance
 *  @param[in] channel PWM match channel
 *  @param[in] matchValue PWM match value
 *  @param[in] matchUpdate PWM match update type
 *  @return None
 */
void HAL_PWM_MatchUpdate(pwm_hal_instance_t instance, pwm_hal_channel_t channel,
						 uint32_t matchValue,
						 pwm_hal_match_update_t matchUpdate);

/**
 *  @brief Read value of PWM capture register
 *
 *  @param[in] instance PWM instance
 *  @param[in] channel PWM capture channel
 *  @return Value of capture register
 */
uint32_t HAL_PWM_GetCaptureValue(pwm_hal_instance_t instance,
								 pwm_hal_channel_t channel);

#ifdef __cplusplus
}
#endif

#endif /* PWM_H */