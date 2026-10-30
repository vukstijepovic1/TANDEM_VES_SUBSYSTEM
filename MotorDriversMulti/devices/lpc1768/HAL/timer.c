/**
 *	@file     timer.c
 *  @brief    HAL Timer library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "timer.h"

#include <assert.h>

#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_TIM_INSTANCE_CNT 4U

#define HAL_TIM_IRQN_OFFSET 1U

#define HAL_TIM_INSTANCE_TO_IRQN(instance) (instance + HAL_TIM_IRQN_OFFSET)

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

void *prvTIM_CALLBACK[HAL_TIM_INSTANCE_CNT];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static inline LPC_TIM_TypeDef *prvTIM_GetInstance(tim_hal_instance_t instance);

static void prvTIM_HandleInterrupt(tim_hal_instance_t instance,
								   tim_hal_callback_t user_cb);

static void prvTIM_InitPinsCapture(tim_hal_instance_t instance,
								   tim_hal_ch_t channel);
/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_TIM_Init(tim_hal_instance_t instance, tim_hal_cfg_t *cfg)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);

	LPC_TIM_TypeDef *TIMx = NULL;
	TIM_TIMERCFG_Type tim_cfg;
	tim_cfg.PrescaleOption = cfg->Prescale;
	tim_cfg.PrescaleValue = cfg->PrescaleValue;

	TIMx = prvTIM_GetInstance(instance);

	if (TIMx != NULL) {
		TIM_Init(TIMx, TIM_TIMER_MODE, &tim_cfg);
		TIM_Cmd(TIMx, ENABLE);
	}
}

void HAL_TIM_DeInit(tim_hal_instance_t instance)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);

	LPC_TIM_TypeDef *TIMx = NULL;

	TIMx = prvTIM_GetInstance(instance);

	if (TIMx != NULL) {
		TIM_Cmd(TIMx, DISABLE);
		NVIC_DisableIRQ(HAL_TIM_INSTANCE_TO_IRQN(instance));
	}
}

void HAL_TIM_MatchCfg(tim_hal_instance_t instance, tim_hal_match_cfg_t *cfg)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);

	TIM_MATCHCFG_Type match_cfg;
	LPC_TIM_TypeDef *TIMx = NULL;

	match_cfg.MatchChannel = cfg->MatchChannel;
	match_cfg.IntOnMatch = ENABLE;
	match_cfg.StopOnMatch = cfg->StopOnMatch;
	match_cfg.ResetOnMatch = cfg->ResetOnMatch;
	match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	match_cfg.MatchValue = cfg->MatchValue;

	TIMx = prvTIM_GetInstance(instance);

	TIM_ConfigMatch(TIMx, &match_cfg);
}

void HAL_TIM_EnableInterrupt(tim_hal_instance_t instance, tim_hal_callback_t cb)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);
	assert(cb != NULL);

	prvTIM_CALLBACK[instance] = cb;

	NVIC_SetPriority(HAL_TIM_INSTANCE_TO_IRQN(instance),
					 HAL_TIMER_NVIC_PRIORITY);
	NVIC_EnableIRQ(HAL_TIM_INSTANCE_TO_IRQN(instance));
}

uint32_t HAL_TIM_GetVal(tim_hal_instance_t instance)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);

	LPC_TIM_TypeDef *TIMx = NULL;
	uint32_t tim_val = 0;

	TIMx = prvTIM_GetInstance(instance);

	if (TIMx != NULL)
		tim_val = TIMx->TC;

	return tim_val;
}

void HAL_TIM_ConfigCapture(tim_hal_instance_t instance,
						   tim_hal_capture_cfg_t *captureCfg)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);
	assert(captureCfg->captureChannel < TIM_HAL_CH_2);

	LPC_TIM_TypeDef *lpcInstance = NULL;
	TIM_CAPTURECFG_Type TIMCaptureCfg;

	TIMCaptureCfg.CaptureChannel = captureCfg->captureChannel;
	TIMCaptureCfg.RisingEdge = captureCfg->risingEdge;
	TIMCaptureCfg.FallingEdge = captureCfg->fallingEdge;
	TIMCaptureCfg.IntOnCaption = captureCfg->intOnCaption;

	lpcInstance = prvTIM_GetInstance(instance);

	prvTIM_InitPinsCapture(instance, captureCfg->captureChannel);

	TIM_ConfigCapture(lpcInstance, &TIMCaptureCfg);
}

uint32_t HAL_TIM_GetCaptureValue(tim_hal_instance_t instance,
								 tim_hal_ch_t channel)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);
	assert(channel < TIM_HAL_CH_2);

	LPC_PWM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvTIM_GetInstance(instance);

	return TIM_GetCaptureValue(lpcInstance, channel);
}

void HAL_TIM_ResetCounter(tim_hal_instance_t instance)
{
	assert(instance < HAL_TIM_INSTANCE_CNT);
	LPC_TIM_TypeDef *lpcInstance = NULL;

	lpcInstance = prvTIM_GetInstance(instance);

	TIM_ResetCounter(lpcInstance);
}

/************************************ IRQs ************************************/

/**
 *  @brief Timer 0 IRQ Handler.
 */
void TIMER0_IRQHandler(void)
{
	prvTIM_HandleInterrupt(TIM_HAL_INSTANCE_0, prvTIM_CALLBACK[0]);
}

/**
 *  @brief Timer 1 IRQ Handler.
 */
void TIMER1_IRQHandler(void)
{
	prvTIM_HandleInterrupt(TIM_HAL_INSTANCE_1, prvTIM_CALLBACK[1]);
}

/**
 *  @brief Timer 2 IRQ Handler.
 */
