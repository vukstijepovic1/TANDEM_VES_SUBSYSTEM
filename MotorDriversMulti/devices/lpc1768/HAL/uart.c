/**
 *	@file     uart.c
 *  @brief    HAL UART library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "uart.h"

#include <assert.h>
#include <string.h>

#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"

#include "gpio.h"
#include "util.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define HAL_UART_IRQN_OFFSET 5
#define HAL_UART_INSTANCE_TO_IRQN(instance) (instance + HAL_UART_IRQN_OFFSET)

#define HAL_UART_INSTANCE_CNT 4

#define HAL_UART_2_PIN_OPTION_CNT 2
#define HAL_UART_3_PIN_OPTION_CNT 2

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

void *prvUART_TransferHandle[HAL_UART_INSTANCE_CNT];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void prvUART_HandleInterrupt(uart_hal_instance_t instance,
									uart_hal_handle_t *handle);

static inline LPC_UART_TypeDef *prvUART_GetInstance(uart_hal_instance_t UARTx);

static void prvUART_InitPins(uart_hal_instance_t instance);

/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_UART_Init(uart_hal_instance_t instance, uart_hal_cfg_t *uartCfg)
{
	assert(instance < HAL_UART_INSTANCE_CNT);
	LPC_UART_TypeDef *lpcInstance = NULL;
	UART_CFG_Type UARTConfigStruct;

	UARTConfigStruct.Baud_rate = uartCfg->baudrate;
	UARTConfigStruct.Parity = uartCfg->parity;
	UARTConfigStruct.Databits = uartCfg->databits;
	UARTConfigStruct.Stopbits = uartCfg->stopbits;

	lpcInstance = prvUART_GetInstance(instance);

	prvUART_InitPins(instance);

	UART_Init(lpcInstance, &UARTConfigStruct);

	UART_TxCmd(lpcInstance, lFunctionalState_Enable);
}

void HAL_UART_RS232Init(uart_hal_instance_t instance, uart_hal_cfg_t *uartCfg)
{
	assert(instance == UART_HAL_INSTANCE_1);

	LPC_UART_TypeDef *lpcInstance = NULL;
	UART_CFG_Type UARTConfigStruct;

	UARTConfigStruct.Baud_rate = uartCfg->baudrate;
	UARTConfigStruct.Parity = uartCfg->parity;
	UARTConfigStruct.Databits = uartCfg->databits;
	UARTConfigStruct.Stopbits = uartCfg->stopbits;

	lpcInstance = prvUART_GetInstance(instance);

	PINSEL_CFG_Type pin_cfg;

	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Funcnum = PINSEL_FUNC_2;
	pin_cfg.Portnum = PINSEL_PORT_2;
	pin_cfg.Pinnum = PINSEL_PIN_0;
	PINSEL_ConfigPin(&pin_cfg); // TX
	pin_cfg.Pinnum = PINSEL_PIN_1;
	PINSEL_ConfigPin(&pin_cfg); // RX

	UART_Init(lpcInstance, &UARTConfigStruct);

	UART_TxCmd(lpcInstance, lFunctionalState_Enable);

	gpio_hal_cfg_type_t gpioCfg;

	gpioCfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	gpioCfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpioCfg.pinDir = GPIO_HAL_OUTPUT;

	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 2, &gpioCfg); /* RS232/RS485 Select */
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 3, &gpioCfg); /* RS485 - TE485 enable */
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 4, &gpioCfg); /* Loopback enable */
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 5, &gpioCfg); /* Fast mode enable */
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_2, 6, &gpioCfg); /* Shutdown control */

	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 6, 1);
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 2, 0);
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 3, 0);
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 5, 1);
	HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_2, 4, 0);
}

void HAL_UART_DeInit(uart_hal_instance_t instance)
{
	LPC_UART_TypeDef *lpcInstance = NULL;

	lpcInstance = prvUART_GetInstance(instance);

	UART_DeInit(lpcInstance);
}

void HAL_UART_ConfigStructInit(uart_hal_cfg_t *uartCfg)
{
	UART_CFG_Type UARTConfigStruct;

	UART_ConfigStructInit(&UARTConfigStruct);

	uartCfg->baudrate = UARTConfigStruct.Baud_rate;
	uartCfg->parity = UARTConfigStruct.Parity;
	uartCfg->databits = UARTConfigStruct.Databits;
	uartCfg->stopbits = UARTConfigStruct.Stopbits;
}

void HAL_UART_ConfigureFIFO(uart_hal_instance_t instance,
							uart_hal_fifo_cfg_t *fifoCfg)
{
	assert(instance < HAL_UART_INSTANCE_CNT);

	LPC_UART_TypeDef *lpcInstance = NULL;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;

	UARTFIFOConfigStruct.FIFO_ResetRxBuf = fifoCfg->resetRxBuf;
	UARTFIFOConfigStruct.FIFO_ResetTxBuf = fifoCfg->resetTxBuf;
	UARTFIFOConfigStruct.FIFO_DMAMode = fifoCfg->DMAMode;
	UARTFIFOConfigStruct.FIFO_Level = fifoCfg->triggerLevel;

	lpcInstance = prvUART_GetInstance(instance);

	UART_FIFOConfig(lpcInstance, &UARTFIFOConfigStruct);
}

void HAL_UART_FIFOConfigStructInit(uart_hal_fifo_cfg_t *fifoCfg)
{
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;

	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	fifoCfg->resetRxBuf = UARTFIFOConfigStruct.FIFO_ResetRxBuf;
	fifoCfg->resetTxBuf = UARTFIFOConfigStruct.FIFO_ResetTxBuf;
	fifoCfg->DMAMode = UARTFIFOConfigStruct.FIFO_DMAMode;
	fifoCfg->triggerLevel = UARTFIFOConfigStruct.FIFO_Level;
}

lStatus_t HAL_UART_SendByte(uart_hal_instance_t instance, const uint8_t txByte)
{
	LPC_UART_TypeDef *lpcInstance;

	lpcInstance = prvUART_GetInstance(instance);

	if (!(lpcInstance->LSR & UART_LSR_THRE))
		return lStatus_Fail;

	UART_SendByte(lpcInstance, txByte);

	return lStatus_Success;
}

lStatus_t HAL_UART_ReceiveByte(uart_hal_instance_t instance, uint8_t *rxByte)
{
	LPC_UART_TypeDef *lpcInstance;

	lpcInstance = prvUART_GetInstance(instance);

	if (!(lpcInstance->LSR & UART_LSR_RDR))
		return lStatus_Fail;

	*rxByte = UART_ReceiveByte(lpcInstance);

	return lStatus_Success;
}

lStatus_t HAL_UART_Send(uart_hal_instance_t instance, uint8_t *txData,
						uint32_t dataSize, uint32_t timeoutMs)
{
	LPC_UART_TypeDef *lpcInstance = NULL;
	uint64_t time0 = HAL_GetTimeUS();
	uint32_t txDataSize = 0;
	uint8_t fifoCnt;
	uint8_t *pData;

	pData = txData;
	lpcInstance = prvUART_GetInstance(instance);

	if (lpcInstance == NULL)
		return lStatus_Fail;

	while (txDataSize < dataSize) {
		while (!(lpcInstance->LSR & UART_LSR_THRE)) {
			if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
				return lStatus_Timeout;
		}
		if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
			return lStatus_Timeout;

		fifoCnt = UART_TX_FIFO_SIZE;
		while (fifoCnt && txDataSize < dataSize) {
			UART_SendByte(lpcInstance, (*pData++));
			txDataSize++;
			fifoCnt--;
		}
	}

	return lStatus_Success;
}

lStatus_t HAL_UART_Receive(uart_hal_instance_t instance, uint8_t *rxData,
						   uint32_t dataSize, uint32_t timeoutMs)
{
	LPC_UART_TypeDef *lpcInstance = NULL;
	uint64_t time0 = HAL_GetTimeUS();
	uint32_t rxDataSize = 0;
	uint8_t *pData;

	pData = rxData;
	lpcInstance = prvUART_GetInstance(instance);

	if (lpcInstance == NULL)
		return lStatus_Fail;

	while (rxDataSize < dataSize) {
		while (!(lpcInstance->LSR & UART_LSR_RDR)) {
			if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
				return lStatus_Timeout;
		}
		if (HAL_GetTimeUS() - time0 >= (timeoutMs * 1000))
			return lStatus_Timeout;

		(*pData++) = UART_ReceiveByte(lpcInstance);
		rxDataSize++;
	}

	return lStatus_Success;
}

void HAL_UART_EnableInterrupt(uart_hal_instance_t instance)
{
	NVIC_SetPriority(HAL_UART_INSTANCE_TO_IRQN(instance),
					 HAL_UART_NVIC_PRIORITY);
	NVIC_EnableIRQ(HAL_UART_INSTANCE_TO_IRQN(instance));
}

