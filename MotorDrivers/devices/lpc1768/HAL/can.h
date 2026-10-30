/**
 *	@file     can.h
 *  @brief    CAN Hardware Abstraction Layer.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef CAN_H
#define CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdio.h>

#include "common.h"

#ifdef DEV_CONFIG
#include "dev_config.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef CAN_HAL_DEF_INT_PRIO
/*! NVIC Priority */
#define CAN_HAL_DEF_INT_PRIO 6
#endif

#ifndef HAL_CAN_1_PIN_OPTION
/*! CAN1 Pin option.
 *  HAL_CAN_1_PIN_OPTION = 0 -> CAN1 uses pins P0.0 & P0.1
 *  HAL_CAN_1_PIN_OPTION = 1 -> CAN1 uses pins P0.21 & P0.22 */
#define HAL_CAN_1_PIN_OPTION 0
#endif

#ifndef HAL_CAN_2_PIN_OPTION
/*! CAN2 Pin option.
 *  HAL_CAN_2_PIN_OPTION = 0 -> CAN2 uses pins P0.4 & P0.5
 *  HAL_CAN_2_PIN_OPTION = 1 -> CAN2 uses pins P2.7 & P2.8 */
#define HAL_CAN_2_PIN_OPTION 1
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief CAN Instance. */
typedef enum {
	CAN_HAL_INSTANCE_0 = 0, //!< First CAN Instance
	CAN_HAL_INSTANCE_1 = 1, //!< Second CAN Instance
} can_hal_instance_t;

/*! @brief CAN Modes. */
typedef enum {
	CAN_HAL_MODE_OPERATING = 0, //!< Operating Mode
	CAN_HAL_MODE_RESET,			//!< Reset Mode
	CAN_HAL_MODE_LISTENONLY,	//!< Listen-Only Mode
	CAN_HAL_MODE_SELFTEST,		//!< Self-Test Mode
	CAN_HAL_MODE_TXPRIORITY,	//!< Transmit Priority Mode
	CAN_HAL_MODE_SLEEP,			//!< Sleep Mode
	CAN_HAL_MODE_RXPOLARITY,	//!< Receive Polarity Mode
	CAN_HAL_MODE_TEST			//!< Test Mode
} can_hal_mode_type_t;

/*! @brief CAN ID Format. */
typedef enum {
	CAN_HAL_ID_FORMAT_STD = 0, //!< Message ID is 11 bit
	CAN_HAL_ID_FORMAT_EXT = 1  //!< Message ID is 29 bit
} can_hal_id_format_t;

/*! @brief CAN Frame Type. */
typedef enum {
	CAN_HAL_DATA_FRAME = 0,	 //!< Message Contains DATA Frame.
	CAN_HAL_REMOTE_FRAME = 1 //!< Message Contains Remote Frame.
} can_hal_frame_t;

/*! @brief CAN Interrupt type. */
typedef enum {
	CAN_HAL_INT_RI = 0,	  //!< CAN Receiver Interrupt.
	CAN_HAL_INT_TI1 = 1,  //!< CAN Transmit Interrupt for Buffer1.
	CAN_HAL_INT_EI = 2,	  //!< CAN Error Warning Interrupt.
	CAN_HAL_INT_DOI = 3,  //!< CAN Data Overrun Interrupt.
	CAN_HAL_INT_WUI = 4,  //!< CAN Wake-Up Interrupt.
	CAN_HAL_INT_EPI = 5,  //!< CAN Error Passive Interrupt.
	CAN_HAL_INT_ALI = 6,  //!< CAN Arbitration Lost Interrupt.
	CAN_HAL_INT_BEI = 7,  //!< CAN Bus Error Interrupt.
	CAN_HAL_INT_IDI = 8,  //!< CAN ID Ready Interrupt.
	CAN_HAL_INT_TI2 = 9,  //!< CAN Transmit Interrupt for Buffer2.
	CAN_HAL_INT_TI3 = 10, //!< CAN Transmit Interrupt for Buffer3.
	CAN_HAL_INT_FC = 11	  //!< FullCAN Interrupt.
} can_hal_int_t;

