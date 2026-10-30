/**
 *	@file     can.c
 *  @brief    CAN Hardware Abstraction Layer.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "can.h"

#include <assert.h>

#include "lpc17xx_can.h"
#include "lpc17xx_pinsel.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_CAN_INSTANCE_CNT 2

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

can_hal_callback_t prvCAN1_CALLBACK; //!< CAN1 Callback
void *prvCAN1_USR_ARG;				 //!< CAN1 Callback User data
can_hal_callback_t prvCAN2_CALLBACK; //!< CAN2 Callback
void *prvCAN2_USR_ARG;				 //!< CAN2 CAllback User data

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Initialize pins. */
static void prvCAN_InitPins(can_hal_instance_t instance);

/* Get LPC Instance. */
static inline LPC_CAN_TypeDef *prvCAN_GetInstance(can_hal_instance_t instance);

/* Send message on CAN bus. */
static lStatus_t prvCAN_SendMsg(LPC_CAN_TypeDef *lpcInstance,
								can_hal_msg_t *msg);

/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_CAN_Init(can_hal_instance_t instance, uint32_t baudrate)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	prvCAN_InitPins(instance);

	LPC_CAN_TypeDef *lpcInstance = prvCAN_GetInstance(instance);

	CAN_Init(lpcInstance, baudrate);

	CAN_ModeConfig(lpcInstance, CAN_HAL_MODE_OPERATING, ENABLE);

	// Put CAN Controller in Reset State.
	lpcInstance->MOD |= CAN_MOD_RM;
	// Set Transmit Priority Mode bit.
	lpcInstance->MOD |= CAN_MOD_TPM;
	// Put CAN Controller back in Operational Mode.
	lpcInstance->MOD &= ~CAN_MOD_RM;

	CAN_SetAFMode(LPC_CANAF, CAN_AccBP); /** @note: Bypass Acceptance filter. */
}

void HAL_CAN_ModeConfig(can_hal_instance_t instance, can_hal_mode_type_t mode,
						lFunctionalState_t state)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	LPC_CAN_TypeDef *lpcInstance;

	lpcInstance = prvCAN_GetInstance(instance);

	CAN_ModeConfig(lpcInstance, mode, state);
}

void HAL_CAN_DeInit(can_hal_instance_t instance)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	LPC_CAN_TypeDef *lpcInstance = prvCAN_GetInstance(instance);

	CAN_DeInit(lpcInstance);
}

lStatus_t HAL_CAN_SendMessage(can_hal_instance_t instance,
							  can_hal_msg_t *canMessage, uint64_t timeoutUs)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	lStatus_t status;
	CAN_MSG_Type canMsg;
	uint64_t time0;

	time0 = HAL_GetTimeUS();

	canMsg.id = canMessage->messageId;
	memcpy(canMsg.dataA, canMessage->data, 4);
	memcpy(canMsg.dataB, canMessage->data + 4, 4);
	canMsg.len = canMessage->length;
	canMsg.format = canMessage->idFormat;
	canMsg.type = canMessage->frameType;

	LPC_CAN_TypeDef *lpcInstance = prvCAN_GetInstance(instance);

	do {
		status = prvCAN_SendMsg(lpcInstance, &canMsg);

		if ((HAL_GetTimeUS() - time0) >= timeoutUs)
			break;

	} while (status != lStatus_Success);

	return status;
}

lStatus_t HAL_CAN_ReceiveMessage(can_hal_instance_t instance,
								 can_hal_msg_t *canMessage, uint64_t timeoutUs)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	Status status;
	uint64_t time0;
	CAN_MSG_Type canMsg;
	LPC_CAN_TypeDef *lpcInstance = prvCAN_GetInstance(instance);

	time0 = HAL_GetTimeUS();

	do {
		status = CAN_ReceiveMsg(lpcInstance, &canMsg);

		if ((HAL_GetTimeUS() - time0) >= timeoutUs)
			break;

	} while (status != SUCCESS);

	if (status == SUCCESS) {
		canMessage->messageId = canMsg.id;
		memcpy(canMessage->data, &canMsg.dataA, 4);
		memcpy(canMessage->data + 4, &canMsg.dataB, 4);
		canMessage->length = canMsg.len;
		canMessage->idFormat = canMsg.format;
		canMessage->frameType = canMsg.type;

		return lStatus_Success;
	} else {
		return lStatus_Fail;
	}
}

