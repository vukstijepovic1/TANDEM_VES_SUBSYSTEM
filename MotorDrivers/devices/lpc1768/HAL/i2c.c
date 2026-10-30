/**
 *	@file     i2c.c
 *  @brief    HAL I2C library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "i2c.h"

#include <assert.h>

#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"

#include "util.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_I2C_INSTANCE_NUM 3

#define HAL_I2C_MAX_RETRANSMISSION_CNT 3

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static inline LPC_I2C_TypeDef *prvI2C_GetInstance(i2c_hal_instance_t I2Cx);

static void prvI2C_InitPins(i2c_hal_instance_t I2Cx);

static uint32_t prvI2C_Start(LPC_I2C_TypeDef *I2Cx);

static void prvI2C_Stop(LPC_I2C_TypeDef *I2Cx);

static uint32_t prvI2C_SendByte(LPC_I2C_TypeDef *I2Cx, const uint8_t byte);

static uint32_t prvI2C_GetByte(LPC_I2C_TypeDef *I2Cx, uint8_t *byte,
							   const Bool ack);

static int32_t prvI2C_StateMachine(LPC_I2C_TypeDef *I2Cx,
								   const uint32_t codeStatus,
								   I2C_M_SETUP_Type *TransferCfg);

/*******************************************************************************
 * Code
 ******************************************************************************/

lStatus_t HAL_I2C_Init(i2c_hal_instance_t instance, uint32_t clockrate)
{
	assert(instance < HAL_I2C_INSTANCE_NUM);

	LPC_I2C_TypeDef *lpcInstance = NULL;
	prvI2C_InitPins(instance);

	lpcInstance = prvI2C_GetInstance(instance);
	I2C_Init(lpcInstance, clockrate);
	I2C_Cmd(lpcInstance, I2C_MASTER_MODE, ENABLE);

	return lStatus_Success;
}

void HAL_I2C_DeInit(i2c_hal_instance_t instance)
{
	assert(instance < HAL_I2C_INSTANCE_NUM);

	LPC_I2C_TypeDef *lpcInstance = NULL;

	lpcInstance = prvI2C_GetInstance(instance);

	I2C_DeInit(lpcInstance);
}

lStatus_t HAL_I2C_MasterTransmit(i2c_hal_instance_t instance,
								 uint32_t slaveAddress, uint8_t *pData,
								 uint32_t size, uint32_t timeoutMs)
{
	assert(instance < HAL_I2C_INSTANCE_NUM);

	LPC_I2C_TypeDef *lpcInstance = NULL;
	uint32_t retransmissionCnt = 0;
	uint32_t codeStatus;
	int32_t Ret = I2C_OK;
	I2C_M_SETUP_Type tran;
	uint64_t time0 = HAL_GetTimeUS();

	tran.sl_addr7bit = slaveAddress;
	tran.tx_data = pData;
	tran.tx_length = size;
	tran.rx_data = NULL;
	tran.rx_length = 0;

	lpcInstance = prvI2C_GetInstance(instance);

	tran.tx_count = 0;
	tran.rx_count = 0;

	codeStatus = prvI2C_Start(lpcInstance);

	while (1) {
		if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
			return lStatus_Timeout;

		Ret = prvI2C_StateMachine(lpcInstance, codeStatus, &tran);
		if (I2C_CheckError(Ret)) {
			retransmissionCnt++;
			if (retransmissionCnt > HAL_I2C_MAX_RETRANSMISSION_CNT) {
				return lStatus_Fail;
			} else {
				// Reset I2C setup value to default state
				tran.tx_count = 0;
				tran.rx_count = 0;

				codeStatus = prvI2C_Start(lpcInstance);

				continue;
			}
		} else if ((Ret & I2C_BYTE_SENT) || (Ret & I2C_BYTE_RECV)) {
			/* Wait for I2C state to change */
			while (!(lpcInstance->I2CONSET & I2C_I2CONSET_SI))
				;
		} else if ((Ret & I2C_SEND_END)) // Already sent all data
		{
			if (tran.rx_count >= tran.rx_length)
				break;
			else
				prvI2C_Start(lpcInstance);
		}
		codeStatus = lpcInstance->I2STAT & I2C_STAT_CODE_BITMASK;
	}

	return lStatus_Success;
}

