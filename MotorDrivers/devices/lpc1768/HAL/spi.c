/**
 *	@file     spi.c
 *  @brief    HAL SPI library.
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "spi.h"

#include <assert.h>

#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_spi.h"

#include "gpio.h"
#include "util.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_SPI_SPCR_BITS(n)                                                   \
	((n == 0) ? ((uint32_t)0) : ((uint32_t)((n & 0x0F) << 8)))
#define HAL_SPI_SPCR_BITS_REV(n)                                               \
	((n == 0) ? ((uint32_t)0) : ((uint32_t)(n >> 8)))

#define HAL_SPI_IRQN_OFFSET 13
#define HAL_SPI_INSTANCE_TO_IRQN(instance) (instance + HAL_SPI_IRQN_OFFSET)

#define HAL_SPI_INSTANCE_NUM 1

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

void *prvSPI_TransferHandle;

spi_hal_slave_t prvSPI_HAL_SLAVES[HAL_SPI_MAX_SLAVE_NUM];
uint8_t prvSPI_HAL_SLAVES_INDEX;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Initialize pins. */
static void prvSPI_InitPins(spi_hal_mode_t mode);

/* Get LPC Instance. */
static inline LPC_SPI_TypeDef *prvSPI_GetInstance(spi_hal_instance_t SPIx);

/*******************************************************************************
 * Code
 ******************************************************************************/

lStatus_t HAL_SPI_Init(spi_hal_instance_t instance, spi_hal_cfg_t *spiCfg)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);
	assert(spiCfg->clockRate <= CLKPWR_GetPCLK(CLKPWR_PCLKSEL_SPI));

	LPC_SPI_TypeDef *lpcInstance = NULL;
	SPI_CFG_Type SPICfg;

	lpcInstance = prvSPI_GetInstance(instance);

	prvSPI_InitPins(spiCfg->mode);

	SPICfg.Databit = HAL_SPI_SPCR_BITS(spiCfg->databit);
	SPICfg.CPHA = spiCfg->cpha;
	SPICfg.CPOL = spiCfg->cpol;
	SPICfg.Mode = spiCfg->mode;
	SPICfg.DataOrder = spiCfg->dataOrder;
	SPICfg.ClockRate = spiCfg->clockRate;

	SPI_Init(lpcInstance, &SPICfg);

	memset(prvSPI_HAL_SLAVES, 0,
		   HAL_SPI_MAX_SLAVE_NUM * sizeof(spi_hal_slave_t));
	prvSPI_HAL_SLAVES_INDEX = 0;

	return lStatus_Success;
}

void HAL_SPI_DeInit(spi_hal_instance_t instance)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	LPC_SPI_TypeDef *lpcInstance = NULL;

	lpcInstance = prvSPI_GetInstance(instance);

	SPI_DeInit(lpcInstance);
}

void HAL_SPI_SetClock(spi_hal_instance_t instance, const uint32_t target_clock)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	LPC_SPI_TypeDef *lpcInstance = NULL;

	lpcInstance = prvSPI_GetInstance(instance);

	SPI_SetClock(lpcInstance, target_clock);
}

void HAL_SPI_ConfigStructInit(spi_hal_cfg_t *spiCfg)
{
	SPI_CFG_Type SPICfg;
	SPI_ConfigStructInit(&SPICfg);

	spiCfg->databit = HAL_SPI_SPCR_BITS_REV(SPICfg.Databit);
	spiCfg->cpha = SPICfg.CPHA;
	spiCfg->cpol = SPICfg.CPOL;
	spiCfg->mode = SPICfg.Mode;
	spiCfg->dataOrder = SPICfg.DataOrder;
	spiCfg->clockRate = SPICfg.ClockRate;
}

spi_hal_slave_id_t HAL_SPI_SlaveInit(spi_hal_instance_t instance,
									 spi_hal_slave_t *slave)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	if (prvSPI_HAL_SLAVES_INDEX >= HAL_SPI_MAX_SLAVE_NUM)
		return -1;

	gpio_hal_cfg_type_t gpioCfg;

	gpioCfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpioCfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	gpioCfg.pinDir = GPIO_HAL_OUTPUT;

	prvSPI_HAL_SLAVES[prvSPI_HAL_SLAVES_INDEX].pinNum = slave->pinNum;
	prvSPI_HAL_SLAVES[prvSPI_HAL_SLAVES_INDEX].portNum = slave->portNum;

	/* TODO: handle return value! */
	HAL_GPIO_Init(slave->portNum, slave->pinNum, &gpioCfg);
	HAL_GPIO_SetPinValue(slave->portNum, slave->pinNum, 1);

	return prvSPI_HAL_SLAVES_INDEX++;
}

void HAL_SPI_SlavePinState(spi_hal_instance_t instance,
						   spi_hal_slave_id_t slaveId,
						   spi_hal_ssel_pin_value_t slavePinValue)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	HAL_GPIO_SetPinValue(prvSPI_HAL_SLAVES[slaveId].portNum,
						 prvSPI_HAL_SLAVES[slaveId].pinNum, slavePinValue);
}

lStatus_t HAL_SPI_Transmit(spi_hal_instance_t instance, void *txData,
						   uint32_t txSize, uint32_t timeoutMs)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	LPC_SPI_TypeDef *lpcInstance = NULL;
	uint64_t time0 = HAL_GetTimeUS();
	uint32_t dataCnt = 0;
	uint32_t stat;
	uint8_t dataSize = 0;

	uint8_t *txData8 = NULL;
	uint16_t *txData16 = NULL;

	lpcInstance = prvSPI_GetInstance(instance);

	if (SPI_GetDataSize(lpcInstance) == SPI_HAL_DATABIT_8) {
		dataSize = 1;
		txData8 = txData;
	} else {
		dataSize = 2;
		txData16 = txData;
	}

	while (dataCnt < txSize) {
		if (dataSize == 1) {
			SPI_SendData(lpcInstance, *txData8++);
		} else {
			SPI_SendData(lpcInstance, *txData16++);
		}

		dataCnt += dataSize;

		// Wait for transfer complete
		while (!((stat = lpcInstance->SPSR) & SPI_SPSR_SPIF)) {
			if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
				return lStatus_Timeout;
		}
		if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
			return lStatus_Timeout;

		// Check for error
		if (stat &
			(SPI_SPSR_ABRT | SPI_SPSR_MODF | SPI_SPSR_ROVR | SPI_SPSR_WCOL)) {
			return lStatus_Fail;
		}

		/* read data from data line but dont store it */
		SPI_ReceiveData(lpcInstance);
	}

	return lStatus_Success;
}

lStatus_t HAL_SPI_Receive(spi_hal_instance_t instance, void *rxData,
						  uint32_t txSize, uint32_t timeoutMs)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	LPC_SPI_TypeDef *lpcInstance = NULL;
	uint64_t time0 = HAL_GetTimeUS();
	uint32_t dataCnt = 0;
	uint32_t stat;
	uint16_t tmp;
	uint8_t dataSize = 0;
	uint8_t *rxData8 = NULL;
	uint16_t *rxData16 = NULL;

	/* Get LPC SPI Instance */
	lpcInstance = prvSPI_GetInstance(instance);

	if (SPI_GetDataSize(lpcInstance) == SPI_HAL_DATABIT_8) {
		dataSize = 1;
		rxData8 = rxData;
	} else {
		dataSize = 2;
		rxData16 = rxData;
	}

	while (dataCnt < txSize) {
		if (dataSize == 1) {
			SPI_SendData(lpcInstance, 0xFF);
		} else {
			SPI_SendData(lpcInstance, 0xFFFF);
		}

		// Wait for byte transfer complete
		while (!((stat = lpcInstance->SPSR) & SPI_SPSR_SPIF)) {
			if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
				return lStatus_Timeout;
		}
		if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
			return lStatus_Timeout;

		// Check for error
		if (stat &
			(SPI_SPSR_ABRT | SPI_SPSR_MODF | SPI_SPSR_ROVR | SPI_SPSR_WCOL)) {
			return lStatus_Fail;
		}

		/* read data from data line but dont store it */
		tmp = SPI_ReceiveData(lpcInstance);

		if (dataSize == 1) {
			*rxData8 = (uint8_t)tmp;
			rxData8++;
		} else {
			*rxData16 = (uint16_t)tmp;
			rxData16++;
		}

		dataCnt += dataSize;
	}

	return lStatus_Success;
}

void HAL_SPI_TransferCreateHandle(spi_hal_instance_t instance,
								  spi_hal_handle_t *handle,
								  spi_hal_callback_t callback, void *userData)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);
	assert(handle != NULL);

	memset(handle, 0, sizeof(spi_hal_handle_t));

	handle->rxState = lState_Reset;
	handle->txState = lState_Reset;

	handle->callback = callback;
	handle->userData = userData;

	NVIC_SetPriority(HAL_SPI_INSTANCE_TO_IRQN(instance), HAL_SPI_NVIC_PRIORITY);
	NVIC_EnableIRQ(HAL_SPI_INSTANCE_TO_IRQN(instance));
}

lStatus_t HAL_SPI_TransmitISR(spi_hal_instance_t instance,
							  spi_hal_handle_t *handle, uint8_t *txData,
							  uint32_t txSize)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	LPC_SPI_TypeDef *lpcInstance = NULL;
	uint8_t *txData8 = NULL;
	uint16_t *txData16 = NULL;
	uint8_t dataSize = 0;

	handle->txData = txData;
	handle->rxData = NULL;
	handle->counter = 0;
	handle->length = txSize;

	handle->txState = lState_Set;
	handle->rxState = lState_Reset;

	lpcInstance = prvSPI_GetInstance(instance);

	prvSPI_TransferHandle = handle;

	if (lpcInstance->SPINT & SPI_SPINT_INTFLAG)
		lpcInstance->SPINT = SPI_SPINT_INTFLAG;

	SPI_IntCmd(lpcInstance, ENABLE);

	if (SPI_GetDataSize(lpcInstance) == SPI_HAL_DATABIT_8) {
		dataSize = 1;
		txData8 = (uint8_t *)handle->txData;
		SPI_SendData(lpcInstance, *txData8);
	} else {
		dataSize = 2;
		txData16 = (uint16_t *)handle->txData;
		SPI_SendData(lpcInstance, *txData16);
	}

	handle->counter += dataSize;

	return lStatus_Success;
}

lStatus_t HAL_SPI_ReceiveISR(spi_hal_instance_t instance,
							 spi_hal_handle_t *handle, uint8_t *rxData,
							 uint32_t rxSize)
{
	assert(instance < HAL_SPI_INSTANCE_NUM);

	LPC_SPI_TypeDef *lpcInstance = NULL;

	handle->txData = NULL;
	handle->rxData = rxData;
	handle->counter = 0;
	handle->length = rxSize;

	handle->txState = lState_Reset;
	handle->rxState = lState_Set;

	lpcInstance = prvSPI_GetInstance(instance);

	prvSPI_TransferHandle = handle;

	if (lpcInstance->SPINT & SPI_SPINT_INTFLAG)
		lpcInstance->SPINT = SPI_SPINT_INTFLAG;

	SPI_ReceiveData(lpcInstance);

	SPI_IntCmd(lpcInstance, ENABLE);

	if (SPI_GetDataSize(lpcInstance) == SPI_HAL_DATABIT_8) {
		SPI_SendData(lpcInstance, 0xFF);
	} else {
		SPI_SendData(lpcInstance, 0xFFFF);
	}

	return lStatus_Success;
}

/************************************ IRQs ************************************/

/**
 *  @brief SPI IRQ Handler.
 */
