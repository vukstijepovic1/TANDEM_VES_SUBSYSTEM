/**
 *	@file     gpio.c
 *  @brief    GPIO Hardware Abstraction Layer.
 *  @details  v1.2
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "gpio.h"

#include <assert.h>

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define GPIO_HAL_INSTANCE_0_INT_MASK 0x7FFFFFFF //!< GPIO0 Interrupt Mask
#define GPIO_HAL_INSTANCE_2_INT_MASK 0x00003FFF //!< GPIO2 Interrupt Mask

#define GPIO_HAL_INT_SOURCE_INSTANCE_0 0x01 //!< Interrupt Source is GPIO0
#define GPIO_HAL_INT_SOURCE_INSTANCE_2 0x04 //!< Interrupt Source is GPIO2

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

// GPIO Interrupt Callbacks
void *prvGPIO_INSTANCE_0_CALLBACK; //!< GPIO0 Interrupt Callback
void *prvGPIO_INSTANCE_2_CALLBACK; //!< GPIO2 Interrupt Callback

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Clear all GPIO interrupts */
static void prvGPIO_ClearAllInterrupts(gpio_hal_instance_t instance);

/*******************************************************************************
 * Code
 ******************************************************************************/

lStatus_t HAL_GPIO_Init(gpio_hal_instance_t instance, uint8_t pinNum,
						gpio_hal_cfg_type_t *gpioPin)
{
	PINSEL_CFG_Type pinCfg;

	pinCfg.Portnum = instance;
	pinCfg.Pinnum = pinNum;
	pinCfg.Funcnum = PINSEL_FUNC_0;
	pinCfg.OpenDrain = gpioPin->openDrain;
	PINSEL_ConfigPin(&pinCfg);
	GPIO_SetDir(instance, 1 << pinNum, gpioPin->pinDir);
	GPIO_ClearValue(instance, 1 << pinNum);

	return lStatus_Success;
}

void HAL_GPIO_SetPinValueByMask(gpio_hal_instance_t instance, uint32_t mask,
								gpio_hal_pinvalue_t value)
{
	if (value == GPIO_HAL_RESET)
		GPIO_ClearValue(instance, mask);
	else
		GPIO_SetValue(instance, mask);
}

void HAL_GPIO_SetPinValue(gpio_hal_instance_t instance, uint8_t pinNum,
						  gpio_hal_pinvalue_t value)
{
	if (value == GPIO_HAL_RESET)
		GPIO_ClearValue(instance, 1 << pinNum);
	else
		GPIO_SetValue(instance, 1 << pinNum);
}

uint32_t HAL_GPIO_GetPinValue(gpio_hal_instance_t instance, uint8_t pinNum)
{
	return ((GPIO_ReadValue(instance) >> pinNum) & 0x01);
}

lStatus_t HAL_GPIO_ConfigureCallback(gpio_hal_instance_t instance,
									 gpio_hal_callback_t callback)
{
	assert(instance == GPIO_HAL_INSTANCE_0 || instance == GPIO_HAL_INSTANCE_2);

	if (instance == GPIO_HAL_INSTANCE_0)
		prvGPIO_INSTANCE_0_CALLBACK = callback;
	else if (instance == GPIO_HAL_INSTANCE_2)
		prvGPIO_INSTANCE_2_CALLBACK = callback;

	prvGPIO_ClearAllInterrupts(instance);

	HAL_NVIC_SetPriority(EINT3_IRQn, GPIO_HAL_DEF_INT_PRIO);
	HAL_NVIC_EnableIRQ(EINT3_IRQn);
}

lStatus_t HAL_GPIO_EnableInterrupt(gpio_hal_instance_t instance,
								   uint32_t pinNum,
								   gpio_hal_edgestate_t edgeState)
{
	assert(instance == GPIO_HAL_INSTANCE_0 || instance == GPIO_HAL_INSTANCE_2);

	if ((instance == GPIO_HAL_INSTANCE_0) &&
		(edgeState == GPIO_HAL_EDGESTATE_RISING))
		LPC_GPIOINT->IO0IntEnR |= (1 << pinNum);
	else if ((instance == GPIO_HAL_INSTANCE_2) &&
			 (edgeState == GPIO_HAL_EDGESTATE_RISING))
		LPC_GPIOINT->IO2IntEnR |= (1 << pinNum);
	else if ((instance == GPIO_HAL_INSTANCE_0) &&
			 (edgeState == GPIO_HAL_EDGESTATE_FALLING))
		LPC_GPIOINT->IO0IntEnF |= (1 << pinNum);
	else if ((instance == GPIO_HAL_INSTANCE_2) &&
			 (edgeState == GPIO_HAL_EDGESTATE_FALLING))
		LPC_GPIOINT->IO2IntEnF |= (1 << pinNum);

	return lStatus_Success;
}