lStatus_t HAL_I2C_MasterReceive(i2c_hal_instance_t instance,
								uint32_t slaveAddress, uint8_t *pData,
								uint32_t size, uint32_t timeoutMs)
{
	assert(instance < HAL_I2C_INSTANCE_NUM);

	LPC_I2C_TypeDef *lpcInstance = NULL;
	uint32_t retransmissionCnt = 0;
	uint32_t codeStatus;
	int32_t Ret = I2C_OK;
	I2C_M_SETUP_Type tran;
	uint64_t time0 = HAL_GetTimeUS();

	tran.sl_addr7bit = slaveAddress;
	tran.tx_data = NULL;
	tran.tx_length = 0;
	tran.rx_data = pData;
	tran.rx_length = size;

	lpcInstance = prvI2C_GetInstance(instance);

	tran.tx_count = 0;
	tran.rx_count = 0;

	codeStatus = prvI2C_Start(lpcInstance);

	while (1) {
		if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
			return lStatus_Timeout;

		Ret = prvI2C_StateMachine(lpcInstance, codeStatus, &tran);
		if (I2C_CheckError(Ret)) {
			retransmissionCnt++;
			if (retransmissionCnt > HAL_I2C_MAX_RETRANSMISSION_CNT) {
				return lStatus_Fail;
			} else {
				// Reset I2C setup value to default state
				tran.tx_count = 0;
				tran.rx_count = 0;

				codeStatus = prvI2C_Start(lpcInstance);

				continue;
			}
		} else if ((Ret & I2C_BYTE_SENT) || (Ret & I2C_BYTE_RECV)) {
			/* Wait for I2C state to change */
			while (!(lpcInstance->I2CONSET & I2C_I2CONSET_SI))
				;
		} else if (Ret & I2C_RECV_END) {
			break;
		}
		codeStatus = lpcInstance->I2STAT & I2C_STAT_CODE_BITMASK;
	}

	return lStatus_Success;
}

/****************************** static functions ******************************/

/* Initialize Pins */
static void prvI2C_InitPins(i2c_hal_instance_t I2Cx)
{
	PINSEL_CFG_Type pinCfg;
	uint8_t funcNum = PINSEL_FUNC_0;
	uint8_t portNum = PINSEL_PORT_0;
	uint8_t sdaPin = PINSEL_PIN_0;
	uint8_t sclPin = PINSEL_PIN_0;

	switch (I2Cx) {
	case I2C_HAL_INSTANCE_0:
		funcNum = PINSEL_FUNC_1;
		portNum = PINSEL_PORT_0;
		sdaPin = PINSEL_PIN_27;
		sclPin = PINSEL_PIN_28;

		break;
	case I2C_HAL_INSTANCE_1:
		funcNum = PINSEL_FUNC_3;
		portNum = PINSEL_PORT_0;
		sdaPin = PINSEL_PIN_19;
		sclPin = PINSEL_PIN_20;

		break;
	case I2C_HAL_INSTANCE_2:
		funcNum = PINSEL_FUNC_2;
		portNum = PINSEL_PORT_0;
		sdaPin = PINSEL_PIN_10;
		sclPin = PINSEL_PIN_11;

		break;
	default:
		return;
	}

	pinCfg.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
	pinCfg.Pinmode = PINSEL_PINMODE_PULLUP;

	pinCfg.Funcnum = funcNum;
	pinCfg.Portnum = portNum;
	pinCfg.Pinnum = sdaPin;
	PINSEL_ConfigPin(&pinCfg);
	pinCfg.Pinnum = sclPin;
	PINSEL_ConfigPin(&pinCfg);
}