void SPI_IRQHandler(void)
{
	LPC_SPI_TypeDef *lpcInstance;
	spi_hal_handle_t *handle;
	uint16_t tmp;

	uint8_t dataSize = 0;
	uint8_t *rxData8 = NULL;
	uint8_t *txData8 = NULL;
	uint16_t *rxData16 = NULL;
	uint16_t *txData16 = NULL;

	lpcInstance = prvSPI_GetInstance(SPI_HAL_INSTANCE_0);
	handle = prvSPI_TransferHandle;

	if (SPI_GetDataSize(lpcInstance) == SPI_HAL_DATABIT_8) {
		dataSize = 1;
		rxData8 = (uint8_t *)(handle->rxData + handle->counter);
		txData8 = (uint8_t *)(handle->txData + handle->counter);
	} else {
		dataSize = 2;
		rxData16 = (uint16_t *)(handle->rxData + handle->counter);
		txData16 = (uint16_t *)(handle->txData + handle->counter);
	}

	SPI_ClearIntPending(lpcInstance);

	tmp = SPI_GetStatus(lpcInstance);
	if (tmp & (SPI_SPSR_ABRT | SPI_SPSR_MODF | SPI_SPSR_ROVR | SPI_SPSR_WCOL)) {
		SPI_IntCmd(lpcInstance, DISABLE);

		handle->callback(SPI_HAL_INSTANCE_0, handle, lStatus_Fail,
						 handle->userData);
		return;
	}

	if (handle->counter >= handle->length) {
		SPI_IntCmd(lpcInstance, DISABLE);

		handle->callback(SPI_HAL_INSTANCE_0, handle, lStatus_Success,
						 handle->userData);
	} else {
		if (tmp & SPI_SPSR_SPIF) {
			tmp = SPI_ReceiveData(lpcInstance);
			if (handle->rxData != NULL) {
				if (dataSize == 1)
					*rxData8 = (uint8_t)tmp;
				else
					*rxData16 = (uint16_t)tmp;

				handle->counter += dataSize;
			}
		}

		if (handle->counter < handle->length) {
			if (handle->txData == NULL) {
				if (dataSize == 1)
					SPI_SendData(lpcInstance, 0xFF);
				else
					SPI_SendData(lpcInstance, 0xFFFF);

			} else {

				if (dataSize == 1)
					SPI_SendData(lpcInstance, *txData8);
				else
					SPI_SendData(lpcInstance, *txData16);

				handle->counter += dataSize;
			}
		} else {
			SPI_IntCmd(lpcInstance, DISABLE);

			handle->callback(SPI_HAL_INSTANCE_0, handle, lStatus_Success,
							 handle->userData);
		}
	}
}

/****************************** static functions ******************************/

/* Initialize pins. */
static void prvSPI_InitPins(spi_hal_mode_t mode)
{
	PINSEL_CFG_Type PinCfg;

	// SCK
	PinCfg.Funcnum = PINSEL_FUNC_3;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum = PINSEL_PORT_0;
	PinCfg.Pinnum = PINSEL_PIN_15;
	PINSEL_ConfigPin(&PinCfg);

	// MISO
	PinCfg.Pinnum = PINSEL_PIN_17;
	PINSEL_ConfigPin(&PinCfg);

	// MOSI
	PinCfg.Pinnum = PINSEL_PIN_18;
	PINSEL_ConfigPin(&PinCfg);

	if (mode == SPI_HAL_SLAVE_MODE) {
		PinCfg.Funcnum = PINSEL_FUNC_0;
		PinCfg.Pinnum = PINSEL_PIN_16;
		PINSEL_ConfigPin(&PinCfg);
	}
}

/* Get LPC Instance. */
static inline LPC_SPI_TypeDef *prvSPI_GetInstance(spi_hal_instance_t SPIx)
{
	LPC_SPI_TypeDef *lpcInstance = NULL;

	switch (SPIx) {
	case SPI_HAL_INSTANCE_0:
		lpcInstance = LPC_SPI;
		break;
	default:
		break;
	}

	return lpcInstance;
}

/********************************* End Of File ********************************/