void HAL_CAN_EnableInterrupt(can_hal_instance_t instance,
							 can_hal_callback_t callback, void *usr_arg)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	switch (instance) {
	case CAN_HAL_INSTANCE_0:
		prvCAN1_CALLBACK = callback;
		prvCAN1_USR_ARG = usr_arg;

		break;
	case CAN_HAL_INSTANCE_1:
		prvCAN2_CALLBACK = callback;
		prvCAN2_USR_ARG = usr_arg;

		break;
	}

	NVIC_SetPriority(CAN_IRQn, CAN_HAL_DEF_INT_PRIO);
	NVIC_EnableIRQ(CAN_IRQn);
}

void HAL_CAN_IntCmd(can_hal_instance_t instance, can_hal_int_t intType,
					lFunctionalState_t state)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	LPC_CAN_TypeDef *lpcInstance = prvCAN_GetInstance(instance);

	CAN_IRQCmd(lpcInstance, intType, state);
}

uint32_t HAL_CAN_GetIntStatus(can_hal_instance_t instance)
{
	assert(instance < HAL_CAN_INSTANCE_CNT);

	LPC_CAN_TypeDef *lpcInstance = prvCAN_GetInstance(instance);

	return CAN_IntGetStatus(lpcInstance);
}

/************************************ IRQs ************************************/

void CAN_IRQHandler(void)
{
	uint32_t intStatus;

	intStatus = CAN_IntGetStatus(LPC_CAN1);
	if (intStatus) {
		if (prvCAN1_CALLBACK != NULL)
			prvCAN1_CALLBACK(prvCAN1_USR_ARG, intStatus & CAN_HAL_INT_MASK_FC);
	}

	/** @note: CAN2 is not considered for now!
	 *         Reading CAN2 Interrupt Status causes Hard Fault. */
	// intStatus = CAN_IntGetStatus(LPC_CAN2);
	// if (intStatus)
	// {
	// 	if(prvCAN2_CALLBACK != NULL)
	//     	prvCAN2_CALLBACK(prvCAN2_USR_ARG, intStatus & CAN_HAL_INT_MASK_FC);

	// }
}

/****************************** static functions ******************************/

static void prvCAN_InitPins(can_hal_instance_t instance)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t funcNum = PINSEL_FUNC_0;
	uint8_t portNum = PINSEL_PORT_0;
	uint8_t tdPin = PINSEL_PIN_0;
	uint8_t rdPin = PINSEL_PIN_0;

	switch (instance) {
	case CAN_HAL_INSTANCE_0:
		if (HAL_CAN_1_PIN_OPTION == 0) {
			funcNum = PINSEL_FUNC_1;
			portNum = PINSEL_PORT_0;
			rdPin = PINSEL_PIN_0;
			tdPin = PINSEL_PIN_1;
		} else {
			funcNum = PINSEL_FUNC_3;
			portNum = PINSEL_PORT_0;
			rdPin = PINSEL_PIN_21;
			tdPin = PINSEL_PIN_22;
		}
		break;
	case CAN_HAL_INSTANCE_1:
		if (HAL_CAN_2_PIN_OPTION == 0) {
			funcNum = PINSEL_FUNC_2;
			portNum = PINSEL_PORT_0;
			rdPin = PINSEL_PIN_4;
			tdPin = PINSEL_PIN_5;
		} else {
			funcNum = PINSEL_FUNC_1;
			portNum = PINSEL_PORT_2;
			rdPin = PINSEL_PIN_7;
			tdPin = PINSEL_PIN_8;
		}
		break;
	}

	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Funcnum = funcNum;
	PinCfg.Portnum = portNum;
	PinCfg.Pinnum = rdPin;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = tdPin;
	PINSEL_ConfigPin(&PinCfg);
}