void HAL_UART_TransferCreateHandle(uart_hal_instance_t instance,
								   uart_hal_handle_t *handle,
								   uart_hal_callback_t callback, void *userData)
{
	assert(handle != NULL);

	memset(handle, 0, sizeof(uart_hal_handle_t));

	handle->rxState = lState_Reset;
	handle->txState = lState_Reset;

	handle->callback = callback;
	handle->userData = userData;
}

lStatus_t HAL_UART_SendISR(uart_hal_instance_t instance,
						   uart_hal_handle_t *handle, uint8_t *txData,
						   uint32_t dataSize)
{
	assert(txData != NULL && dataSize != 0);

	LPC_UART_TypeDef *lpcInstance = NULL;

	handle->txState = lState_Set;

	handle->txData = txData;
	handle->txDataSize = 0;
	handle->txDataSizeAll = dataSize;

	prvUART_TransferHandle[instance] = handle;

	lpcInstance = prvUART_GetInstance(instance);

	if (!(lpcInstance->LSR & UART_LSR_THRE))
		return lStatus_Fail;

	UART_IntConfig(lpcInstance, UART_INTCFG_THRE, ENABLE);

	UART_SendByte(lpcInstance, *handle->txData);
	handle->txDataSize++;

	return lStatus_Success;
}

lStatus_t HAL_UART_ReceiveISR(uart_hal_instance_t instance,
							  uart_hal_handle_t *handle, uint8_t *rxData,
							  uint32_t dataSize)
{
	LPC_UART_TypeDef *lpcInstance = NULL;

	handle->rxState = lState_Set;

	handle->rxData = rxData;
	handle->rxDataSize = 0;
	handle->rxDataSizeAll = dataSize;

	prvUART_TransferHandle[instance] = handle;

	lpcInstance = prvUART_GetInstance(instance);

	UART_IntConfig(lpcInstance, UART_INTCFG_RBR, ENABLE);

	return lStatus_Success;
}

/************************************ IRQs ************************************/

/**
 *  @brief UART0 IRQ Handler.
 */
void UART0_IRQHandler(void)
{
	prvUART_HandleInterrupt(UART_HAL_INSTANCE_0, prvUART_TransferHandle[0]);
}

/**
 *  @brief UART1 IRQ Handler.
 */
void UART1_IRQHandler(void)
{
	prvUART_HandleInterrupt(UART_HAL_INSTANCE_1, prvUART_TransferHandle[1]);
}

/**
 *  @brief UART2 IRQ Handler.
 */
void UART2_IRQHandler(void)
{
	prvUART_HandleInterrupt(UART_HAL_INSTANCE_2, prvUART_TransferHandle[2]);
}

/**
 *  @brief UART3 IRQ Handler.
 */
void UART3_IRQHandler(void)
{
	prvUART_HandleInterrupt(UART_HAL_INSTANCE_3, prvUART_TransferHandle[3]);
}

/****************************** static functions ******************************/

/**
 *  @brief Handle UART Interrupt.
 *
 *  @param[in] instance UART Instance.
 *  @param[in] handle UART Transfer Handle.
 *  @return None.
 */
static void prvUART_HandleInterrupt(uart_hal_instance_t instance,
									uart_hal_handle_t *handle)
{
	LPC_UART_TypeDef *lpcInstance;
	uint32_t intSrc;
	uint8_t tmp;

	lpcInstance = prvUART_GetInstance(instance);
	intSrc = UART_GetIntId(lpcInstance);
	intSrc &= UART_IIR_INTID_MASK;

	switch (intSrc) {
	case UART_IIR_INTID_RLS:
		tmp = UART_GetLineStatus(lpcInstance); // Check line status
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI |
				UART_LSR_RXFE);
		// If any error exist
		if (tmp) {
			handle->callback(instance, handle, lStatus_Fail, handle->userData);
		}

		break;
	case UART_IIR_INTID_RDA:
	case UART_IIR_INTID_CTI:
		UART_IntConfig(lpcInstance, UART_INTCFG_RBR, DISABLE);

		*(handle->rxData + handle->rxDataSize) = UART_ReceiveByte(lpcInstance);
		handle->rxDataSize++;

		if (handle->rxDataSize < handle->rxDataSizeAll) {
			UART_IntConfig(lpcInstance, UART_INTCFG_RBR, ENABLE);
		} else {
			handle->rxState = lState_Reset;
			handle->callback(instance, handle, lStatus_Success,
							 handle->userData);
		}

		break;
	case UART_IIR_INTID_THRE:
		if (handle->txDataSize < handle->txDataSizeAll) {
			UART_IntConfig(lpcInstance, UART_INTCFG_THRE, ENABLE);
			UART_SendByte(lpcInstance, *(handle->txData + handle->txDataSize));
			handle->txDataSize++;
		} else {
			UART_IntConfig(lpcInstance, UART_INTCFG_THRE, DISABLE);
			handle->txState = lState_Reset;
			handle->callback(instance, handle, lStatus_Success,
							 handle->userData);
		}

		break;
	default:
		break;
	}
}