/* Get LPC I2C Instance. */
static inline LPC_I2C_TypeDef *prvI2C_GetInstance(i2c_hal_instance_t I2Cx)
{
	assert(I2Cx < HAL_I2C_INSTANCE_NUM);

	LPC_I2C_TypeDef *lpcInstance = NULL;

	switch (I2Cx) {
	case I2C_HAL_INSTANCE_0:
		lpcInstance = LPC_I2C0;
		break;
	case I2C_HAL_INSTANCE_1:
		lpcInstance = LPC_I2C1;
		break;
	case I2C_HAL_INSTANCE_2:
		lpcInstance = LPC_I2C2;
		break;
	default:
		break;
	}

	return lpcInstance;
}

/* Start I2C Transfer - used by state machine */
static uint32_t prvI2C_Start(LPC_I2C_TypeDef *I2Cx)
{
	// Reset STA, STO, SI
	/* Clear interrupt bit and START bit */
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC | I2C_I2CONCLR_STOC | I2C_I2CONCLR_STAC;

	/* Enter master mode and transmit a START condition,
		or transmit a repeated START condition */
	I2Cx->I2CONSET = I2C_I2CONSET_STA;

	/* Wait for I2C state to change */
	while (!(I2Cx->I2CONSET & I2C_I2CONSET_SI))
		;

	/* Clear START bit */
	I2Cx->I2CONCLR = I2C_I2CONCLR_STAC; // Clear START bit in I2CONSET register

	return (I2Cx->I2STAT & I2C_STAT_CODE_BITMASK);
}

/* Stop I2C Transfer - used by state machine */
static void prvI2C_Stop(LPC_I2C_TypeDef *I2Cx)
{
	/* Make sure start bit is not active */
	if (I2Cx->I2CONSET & I2C_I2CONSET_STA) {
		/* Clear START bit*/
		I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
	}

	/* Transmit STOP condition and acknowledge */
	I2Cx->I2CONSET = I2C_I2CONSET_STO | I2C_I2CONSET_AA;

	/* Clear Interrupt bit */
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
}

/* Send Byte during I2C Transfer - used by state machine. */
static uint32_t prvI2C_SendByte(LPC_I2C_TypeDef *I2Cx, const uint8_t byte)
{
	uint32_t CodeStatus = I2Cx->I2STAT & I2C_STAT_CODE_BITMASK;

	if ((CodeStatus != I2C_I2STAT_M_TX_START) &&
		(CodeStatus != I2C_I2STAT_M_TX_RESTART) &&
		(CodeStatus != I2C_I2STAT_M_TX_SLAW_ACK) &&
		(CodeStatus != I2C_I2STAT_M_TX_DAT_ACK)) {
		return CodeStatus;
	}

	/* Make sure start bit is not active */
	if (I2Cx->I2CONSET & I2C_I2CONSET_STA) {
		/* Clear START bit */
		I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
	}

	/* Mask data byte */
	I2Cx->I2DAT = byte & I2C_I2DAT_BITMASK;

	/* Send ACK */
	I2Cx->I2CONSET = I2C_I2CONSET_AA;

	/* Clear Interrupt bit */
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;

	return (I2Cx->I2STAT & I2C_STAT_CODE_BITMASK);
}

/* Get Byte during I2C Transfer - used by state machine. */
static uint32_t prvI2C_GetByte(LPC_I2C_TypeDef *I2Cx, uint8_t *byte,
							   const Bool ack)
{
	*byte = (uint8_t)(I2Cx->I2DAT & I2C_I2DAT_BITMASK);

	if (ack == TRUE) {
		/* Send ACK */
		I2Cx->I2CONSET = I2C_I2CONSET_AA;
	} else {
		/* Clear ACK */
		I2Cx->I2CONCLR = I2C_I2CONCLR_AAC;
	}

	/* Clear Interrupt bit */
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;

	return (I2Cx->I2STAT & I2C_STAT_CODE_BITMASK);
}

