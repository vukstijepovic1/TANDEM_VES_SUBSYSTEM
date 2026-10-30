/**
 *	@file     pwm.c
 *  @brief    HAL PWM library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "pwm.h"

#include <assert.h>

#include "lpc17xx_pinsel.h"
#include "lpc17xx_pwm.h"

#include "util.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_PWM_IRQN_OFFSET 9
#define HAL_PWM_INSTANCE_TO_IRQN(instance) (instance + HAL_PWM_IRQN_OFFSET)

#define HAL_PWM_INSTANCE_CNT 1

/*******************************************************************************
 * Variables
 ******************************************************************************/

pwm_hal_callback_t prvPWM_CALLBACK;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static inline LPC_PWM_TypeDef *prvPWM_GetInstance(pwm_hal_instance_t instance);

static void prvPWM_InitPinsMatch(pwm_hal_instance_t instance,
								 pwm_hal_channel_t channel);

static void prvPWM_InitPinsCapture(pwm_hal_instance_t instance,
								   pwm_hal_channel_t channel);

static void prvPWM_HandleInterrupt(pwm_hal_instance_t instance,
								   pwm_hal_callback_t user_cb);

/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_PWM_Init(pwm_hal_instance_t instance, pwm_hal_cfg_t *pwmCfg)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;
	PWM_TIMERCFG_Type timerCfg;

	lpcInstance = prvPWM_GetInstance(instance);

	timerCfg.PrescaleOption = pwmCfg->prescaleOption;
	timerCfg.PrescaleValue = pwmCfg->prescaleValue;

	PWM_Init(lpcInstance, PWM_MODE_TIMER, (void *)(&timerCfg));
}

void HAL_PWM_DeInit(pwm_hal_instance_t instance)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	PWM_CounterCmd(lpcInstance, DISABLE);
	PWM_Cmd(lpcInstance, DISABLE);

	PWM_DeInit(lpcInstance);
}

void HAL_PWM_Activate(pwm_hal_instance_t instance)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	PWM_ResetCounter(lpcInstance);
	PWM_CounterCmd(lpcInstance, ENABLE);
	PWM_Cmd(lpcInstance, ENABLE);
}

void HAL_PWM_ConfigStructInit(pwm_hal_instance_t instance,
							  pwm_hal_cfg_t *pwmCfg)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;
	PWM_TIMERCFG_Type timerCfg;

	lpcInstance = prvPWM_GetInstance(instance);

	PWM_ConfigStructInit(PWM_MODE_TIMER, (void *)(&timerCfg));

	pwmCfg->prescaleOption = timerCfg.PrescaleOption;
	pwmCfg->prescaleValue = timerCfg.PrescaleValue;
}

void HAL_PWM_ConfigMatch(pwm_hal_instance_t instance,
						 pwm_hal_match_cfg_t *matchCfg)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;
	PWM_MATCHCFG_Type PWMMatchCfg;

	PWMMatchCfg.IntOnMatch = matchCfg->intOnMatch;
	PWMMatchCfg.MatchChannel = matchCfg->matchChannel;
	PWMMatchCfg.ResetOnMatch = matchCfg->resetOnMatch;
	PWMMatchCfg.StopOnMatch = matchCfg->stopOnMatch;

	lpcInstance = prvPWM_GetInstance(instance);

	if (matchCfg->matchChannel != PWM_HAL_CHANNEL_0)
		prvPWM_InitPinsMatch(instance, matchCfg->matchChannel);

	PWM_ConfigMatch(lpcInstance, &PWMMatchCfg);
}

void HAL_PWM_ConfigCapture(pwm_hal_instance_t instance,
						   pwm_hal_capture_cfg_t *captureCfg)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	assert(captureCfg->captureChannel < PWM_HAL_CHANNEL_2);
	LPC_PWM_TypeDef *lpcInstance = NULL;
	PWM_CAPTURECFG_Type PWMCaptureCfg;

	PWMCaptureCfg.CaptureChannel = captureCfg->captureChannel;
	PWMCaptureCfg.FallingEdge = captureCfg->fallingEdge;
	PWMCaptureCfg.IntOnCaption = captureCfg->intOnCaption;
	PWMCaptureCfg.RisingEdge = captureCfg->risingEdge;

	lpcInstance = prvPWM_GetInstance(instance);

	prvPWM_InitPinsCapture(instance, captureCfg->captureChannel);

	PWM_ConfigCapture(lpcInstance, &PWMCaptureCfg);
}

void HAL_PWM_EnableInterrupt(pwm_hal_instance_t instance,
							 pwm_hal_callback_t callback)
{
	NVIC_DisableIRQ(HAL_PWM_INSTANCE_TO_IRQN(instance));
	prvPWM_CALLBACK = callback;
	NVIC_SetPriority(HAL_PWM_INSTANCE_TO_IRQN(instance), HAL_PWM_NVIC_PRIORITY);
	NVIC_EnableIRQ(HAL_PWM_INSTANCE_TO_IRQN(instance));
}

