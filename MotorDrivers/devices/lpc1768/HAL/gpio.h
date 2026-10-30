/**
 *	@file     gpio.h
 *  @brief    GPIO Hardware Abstraction Layer.
 *  @details  v1.2
 *  @author   LisumLab
 */

#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

#ifdef DEV_CONFIG
#include "dev_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef GPIO_HAL_DEF_INT_PRIO
/*! NVIC Priority */
#define GPIO_HAL_DEF_INT_PRIO 7
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief GPIO Instance. */
typedef enum {
	GPIO_HAL_INSTANCE_0 = 0,
	GPIO_HAL_INSTANCE_1 = 1,
	GPIO_HAL_INSTANCE_2 = 2,
	GPIO_HAL_INSTANCE_3 = 3,
	GPIO_HAL_INSTANCE_4 = 4
} gpio_hal_instance_t;

/*! @brief GPIO Pin Direction. */
typedef enum { GPIO_HAL_INPUT = 0, GPIO_HAL_OUTPUT = 1 } gpio_hal_pindir_t;

/*! @brief GPIO Pin Value. */
typedef enum { GPIO_HAL_RESET = 0, GPIO_HAL_SET = 1 } gpio_hal_pinvalue_t;

/*! @brief GPIO Pin Mode. */
typedef enum {
	GPIO_HAL_PINMODE_PULLUP = 0,
	GPIO_HAL_PINMODE_TRISTATE = 2,
	GPIO_HAL_PINMODE_PULLDOWN = 3
} gpio_hal_pinmode_t;

/*! @brief GPIO Pin Opendrain. */
typedef enum {
	GPIO_HAL_OPENDRAIN_NORMAL = 0,
	GPIO_HAL_OPENDRAIN_OPENDRAIN = 1
} gpio_hal_opendrain_t;

/*! @brief GPIO Pin Interrupt Edgestate. */
typedef enum {
	GPIO_HAL_EDGESTATE_RISING = 0,
	GPIO_HAL_EDGESTATE_FALLING = 1,
} gpio_hal_edgestate_t;

/*! @brief GPIO Pin Config. */
typedef struct {
	gpio_hal_pinmode_t pinMode;
	gpio_hal_opendrain_t openDrain;
	gpio_hal_pindir_t pinDir;
} gpio_hal_cfg_type_t;

/* GPIO interrupt callback. */
typedef void (*gpio_hal_callback_t)(uint16_t pinNum);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Function for GPIO initialization
 * @param instance: Port number - available ports: 0,1,2,3,4
 * @param pinNum: Pin number - available pins:
 *                - P0[30:0] Note: 1)
 *                - P1[31:0] Note: 2)
 *                - P2[13:0]
 *                - P3[26:25]
 *                - P4[29:28]
 *               Note: 1) P0[14:12] -> NOT avaliable pins
 *               Note: 2) P1[2], P1[3], P1[7:5], P1[13:11] -> NOT available pins
 * @param gpioPin: gpio_hal_cfg_type_t configurational struct
 * @retval ::lStatus_t
 *
 */
lStatus_t HAL_GPIO_Init(gpio_hal_instance_t instance, uint8_t pinNum,
						gpio_hal_cfg_type_t *gpioPin);

/**
 * @brief Function for setting value on GPIO pins by mask
 * @param instance: Port number - available ports: 0,1,2,3,4
 * @param mask: Binary mask that contains all bits to set
 * @param value: Value of set pin, GPIO_HAL_SET or GPIO_HAL_RESET
 * @return None
 */
void HAL_GPIO_SetPinValueByMask(gpio_hal_instance_t instance, uint32_t mask,
								gpio_hal_pinvalue_t value);

/**
 * @brief Function for setting value on GPIO pin
 * @param instance: Port number - available ports: 0,1,2,3,4
 * @param pinNum: Pin number - available pins:
 *                - P0[30:0] Note: 1)
 *                - P1[31:0] Note: 2)
 *                - P2[13:0]
 *                - P3[26:25]
 *                - P4[29:28]
 *               Note: 1) P0[14:12] -> NOT avaliable pins
 *               Note: 2) P1[2], P1[3], P1[7:5], P1[13:11] -> NOT available pins
 * @param value: Value of set pin, GPIO_HAL_SET or GPIO_HAL_RESET
 * @return None
 */
void HAL_GPIO_SetPinValue(gpio_hal_instance_t instance, uint8_t pinNum,
						  gpio_hal_pinvalue_t value);

/**
 * @brief Function to get value on pins of desired port
 * @param instance: Port number - available ports: 0,1,2,3,4
 * @param pinNum: Pin number.
 * @return Requested pin value.
 */
uint32_t HAL_GPIO_GetPinValue(gpio_hal_instance_t instance, uint8_t pinNum);

/**
 * @brief Function for specifying callback function on interrupt call
 * @param callback: Pointer to callback function that should be called when
 *                  interrupt is triggered on pins specified with mask parameter
 * @retval 	::lStatus_t
 */
lStatus_t HAL_GPIO_ConfigureCallback(gpio_hal_instance_t instance,
									 gpio_hal_callback_t callback);

/**
 * @brief Function for enabling interrupt
 * @param instance: Port number - available ports: 0,2
 * @param pinNum: Pin number - available pins:
 *                - P0[30:0]
 *                - P2[13:0]
 * @param edgestate: Edgestate -> Interrupt on falling or rising edge
 *                               - GPIO_HAL_EDGESTATE_RISING
 *                               - GPIO_HAL_EDGESTATE_FALLING
 * @retval 	::lStatus_t
 */
lStatus_t HAL_GPIO_EnableInterrupt(gpio_hal_instance_t instance,
								   uint32_t pinNum,
								   gpio_hal_edgestate_t edgeState);

/**
 * @brief Function for disabling interrupt
 * @param instance: Port number - available ports: 0,2
 * @param pinNum: Pin number - available pins:
 *                - P0[30:0]
 *                - P2[13:0]
 * @param edgestate: Edgestate -> Interrupt on falling or rising edge
 *                               - GPIO_HAL_EDGESTATE_RISING
 *                               - GPIO_HAL_EDGESTATE_FALLING
 * @retval 	::lStatus_t
 */
lStatus_t HAL_GPIO_DisableInterrupt(gpio_hal_instance_t instance,
									uint32_t pinNum,
									gpio_hal_edgestate_t edgeState);

/**
 * @brief Function to get interrupt status on specific port and pin
 * @param instance: Port number - available ports: 0,2
 * @param pinNum: Pin number - available pins:
 *                - P0[30:0]
 *                - P2[13:0]
 * @param edgestate: Edgestate -> Interrupt status on falling or rising edge
 *                               - GPIO_HAL_EDGESTATE_RISING
 *                               - GPIO_HAL_EDGESTATE_FALLING
 * @retval ::gpio_hal_func_state_t
 */
lFunctionalState_t HAL_GPIO_GetIntStatus(gpio_hal_instance_t instance,
										 uint32_t pinNum,
										 gpio_hal_edgestate_t edgestate);

/**
 * @brief Function that clears the interrupt on specific port and pin
 * @param instance: Port number - available ports: 0,2
 * @param pinNum: Number of pin which interrupt will be cleared.
 * @return None
 */
void HAL_GPIO_ClearInt(gpio_hal_instance_t instance, const uint32_t pinNum);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */