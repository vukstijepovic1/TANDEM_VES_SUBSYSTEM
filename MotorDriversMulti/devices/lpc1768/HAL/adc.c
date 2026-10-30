/**
 *	@file     adc.c
 *  @brief    HAL ADC library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "adc.h"

#include <assert.h>

#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

#include "util.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_ADC_IRQN_OFFSET 22
#define HAL_ADC_INSTANCE_TO_IRQN(instance) (instance + HAL_ADC_IRQN_OFFSET)

#define HAL_ADC_INSTANCE_CNT 1

/*******************************************************************************
 * Variables
 ******************************************************************************/

adc_hal_callback_t prvADC_CALLBACK;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static inline LPC_ADC_TypeDef *prvADC_GetInstance(adc_hal_instance_t instance);

static void prvADC_InitPins(adc_hal_instance_t instance,
							adc_hal_channel_t channel);

/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_ADC_Init(adc_hal_instance_t instance, uint32_t rate)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	ADC_Init(lpcInstance, rate);
}

void HAL_ADC_DeInit(adc_hal_instance_t instance)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	ADC_DeInit(lpcInstance);
}

void HAL_ADC_ChannelConfig(adc_hal_instance_t instance,
						   adc_hal_channel_t channel)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	prvADC_InitPins(instance, channel);

	ADC_ChannelCmd(lpcInstance, channel, ENABLE);
}

void HAL_ADC_ChannelCmd(adc_hal_instance_t instance, adc_hal_channel_t channel,
						lFunctionalState_t state)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	ADC_ChannelCmd(lpcInstance, channel, state);
}

void HAL_ADC_StartCmd(adc_hal_instance_t instance,
					  adc_hal_start_mode_t startMode)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	ADC_StartCmd(lpcInstance, startMode);
}

void HAL_ADC_BurstCmd(adc_hal_instance_t instance, adc_hal_channel_t channel,
					  lFunctionalState_t state)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	ADC_BurstCmd(lpcInstance, state);
}

void HAL_ADC_EnableInterrupt(adc_hal_instance_t instance,
							 adc_hal_callback_t callback,
							 adc_hal_channel_t channel)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	prvADC_CALLBACK = callback;
	ADC_IntConfig(lpcInstance, channel, ENABLE);
	NVIC_SetPriority(HAL_ADC_INSTANCE_TO_IRQN(instance), HAL_ADC_NVIC_PRIORITY);
	NVIC_EnableIRQ(HAL_ADC_INSTANCE_TO_IRQN(instance));
}

void HAL_ADC_IntCmd(adc_hal_instance_t instance, lFunctionalState_t state)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	if (state == lFunctionalState_Enable)
		NVIC_EnableIRQ(HAL_ADC_INSTANCE_TO_IRQN(instance));
	else
		NVIC_DisableIRQ(HAL_ADC_INSTANCE_TO_IRQN(instance));
}

uint16_t HAL_ADC_GetData(adc_hal_instance_t instance, adc_hal_channel_t channel)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	return ADC_ChannelGetData(lpcInstance, channel);
}

lState_t HAL_ADC_GetChannelStatus(adc_hal_instance_t instance,
								  adc_hal_channel_t channel,
								  adc_hal_data_status_t statusType)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	LPC_ADC_TypeDef *lpcInstance = prvADC_GetInstance(instance);

	return ADC_ChannelGetStatus(lpcInstance, channel, statusType);
}

lStatus_t HAL_ADC_PollData(adc_hal_instance_t instance,
						   adc_hal_channel_t channel, uint16_t *data,
						   uint32_t timeoutMs)
{
	assert(instance < HAL_ADC_INSTANCE_CNT);

	uint64_t time0 = HAL_GetTimeUS();

	HAL_ADC_ChannelCmd(instance, channel, lFunctionalState_Enable);

	HAL_ADC_StartCmd(instance, ADC_HAL_START_MODE_NOW);

	while (1) {
		if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000)) {
			HAL_ADC_ChannelCmd(instance, channel, lFunctionalState_Disable);
			return lStatus_Timeout;
		}

		if (HAL_ADC_GetChannelStatus(instance, channel,
									 ADC_HAL_DATA_STATUS_DONE) == lState_Set) {
			*data = HAL_ADC_GetData(instance, channel);
			break;
		}
	}

	HAL_ADC_ChannelCmd(instance, channel, lFunctionalState_Disable);
	return lStatus_Success;
}

/************************************ IRQs ************************************/

void ADC_IRQHandler(void)
{
	if (prvADC_CALLBACK != NULL)
		prvADC_CALLBACK(ADC_HAL_INSTANCE_0);
}

/****************************** static functions ******************************/

static void prvADC_InitPins(adc_hal_instance_t instance,
							adc_hal_channel_t channel)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t funcNum = PINSEL_FUNC_0;
	uint8_t portNum = PINSEL_PORT_0;
	uint8_t pinNum = PINSEL_PIN_0;

	switch (channel) {
	case ADC_CHANNEL_0:
		funcNum = PINSEL_FUNC_1;
		portNum = PINSEL_PORT_0;
		pinNum = PINSEL_PIN_23;
		break;
	case ADC_CHANNEL_1:
		funcNum = PINSEL_FUNC_1;
		portNum = PINSEL_PORT_0;
		pinNum = PINSEL_PIN_24;
		break;
	case ADC_CHANNEL_2:
		funcNum = PINSEL_FUNC_1;
		portNum = PINSEL_PORT_0;
		pinNum = PINSEL_PIN_25;
		break;
	case ADC_CHANNEL_3:
		funcNum = PINSEL_FUNC_1;
		portNum = PINSEL_PORT_0;
		pinNum = PINSEL_PIN_26;
		break;
	case ADC_CHANNEL_4:
		funcNum = PINSEL_FUNC_3;
		portNum = PINSEL_PORT_1;
		pinNum = PINSEL_PIN_30;
		break;
	case ADC_CHANNEL_5:
		funcNum = PINSEL_FUNC_3;
		portNum = PINSEL_PORT_1;
		pinNum = PINSEL_PIN_31;
		break;
	case ADC_CHANNEL_6:
		funcNum = PINSEL_FUNC_2;
		portNum = PINSEL_PORT_0;
		pinNum = PINSEL_PIN_3;
		break;
	case ADC_CHANNEL_7:
		funcNum = PINSEL_FUNC_2;
		portNum = PINSEL_PORT_0;
		pinNum = PINSEL_PIN_2;
		break;
	}

	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Funcnum = funcNum;
	PinCfg.Portnum = portNum;
	PinCfg.Pinnum = pinNum;
	PINSEL_ConfigPin(&PinCfg);
}

static inline LPC_ADC_TypeDef *prvADC_GetInstance(adc_hal_instance_t instance)
{
	LPC_ADC_TypeDef *lpcInstance = NULL;

	switch (instance) {
	case ADC_HAL_INSTANCE_0:
		lpcInstance = (LPC_ADC_TypeDef *)LPC_ADC;
		break;
	}

	return lpcInstance;
}

/* --------------------------------- End Of File -----------------------------*/