void HAL_PWM_MatchChannelCmd(pwm_hal_instance_t instance,
							 pwm_hal_channel_t channel,
							 lFunctionalState_t state)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	PWM_ChannelCmd(lpcInstance, channel, state);
}

void HAL_PWM_ConfigMatchChannel(pwm_hal_instance_t instance,
								pwm_hal_channel_t channel,
								pwm_hal_channel_mode_t mode)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	PWM_ChannelConfig(lpcInstance, channel, mode);
}

void HAL_PWM_ResetCounter(pwm_hal_instance_t instance)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	PWM_ResetCounter(lpcInstance);
}

void HAL_PWM_MatchUpdate(pwm_hal_instance_t instance, pwm_hal_channel_t channel,
						 uint32_t matchValue,
						 pwm_hal_match_update_t matchUpdate)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	PWM_MatchUpdate(lpcInstance, channel, matchValue, matchUpdate);
}

uint32_t HAL_PWM_GetCaptureValue(pwm_hal_instance_t instance,
								 pwm_hal_channel_t channel)
{
	assert(instance < HAL_PWM_INSTANCE_CNT);
	assert(channel < PWM_HAL_CHANNEL_2);
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	return PWM_GetCaptureValue(lpcInstance, channel);
}

/************************************ IRQs ************************************/

void PWM1_IRQHandler(void)
{
	prvPWM_HandleInterrupt(PWM_HAL_INSTANCE_0, prvPWM_CALLBACK);
}

/****************************** static functions ******************************/

static inline LPC_PWM_TypeDef *prvPWM_GetInstance(pwm_hal_instance_t instance)
{
	LPC_PWM_TypeDef *lpcInstance = NULL;

	switch (instance) {
	case PWM_HAL_INSTANCE_0:
		lpcInstance = (LPC_PWM_TypeDef *)LPC_PWM1;
		break;
	}

	return lpcInstance;
}

static void prvPWM_InitPinsMatch(pwm_hal_instance_t instance,
								 pwm_hal_channel_t channel)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t funcNum = PINSEL_FUNC_2;
	uint8_t portNum = PINSEL_PORT_1;
	uint8_t pinNum = PINSEL_PIN_0;

	switch (channel) {
	case PWM_HAL_CHANNEL_1:
		pinNum = PINSEL_PIN_18;
		break;
	case PWM_HAL_CHANNEL_2:
		pinNum = PINSEL_PIN_20;
		break;
	case PWM_HAL_CHANNEL_3:
		pinNum = PINSEL_PIN_21;
		break;
	case PWM_HAL_CHANNEL_4:
		pinNum = PINSEL_PIN_23;
		break;
	case PWM_HAL_CHANNEL_5:
		pinNum = PINSEL_PIN_24;
		break;
	case PWM_HAL_CHANNEL_6:
		pinNum = PINSEL_PIN_26;
		break;
	}

	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Funcnum = funcNum;
	PinCfg.Portnum = portNum;
	PinCfg.Pinnum = pinNum;
	PINSEL_ConfigPin(&PinCfg);
}

static void prvPWM_InitPinsCapture(pwm_hal_instance_t instance,
								   pwm_hal_channel_t channel)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t funcNum = PINSEL_FUNC_2;
	uint8_t portNum = PINSEL_PORT_1;
	uint8_t pinNum = PINSEL_PIN_0;

	switch (channel) {
	case PWM_HAL_CHANNEL_0:
		pinNum = PINSEL_PIN_28;
		break;
	case PWM_HAL_CHANNEL_1:
		pinNum = PINSEL_PIN_29;
		break;
	}

	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Funcnum = funcNum;
	PinCfg.Portnum = portNum;
	PinCfg.Pinnum = pinNum;
	PINSEL_ConfigPin(&PinCfg);
}

static void prvPWM_HandleInterrupt(pwm_hal_instance_t instance,
								   pwm_hal_callback_t user_cb)
{
	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvPWM_GetInstance(instance);

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_MR0) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_0, PWM_HAL_INT_TYPE_MATCH);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_MR0);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_MR1) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_1, PWM_HAL_INT_TYPE_MATCH);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_MR1);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_MR2) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_2, PWM_HAL_INT_TYPE_MATCH);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_MR2);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_MR3) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_3, PWM_HAL_INT_TYPE_MATCH);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_MR3);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_MR4) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_4, PWM_HAL_INT_TYPE_MATCH);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_MR4);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_MR5) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_5, PWM_HAL_INT_TYPE_MATCH);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_MR5);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_MR6) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_6, PWM_HAL_INT_TYPE_MATCH);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_MR6);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_CAP0) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_0, PWM_HAL_INT_TYPE_CAPTURE);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_CAP0);
	}

	if (PWM_GetIntStatus(lpcInstance, PWM_INTSTAT_CAP1) == SET) {
		if (user_cb != NULL)
			user_cb(PWM_HAL_CHANNEL_1, PWM_HAL_INT_TYPE_CAPTURE);

		PWM_ClearIntPending(lpcInstance, PWM_INTSTAT_CAP1);
	}
}
/* --------------------------------- End Of File -----------------------------*/