static inline LPC_CAN_TypeDef *prvCAN_GetInstance(can_hal_instance_t instance)
{
	LPC_CAN_TypeDef *lpcInstance;
	switch (instance) {
	case CAN_HAL_INSTANCE_0:
		lpcInstance = (LPC_CAN_TypeDef *)LPC_CAN1;
		break;
	case CAN_HAL_INSTANCE_1:
		lpcInstance = (LPC_CAN_TypeDef *)LPC_CAN2;
		break;
	}

	return lpcInstance;
}

static uint8_t prvCAN_GetPriority(LPC_CAN_TypeDef *lpcInstance)
{
	uint8_t prio = 0;
	uint8_t tmp;

	if ((lpcInstance->SR & (1 << 2)) == 0) {
		tmp = lpcInstance->TFI1 & 0xFFU;
		if (tmp >= prio)
			prio = tmp + 1;
	}

	if ((lpcInstance->SR & (1 << 10)) == 0) {
		tmp = lpcInstance->TFI2 & 0xFFU;
		if (tmp >= prio)
			prio = tmp + 1;
	}

	if ((lpcInstance->SR & (1 << 18)) == 0) {
		tmp = lpcInstance->TFI3 & 0xFFU;
		if (tmp >= prio)
			prio = tmp + 1;
	}

	return prio;
}

static lStatus_t prvCAN_SendMsg(LPC_CAN_TypeDef *lpcInstance,
								can_hal_msg_t *msg)
{
	uint32_t *TFI = NULL;
	uint32_t *TID = NULL;
	uint32_t *TDA = NULL;
	uint32_t *TDB = NULL;
	uint32_t cmr;
	uint32_t data;

	uint8_t prio = 0;

	prio = prvCAN_GetPriority(lpcInstance);

	if (prio == 0xFF)
		return lStatus_Fail;

	if (lpcInstance->SR & (1 << 2)) {
		/* Transmit Buffer 1 is Empty */
		TFI = &lpcInstance->TFI1;
		TID = &lpcInstance->TID1;
		TDA = &lpcInstance->TDA1;
		TDB = &lpcInstance->TDB1;
		cmr = 0x21;
	} else if (lpcInstance->SR & (1 << 10)) {
		/* Transmit Buffer 2 is Empty */
		TFI = &lpcInstance->TFI2;
		TID = &lpcInstance->TID2;
		TDA = &lpcInstance->TDA2;
		TDB = &lpcInstance->TDB2;
		cmr = 0x41;
	} else if (lpcInstance->SR & (1 << 18)) {
		/* Transmit Buffer 3 is Empty */
		TFI = &lpcInstance->TFI3;
		TID = &lpcInstance->TID3;
		TDA = &lpcInstance->TDA3;
		TDB = &lpcInstance->TDB3;
		cmr = 0x81;
	} else {
		return lStatus_Fail;
	}

	*TFI &= ~0x000F0000;
	*TFI |= prio;
	*TFI |= (msg->length << 16);

	if (msg->frameType == CAN_HAL_REMOTE_FRAME)
		*TFI |= (1 << 30);
	else
		*TFI &= ~(1 << 30);

	if (msg->idFormat == CAN_HAL_ID_FORMAT_EXT)
		*TFI |= (0x80000000);
	else
		*TFI &= ~(0x80000000);

	*TID = msg->messageId;

	/*Write first 4 data bytes*/
	data = (msg->data[0]) | (((msg->data[1])) << 8) | ((msg->data[2]) << 16) |
		   ((msg->data[3]) << 24);
	*TDA = data;

	data = (msg->data[4]) | (((msg->data[5])) << 8) | ((msg->data[6]) << 16) |
		   ((msg->data[7]) << 24);
	*TDB = data;

	lpcInstance->CMR = cmr;

	return lStatus_Success;
}

/********************************* End Of File ********************************/