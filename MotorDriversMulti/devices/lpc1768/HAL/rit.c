/**
 *	@file     rit.c
 *  @brief    HAL RIT library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "rit.h"

#include <assert.h>

#include "lpc17xx_pinsel.h"
#include "lpc17xx_rit.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_RIT_IRQN_OFFSET 29
#define HAL_RIT_INSTANCE_TO_IRQN(instance) (instance + HAL_RIT_IRQN_OFFSET)

#define HAL_RIT_INSTANCE_CNT 1

/*******************************************************************************
 * Variables
 ******************************************************************************/

rit_hal_callback_t prvRIT_CALLBACK;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static inline LPC_RIT_TypeDef *prvRIT_GetInstance(rit_hal_instance_t instance);

/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_RIT_Init(rit_hal_instance_t instance)
{
	assert(instance < HAL_RIT_INSTANCE_CNT);

	LPC_RIT_TypeDef *lpcInstance = prvRIT_GetInstance(instance);

	RIT_Init(lpcInstance);
}

void HAL_RIT_DeInit(rit_hal_instance_t instance)
{
	assert(instance < HAL_RIT_INSTANCE_CNT);

	LPC_RIT_TypeDef *lpcInstance = prvRIT_GetInstance(instance);

	RIT_DeInit(lpcInstance);
}

void HAL_RIT_TimerConfig(rit_hal_instance_t instance, uint32_t interval)
{
	assert(instance < HAL_RIT_INSTANCE_CNT);

	LPC_RIT_TypeDef *lpcInstance = prvRIT_GetInstance(instance);

	RIT_TimerConfig(lpcInstance, interval);
}

void HAL_RIT_EnableInterrupt(rit_hal_instance_t instance,
							 rit_hal_callback_t callback)
{
	assert(instance < HAL_RIT_INSTANCE_CNT);

	prvRIT_CALLBACK = callback;
	NVIC_SetPriority(HAL_RIT_INSTANCE_TO_IRQN(instance), HAL_RIT_NVIC_PRIORITY);
	NVIC_EnableIRQ(HAL_RIT_INSTANCE_TO_IRQN(instance));
}

void HAL_RIT_Cmd(rit_hal_instance_t instance, lFunctionalState_t state)
{
	assert(instance < HAL_RIT_INSTANCE_CNT);

	LPC_RIT_TypeDef *lpcInstance = prvRIT_GetInstance(instance);

	RIT_Cmd(lpcInstance, state);
}

lState_t HAL_Rit_GetIntStatus(rit_hal_instance_t instance)
{
	assert(instance < HAL_RIT_INSTANCE_CNT);

	LPC_RIT_TypeDef *lpcInstance = prvRIT_GetInstance(instance);

	return RIT_GetIntStatus(LPC_RIT);
}

/************************************ IRQs ************************************/

void RIT_IRQHandler(void)
{
	if (prvRIT_CALLBACK != NULL)
		prvRIT_CALLBACK(RIT_HAL_INSTANCE_0);
}

/****************************** static functions ******************************/

static inline LPC_RIT_TypeDef *prvRIT_GetInstance(rit_hal_instance_t instance)
{
	LPC_RIT_TypeDef *lpcInstance = NULL;

	switch (instance) {
	case RIT_HAL_INSTANCE_0:
		lpcInstance = (LPC_RIT_TypeDef *)LPC_RIT;
		break;
	}

	return lpcInstance;
}

/* --------------------------------- End Of File -----------------------------*/