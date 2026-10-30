/**
 * @file    uart_tl.c
 * @brief   UART Transport Layer.
 * @version	1.0.0
 * @date    24.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "uart_tl.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* On recv function wrapper */
static int uart_tl_on_recv_wrap(void *tl, uint8_t *data, uint16_t size);

/* UART Callback */
void uart_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			 lStatus_t status, void *userData);

/*******************************************************************************
 * Code
 ******************************************************************************/

void uart_tl_init(uart_tl_t *tl, uart_hal_instance_t instance,
				  uint16_t rx_queue_size, uint16_t tx_queue_size)
{
	uart_tl_t *uart_tl;

	uart_tl = (uart_tl_t *)tl;

	uart_tl->instance = instance;
	uart_tl->queue_rx = xQueueCreate(rx_queue_size, sizeof(uint8_t));
	uart_tl->queue_tx = xQueueCreate(tx_queue_size, sizeof(uint8_t));
	uart_tl->mutex = xSemaphoreCreateMutex();

	HAL_UART_TransferCreateHandle(uart_tl->instance, &uart_tl->handle, uart_cb,
								  uart_tl);

	/* configure callbacks */
	uart_tl->tl.on_recv = NULL;
	uart_tl->tl.on_recv_arg = NULL;
	uart_tl->tl.send = uart_tl_send;

	HAL_UART_ReceiveISR(uart_tl->instance, &uart_tl->handle, &uart_tl->rx_byte,
						sizeof(uart_tl->rx_byte));
}

uint16_t uart_tl_send(void *tl, const uint8_t *data, uint16_t size)
{
	uart_tl_t *uart_tl;

	uart_tl = (uart_tl_t *)tl;

	if (uxQueueSpacesAvailable(uart_tl->queue_tx) < size) {
#ifdef UART_TL_DBG
		uart_tl->err_cnt++;
#endif
		return -1;
	}

	xSemaphoreTake(uart_tl->mutex, portMAX_DELAY);

	for (uint16_t i = 0; i < size; i++) {
		xQueueSendToBack(uart_tl->queue_tx, (data + i), 0);
	}

	xSemaphoreGive(uart_tl->mutex);

	return size;
}

void uart_tl_process(uart_tl_t *tl)
{
	uart_tl_t *uart_tl;
	uint8_t byte;

	uart_tl = (uart_tl_t *)tl;

	if (pdPASS == xQueuePeek(uart_tl->queue_tx, &byte, 0)) {
		if (lStatus_Success == HAL_UART_SendByte(uart_tl->instance, byte))
			xQueueReceive(uart_tl->queue_tx, &byte, 0);
	}

	if (pdPASS == xQueueReceive(uart_tl->queue_rx, &byte, 0))
		uart_tl_on_recv_wrap(uart_tl, &byte, sizeof(byte));
}

/****************************** static functions ******************************/

static int uart_tl_on_recv_wrap(void *tl, uint8_t *data, uint16_t size)
{
	uart_tl_t *uart_tl;

	uart_tl = (uart_tl_t *)tl;

	if (uart_tl->tl.on_recv == NULL)
		return 0;

	return uart_tl->tl.on_recv(uart_tl->tl.on_recv_arg, data, size);
}

void uart_cb(uart_hal_instance_t instance, uart_hal_handle_t *handle,
			 lStatus_t status, void *userData)
{
	BaseType_t taskWoken = pdFALSE;
	uart_tl_t *uart_tl;

	uart_tl = (uart_tl_t *)userData;

	if (handle->rxDataSize && status == lStatus_Success && !handle->rxState) {
		BaseType_t ret;

		ret = xQueueSendToBackFromISR(uart_tl->queue_rx, handle->rxData,
									  &taskWoken);
#ifdef UART_TL_DBG
		if (ret != pdPASS)
			uart_tl->err_cnt++;
#endif

		HAL_UART_ReceiveISR(instance, handle, handle->rxData, sizeof(uint8_t));
	}

	portYIELD_FROM_ISR(taskWoken);
}

/********************************* End Of File ********************************/