void TIMER2_IRQHandler(void)
{
	prvTIM_HandleInterrupt(TIM_HAL_INSTANCE_2, prvTIM_CALLBACK[2]);
}

/**
 *  @brief Timer 3 IRQ Handler.
 */
void TIMER3_IRQHandler(void)
{
	prvTIM_HandleInterrupt(TIM_HAL_INSTANCE_3, prvTIM_CALLBACK[3]);
}

/****************************** static functions ******************************/

/**
 *  @brief Get LPC Timer instance.
 *
 *  @param[in] instance timer.
 *  @return LPC Timer instance.
 */
static inline LPC_TIM_TypeDef *prvTIM_GetInstance(tim_hal_instance_t instance)
{
	LPC_TIM_TypeDef *TIMx = NULL;

	switch (instance) {
	case TIM_HAL_INSTANCE_0:
		TIMx = LPC_TIM0;
		break;
	case TIM_HAL_INSTANCE_1:
		TIMx = LPC_TIM1;
		break;
	case TIM_HAL_INSTANCE_2:
		TIMx = LPC_TIM2;
		break;
	case TIM_HAL_INSTANCE_3:
		TIMx = LPC_TIM3;
		break;
	default:
		break;
	}

	return TIMx;
}

/**
 *  @brief Handle Timer Interrupt.
 *
 *  @param[in] instance Timer Instance.
 *  @param[in] user_cb User Callback.
 *  @return None.
 */
static void prvTIM_HandleInterrupt(tim_hal_instance_t instance,
								   tim_hal_callback_t user_cb)
{
	LPC_TIM_TypeDef *TIMx = NULL;

	TIMx = prvTIM_GetInstance(instance);

	if (TIM_GetIntStatus(TIMx, TIM_MR0_INT) == SET) {
		if (user_cb != NULL)
			user_cb(TIM_HAL_CH_0, TIM_HAL_INT_TYPE_MATCH);

		TIM_ClearIntPending(TIMx, TIM_MR0_INT);
	}

	if (TIM_GetIntStatus(TIMx, TIM_MR1_INT) == SET) {
		if (user_cb != NULL)
			user_cb(TIM_HAL_CH_1, TIM_HAL_INT_TYPE_MATCH);

		TIM_ClearIntPending(TIMx, TIM_MR1_INT);
	}

	if (TIM_GetIntStatus(TIMx, TIM_MR2_INT) == SET) {
		if (user_cb != NULL)
			user_cb(TIM_HAL_CH_2, TIM_HAL_INT_TYPE_MATCH);

		TIM_ClearIntPending(TIMx, TIM_MR2_INT);
	}

	if (TIM_GetIntStatus(TIMx, TIM_MR3_INT) == SET) {
		if (user_cb != NULL)
			user_cb(TIM_HAL_CH_3, TIM_HAL_INT_TYPE_MATCH);

		TIM_ClearIntPending(TIMx, TIM_MR3_INT);
	}

	if (TIM_GetIntStatus(TIMx, TIM_CR0_INT) == SET) {
		if (user_cb != NULL)
			user_cb(TIM_HAL_CH_0, TIM_HAL_INT_TYPE_CAPTURE);

		TIM_ClearIntPending(TIMx, TIM_CR0_INT);
	}

	if (TIM_GetIntStatus(TIMx, TIM_CR1_INT) == SET) {
		if (user_cb != NULL)
			user_cb(TIM_HAL_CH_1, TIM_HAL_INT_TYPE_CAPTURE);

		TIM_ClearIntPending(TIMx, TIM_CR1_INT);
	}
}

static void prvTIM_InitPinsCapture(tim_hal_instance_t instance,
								   tim_hal_ch_t channel)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t funcNum = PINSEL_FUNC_3; // Can only be alt_fun3
	uint8_t portNum;
	uint8_t pinNum;

	switch (instance) {
	case TIM_HAL_INSTANCE_0: {
		if (channel == TIM_HAL_CH_0) /* CAP0.0 */
		{
			portNum = PINSEL_PORT_1;
			pinNum = PINSEL_PIN_26;
		} else if (channel == TIM_HAL_CH_1) { /* CAP0.1 */
			portNum = PINSEL_PORT_1;
			pinNum = PINSEL_PIN_27;
		}
		break;
	}
	case TIM_HAL_INSTANCE_1: {
		if (channel == TIM_HAL_CH_0) /* CAP1.0 */
		{
			portNum = PINSEL_PORT_1;
			pinNum = PINSEL_PIN_18;
		} else if (channel == TIM_HAL_CH_1) { /* CAP1.1 */
			portNum = PINSEL_PORT_1;
			pinNum = PINSEL_PIN_19;
		}
		break;
	}
	case TIM_HAL_INSTANCE_2: {
		if (channel == TIM_HAL_CH_0) /* CAP2.0 */
		{
			portNum = PINSEL_PORT_0;
			pinNum = PINSEL_PIN_4;
		} else if (channel == TIM_HAL_CH_1) { /* CAP2.1 */
			portNum = PINSEL_PORT_0;
			pinNum = PINSEL_PIN_5;
		}
		break;
	}
	case TIM_HAL_INSTANCE_3: {
		if (channel == TIM_HAL_CH_0) /* CAP3.0 */
		{
			portNum = PINSEL_PORT_0;
			pinNum = PINSEL_PIN_23;
		} else if (channel == TIM_HAL_CH_1) { /* CAP3.1 */
			portNum = PINSEL_PORT_0;
			pinNum = PINSEL_PIN_24;
		}
		break;
	}

	default:
		break;
	}

	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Funcnum = funcNum;
	PinCfg.Portnum = portNum;
	PinCfg.Pinnum = pinNum;
	PINSEL_ConfigPin(&PinCfg);
}
/********************************* End Of File ********************************/