/**
 *  @brief Get LPC UART Instance.
 *
 *  @param[in] instance UART Instance.
 *  @return None.
 */
static inline LPC_UART_TypeDef *prvUART_GetInstance(uart_hal_instance_t UARTx)
{
	LPC_UART_TypeDef *lpcInstance = NULL;

	switch (UARTx) {
	case UART_HAL_INSTANCE_0:
		lpcInstance = (LPC_UART_TypeDef *)LPC_UART0;
		break;
	case UART_HAL_INSTANCE_1:
		lpcInstance = (LPC_UART_TypeDef *)LPC_UART1;
		break;
	case UART_HAL_INSTANCE_2:
		lpcInstance = (LPC_UART_TypeDef *)LPC_UART2;
		break;
	case UART_HAL_INSTANCE_3:
		lpcInstance = (LPC_UART_TypeDef *)LPC_UART3;
		break;
	default:
		break;
	}

	return lpcInstance;
}

/**
 *  @brief Initialize UART Pins.
 *
 *  @param[in] instance UART Instance.
 *  @return None.
 */
static void prvUART_InitPins(uart_hal_instance_t instance)
{
	assert(HAL_UART_2_PIN_OPTION < HAL_UART_2_PIN_OPTION_CNT);
	assert(HAL_UART_3_PIN_OPTION < HAL_UART_3_PIN_OPTION_CNT);

	PINSEL_CFG_Type pin_cfg;
	uint8_t func_num = PINSEL_FUNC_0;
	uint8_t port_num = PINSEL_PORT_0;
	uint8_t tx_pin = PINSEL_PIN_0;
	uint8_t rx_pin = PINSEL_PIN_0;

	switch (instance) {
	case UART_HAL_INSTANCE_0:
		func_num = PINSEL_FUNC_1;
		port_num = PINSEL_PORT_0;
		tx_pin = PINSEL_PIN_2;
		rx_pin = PINSEL_PIN_3;

		break;
	case UART_HAL_INSTANCE_1:
		func_num = PINSEL_FUNC_1;
		port_num = PINSEL_PORT_0;
		tx_pin = PINSEL_PIN_15;
		rx_pin = PINSEL_PIN_16;

		break;
	case UART_HAL_INSTANCE_2:
		if (HAL_UART_2_PIN_OPTION == 0) {
			func_num = PINSEL_FUNC_1;
			port_num = PINSEL_PORT_0;
			tx_pin = PINSEL_PIN_10;
			rx_pin = PINSEL_PIN_11;
		} else if (HAL_UART_2_PIN_OPTION == 1) {
			func_num = PINSEL_FUNC_2;
			port_num = PINSEL_PORT_2;
			tx_pin = PINSEL_PIN_8;
			rx_pin = PINSEL_PIN_9;
		}

		break;
	case UART_HAL_INSTANCE_3:
		if (HAL_UART_3_PIN_OPTION == 0) {
			func_num = PINSEL_FUNC_3;
			port_num = PINSEL_PORT_0;
			tx_pin = PINSEL_PIN_25;
			rx_pin = PINSEL_PIN_26;
		} else if (HAL_UART_3_PIN_OPTION == 1) {
			func_num = PINSEL_FUNC_3;
			port_num = PINSEL_PORT_4;
			tx_pin = PINSEL_PIN_28;
			rx_pin = PINSEL_PIN_29;
		}

		break;
	}

	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Funcnum = func_num;
	pin_cfg.Portnum = port_num;
	pin_cfg.Pinnum = tx_pin;
	PINSEL_ConfigPin(&pin_cfg); // TX
	pin_cfg.Pinnum = rx_pin;
	PINSEL_ConfigPin(&pin_cfg); // RX
}

/********************************* End Of File ********************************/