lStatus_t HAL_GPIO_DisableInterrupt(gpio_hal_instance_t instance,
									uint32_t pinNum,
									gpio_hal_edgestate_t edgeState)
{
	assert(instance == GPIO_HAL_INSTANCE_0 || instance == GPIO_HAL_INSTANCE_2);

	if ((instance == GPIO_HAL_INSTANCE_0) &&
		(edgeState == GPIO_HAL_EDGESTATE_RISING))
		LPC_GPIOINT->IO0IntEnR &= ~(1 << pinNum);
	else if ((instance == GPIO_HAL_INSTANCE_2) &&
			 (edgeState == GPIO_HAL_EDGESTATE_RISING))
		LPC_GPIOINT->IO2IntEnR &= ~(1 << pinNum);
	else if ((instance == GPIO_HAL_INSTANCE_0) &&
			 (edgeState == GPIO_HAL_EDGESTATE_FALLING))
		LPC_GPIOINT->IO0IntEnF &= ~(1 << pinNum);
	else if ((instance == GPIO_HAL_INSTANCE_2) &&
			 (edgeState == GPIO_HAL_EDGESTATE_FALLING))
		LPC_GPIOINT->IO2IntEnF &= ~(1 << pinNum);

	return lStatus_Success;
}

lFunctionalState_t HAL_GPIO_GetIntStatus(gpio_hal_instance_t instance,
										 uint32_t pinNum,
										 gpio_hal_edgestate_t edgestate)
{
	assert(instance == GPIO_HAL_INSTANCE_0 || instance == GPIO_HAL_INSTANCE_2);

	return (lFunctionalState_t)GPIO_GetIntStatus(
		(const uint8_t)instance, pinNum, (const uint8_t)edgestate);
}

void HAL_GPIO_ClearInt(gpio_hal_instance_t instance, const uint32_t pinNum)
{
	assert(instance == GPIO_HAL_INSTANCE_0 || instance == GPIO_HAL_INSTANCE_2);

	GPIO_ClearInt((const uint8_t)instance, (1 << pinNum));
}

/************************************ IRQs ************************************/

void EINT3_IRQHandler(void)
{
	gpio_hal_callback_t userCallback;

	if (LPC_GPIOINT->IntStatus & GPIO_HAL_INT_SOURCE_INSTANCE_0) {
		uint32_t lineStatus;
		uint8_t cnt;

		userCallback = prvGPIO_INSTANCE_0_CALLBACK;
		lineStatus = LPC_GPIOINT->IO0IntStatR | LPC_GPIOINT->IO0IntStatF;

		if (userCallback == NULL) {
			/* clear all interrupts in case there is no callbacks */
			LPC_GPIOINT->IO0IntClr = lineStatus;
		} else {
			cnt = 0;
			while (lineStatus) {
				if (lineStatus & 0x01) {
					userCallback(cnt);
					HAL_GPIO_ClearInt(GPIO_HAL_INSTANCE_0, cnt);
				}

				/** TODO: Optimize for speed in future */
				cnt++;
				lineStatus = lineStatus >> 1;
			}
		}
	}

	if (LPC_GPIOINT->IntStatus & GPIO_HAL_INT_SOURCE_INSTANCE_2) {
		uint32_t lineStatus;
		uint8_t cnt;

		userCallback = prvGPIO_INSTANCE_2_CALLBACK;
		lineStatus = LPC_GPIOINT->IO2IntStatR | LPC_GPIOINT->IO2IntStatF;

		if (userCallback == NULL) {
			/* clear all interrupts in case there is no callbacks */
			LPC_GPIOINT->IO2IntClr = lineStatus;

		} else {
			cnt = 0;
			while (lineStatus) {
				if (lineStatus & 0x01) {
					userCallback(cnt);
					HAL_GPIO_ClearInt(GPIO_HAL_INSTANCE_2, cnt);
				}

				cnt++;
				lineStatus = lineStatus >> 1;
			}
		}
	}
}

/****************************** static functions ******************************/

static void prvGPIO_ClearAllInterrupts(gpio_hal_instance_t instance)
{
	if (instance == GPIO_HAL_INSTANCE_0)
		GPIO_ClearInt(instance, GPIO_HAL_INSTANCE_0_INT_MASK);

	if (instance == GPIO_HAL_INSTANCE_2)
		GPIO_ClearInt(instance, GPIO_HAL_INSTANCE_2_INT_MASK);
}