/*! @brief CAN Interrupt Masks. */
typedef enum {
	CAN_HAL_INT_MASK_RI = 0x0001,  //!< Receive Interrupt
	CAN_HAL_INT_MASK_TI1 = 0x0002, //!< Transmit Interrupt 1
	CAN_HAL_INT_MASK_EI = 0x0004,  //!< Error Warning Interrupt
	CAN_HAL_INT_MASK_DOI = 0x0008, //!< Data Overrun Interrupt
	CAN_HAL_INT_MASK_WUI = 0x0010, //!< Wake-Up Interrupt
	CAN_HAL_INT_MASK_EPI = 0x0020, //!< Error Passive Interrupt
	CAN_HAL_INT_MASK_ALI = 0x0040, //!< Arbitration Lost Interrupt
	CAN_HAL_INT_MASK_BEI = 0x0080, //!< Bus Error Interrupt
	CAN_HAL_INT_MASK_IDI = 0x0100, //!< ID Ready Interrupt
	CAN_HAL_INT_MASK_TI2 = 0x0200, //!< Transmit Interrupt 2
	CAN_HAL_INT_MASK_TI3 = 0x0400, //!< Transmit Interrupt 3
	CAN_HAL_INT_MASK_FC = 0x07FF   //!< Full CAN Interrupt
} can_hal_int_mask_t;

/*! @brief CAN message. */
typedef struct {
	uint32_t messageId;			  //!< CAN Message ID
	uint8_t data[8];			  //!< Message Data
	uint8_t length;				  //!< Message Data length
	can_hal_id_format_t idFormat; //!< Message ID Format
	can_hal_frame_t frameType;	  //!< Message Frame Type
} can_hal_msg_t;

/** Function prototype for CAN receive callback function */
typedef void (*can_hal_callback_t)(void *usr_arg, uint32_t int_status);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 *  @brief Function for CAN initialization
 *  @param[in] instance CAN instance.
 *  @param[in] baudrate CAN baudrate.
 *  @return None
 */
void HAL_CAN_Init(can_hal_instance_t instance, uint32_t baudrate);

/**
 *  @brief Function for configuring CAN Controller operational mode.
 *  @param[in] instance CAN instance.
 *  @param[in] mode CAN Controller operational mode.
 *  @param[in] state Functional state of CAN controller mode:
 *                      lFunctionalState_Enable -> enable mode
 *                      lFunctionalState_Disable -> disable mode
 *  @return None
 */
void HAL_CAN_ModeConfig(can_hal_instance_t instance, can_hal_mode_type_t mode,
						lFunctionalState_t state);

/**
 *  @brief Function for CAN deinitialization
 *  @param[in] instance CAN instance
 *  @return None
 */
void HAL_CAN_DeInit(can_hal_instance_t instance);

/**
 *  @brief Function for sending message
 *  @param[in] instance CAN instance.
 *  @param[in] canMessage CAN message.
 *  @param[in] timeoutUs Transfer timeout [us].
 *  @retval Transfer status.
 */
lStatus_t HAL_CAN_SendMessage(can_hal_instance_t instance,
							  can_hal_msg_t *canMessage, uint64_t timeoutUs);

/**
 *  @brief Function for receiving message
 *  @param[in] instance CAN instance
 *  @param[in] canMessage CAN message.
 *  @param[in] timeoutUs Transfer timeout [us].
 *  @retval Transfer status.
 */
lStatus_t HAL_CAN_ReceiveMessage(can_hal_instance_t instance,
								 can_hal_msg_t *canMessage, uint64_t timeoutUs);

/**
 *  @brief Function for enabling interrupt
 *  @param[in] instance CAN instance
 *  @param[in] callback CAN callback.
 *  @param[in] usr_arg User data.
 *  @return None
 */
void HAL_CAN_EnableInterrupt(can_hal_instance_t instance,
							 can_hal_callback_t callback, void *usr_arg);

/**
 *  @brief Function for enable/disable interrupt
 *  @param[in] instance CAN instance
 *  @param[in] intType Interrupt type
 *  @param[in] state New state
 *  @return None
 */
void HAL_CAN_IntCmd(can_hal_instance_t instance, can_hal_int_t intType,
					lFunctionalState_t state);

/**
 *  @brief Function for getting interrupt status
 *  @param[in] instance CAN instance
 *  @return Value of interrupt status register.
 */
uint32_t HAL_CAN_GetIntStatus(can_hal_instance_t instance);

#ifdef __cplusplus
}
#endif

#endif /* CAN_H */