/* I2C State Machine, used to track transfer. */
static int32_t prvI2C_StateMachine(LPC_I2C_TypeDef *I2Cx,
								   const uint32_t codeStatus,
								   I2C_M_SETUP_Type *TransferCfg)
{
	uint8_t *txdat;
	uint8_t *rxdat;
	uint8_t tmp;
	int32_t Ret = I2C_OK;

	// get buffer to send/receive
	txdat = (uint8_t *)&TransferCfg->tx_data[TransferCfg->tx_count];
	rxdat = (uint8_t *)&TransferCfg->rx_data[TransferCfg->rx_count];

	switch (codeStatus) {
	case I2C_I2STAT_M_TX_START:
	case I2C_I2STAT_M_TX_RESTART:
		// Send data first
		if (TransferCfg->tx_count < TransferCfg->tx_length) {
			/* Send slave address + WR direction bit = 0 ---------------- */
			prvI2C_SendByte(I2Cx, (TransferCfg->sl_addr7bit << 1));
			Ret = I2C_BYTE_SENT;
		} else if (TransferCfg->rx_count < TransferCfg->rx_length) {
			/* Send slave address + RD direction bit = 1 ---------------- */
			prvI2C_SendByte(I2Cx, ((TransferCfg->sl_addr7bit << 1) | 0x01));
			Ret = I2C_BYTE_SENT;
		}

		break;
	case I2C_I2STAT_M_TX_SLAW_ACK:
	case I2C_I2STAT_M_TX_DAT_ACK:
		if (TransferCfg->tx_count < TransferCfg->tx_length) {
			prvI2C_SendByte(I2Cx, *txdat);

			txdat++;

			TransferCfg->tx_count++;

			Ret = I2C_BYTE_SENT;
		} else {
			prvI2C_Stop(I2Cx);

			Ret = I2C_SEND_END;
		}

		break;
	case I2C_I2STAT_M_TX_DAT_NACK:
		prvI2C_Stop(I2Cx);
		Ret = I2C_SEND_END;

		break;
	case I2C_I2STAT_M_RX_ARB_LOST:
		I2Cx->I2CONSET = I2C_I2CONSET_STA | I2C_I2CONSET_AA;
		I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;

		break;
	case I2C_I2STAT_M_RX_SLAR_ACK:
		I2Cx->I2CONSET = I2C_I2CONSET_AA;
		I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
		Ret = I2C_BYTE_RECV;

		break;
	case I2C_I2STAT_M_RX_DAT_ACK:
		if (TransferCfg->rx_count < TransferCfg->rx_length) {
			if (TransferCfg->rx_count < (TransferCfg->rx_length - 2)) {
				prvI2C_GetByte(I2Cx, &tmp, TRUE);

				Ret = I2C_BYTE_RECV;
			} else // the next byte is the last byte, send NACK instead.
			{
				prvI2C_GetByte(I2Cx, &tmp, FALSE);
				Ret = I2C_BYTE_RECV;
			}
			*rxdat++ = tmp;

			TransferCfg->rx_count++;
		} else {
			Ret = I2C_RECV_END;
		}

		break;
	case I2C_I2STAT_M_RX_DAT_NACK:
		prvI2C_GetByte(I2Cx, &tmp, FALSE);
		*rxdat++ = tmp;
		TransferCfg->rx_count++;
		prvI2C_Stop(I2Cx);
		Ret = I2C_RECV_END;

		break;
	case I2C_I2STAT_M_RX_SLAR_NACK:
	case I2C_I2STAT_M_TX_SLAW_NACK:
	case I2C_I2STAT_BUS_ERROR:
		// Send STOP condition
		prvI2C_Stop(I2Cx);
		Ret = I2C_ERR;

		break;
		/* No status information */
	case I2C_I2STAT_NO_INF:
	default:
		I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
		break;
	}

	return Ret;
}