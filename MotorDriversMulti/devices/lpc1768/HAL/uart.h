/**
 *	@file     uart.h
 *  @brief    HAL UART library.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef UART_H
#define UART_H

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

#ifndef HAL_UART_2_PIN_OPTION
/*! UART2 Pin Option:
 *    0 - Pin 0.10 is used as TX
 *        Pin 0.11 is used as RX
 *    1 - Pin 2.8 is used as TX
 *        Pin 2.9 is used as RX
 */
#define HAL_UART_2_PIN_OPTION 0
#endif

#ifndef HAL_UART_3_PIN_OPTION
/*! UART3 Pin Option:
 *   0 - Pin 0.25 is used as TX
 *       Pin 0.26 is used as RX
 *   1 - Pin 4.28 is used as TX
 *       Pin 4.29 is used as RX
 */
#define HAL_UART_3_PIN_OPTION 0
#endif

#ifndef HAL_UART_NVIC_PRIORITY
/*! NVIC Priority */
#define HAL_UART_NVIC_PRIORITY 7
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief UART Instance. */
typedef enum {
	UART_HAL_INSTANCE_0 = 0,
	UART_HAL_INSTANCE_1 = 1,
	UART_HAL_INSTANCE_2 = 2,
	UART_HAL_INSTANCE_3 = 3
} uart_hal_instance_t;

/*! @brief UART Parity type. */
typedef enum {
	UART_HAL_PARITY_NONE = 0, //!< No parity
	UART_HAL_PARITY_ODD = 1,  //!< Odd parity
	UART_HAL_PARITY_EVEN = 2, //!< Even parity
	UART_HAL_PARITY_SP_1 = 3, //!< Forced "1" stick parity
	UART_HAL_PARITY_SP_0 = 4  //!< Forced "0" stick parity
} uart_hal_parity_t;

/*! @brief UART Number of Data Bits. */
typedef enum {
	UART_HAL_DATABIT_5 = 0, //!< 5 Data Bits.
	UART_HAL_DATABIT_6 = 1, //!< 6 Data Bits.
	UART_HAL_DATABIT_7 = 2, //!< 7 Data Bits.
	UART_HAL_DATABIT_8 = 3	//!< 8 Data Bits.
} uart_hal_databit_t;

/*! @brief Number of Stop Bits used by UART. */
typedef enum {
	UART_HAL_STOPBIT_1 = 0, //!< 1 Stop Bit
	UART_HAL_STOPBIT_2 = 1	//!< 2 Stop Bits
} uart_hal_stopbit_t;

/*! @brief UART FIFO trigger level. */
typedef enum {
	UART_HAL_FIFO_TRIGGER_LEVEL_0 = 0, //!< FIFO trigger level 0: 1 character
	UART_HAL_FIFO_TRIGGER_LEVEL_1 = 1, //!< FIFO trigger level 1: 4 characters
	UART_HAL_FIFO_TRIGGER_LEVEL_2 = 2, //!< FIFO trigger level 2: 8 characters
	UART_HAL_FIFO_TRIGGER_LEVEL_3 = 3  //!< FIFO trigger level 3: 14 characters
} uart_hal_fifo_trigger_level_t;

/*! @brief UART config struc.t */
typedef struct {
	uint32_t baudrate;			 //!< UART Baud rate
	uart_hal_parity_t parity;	 //!< UART Parity Type
	uart_hal_databit_t databits; //!< UART Number of Data Bits
	uart_hal_stopbit_t stopbits; //!< UART Number of Stop Bits
} uart_hal_cfg_t;

/*! @brief UART FIFO config struct. */
typedef struct {
	lFunctionalState_t
		resetRxBuf; /**< reset RX FIFO command state:
		  - lFunctionalState_Enable - Reset RX FIFO in UART
		  - lFunctionalState_Enable - Do not reset RX FIFO in UART
		  */

	lFunctionalState_t resetTxBuf; /**< reset TX FIFO command state:
					   - lFunctionalState_Enable - Reset TX FIFO in UART
					   - lFunctionalState_Disable - Do not reset TX FIFO in UART
					   */
	lFunctionalState_t DMAMode;	   /**< DMA mode:
						 - lFunctionalState_Enable - Enable DMA
						 - lFunctionalState_Disable - Disable DMA
						 */
	uart_hal_fifo_trigger_level_t
		triggerLevel; /**< Rx FIFO trigger level:
- UART_HAL_FIFO_TRIGGER_LEVEL_0: UART FIFO trigger level 0: 1 character
- UART_HAL_FIFO_TRIGGER_LEVEL_1: UART FIFO trigger level 1: 4 character
- UART_HAL_FIFO_TRIGGER_LEVEL_2: UART FIFO trigger level 2: 8 character
- UART_HAL_FIFO_TRIGGER_LEVEL_3: UART FIFO trigger level 3: 14 character
*/
} uart_hal_fifo_cfg_t;

typedef struct _uart_hal_handle uart_hal_handle_t;

/*! @brief UART Callback. */
typedef void (*uart_hal_callback_t)(uart_hal_instance_t instance,
									uart_hal_handle_t *handle, lStatus_t status,
									void *userData);

/*! @brief UART Handle. */
struct _uart_hal_handle {
	const uint8_t *volatile txData; //!< Pointer to tx data
	volatile uint32_t txDataSize;	//!< Size of transmitted data
	uint32_t txDataSizeAll;			//!< Size of data to be transmitted
	uint8_t *volatile rxData;		//!< Pointer to rx data
	volatile uint32_t rxDataSize;	//!< Size of received data
	uint32_t rxDataSizeAll;			//!< Size of data to be received

	uart_hal_callback_t callback; //!< UART user callback
	void *userData;				  //!< User Data

	volatile uint8_t txState; //!< Is data being transmitted
	volatile uint8_t rxState; //!< Is data being received
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 *  @brief Initialize UART.
 *  @param[in] instance UART Instance.
 *  @param[in] uartCfg UART config struct.
 *  @return None
 */
void HAL_UART_Init(uart_hal_instance_t instance, uart_hal_cfg_t *uartCfg);

/**
 *  @brief Initialize RS232.
 *  @note Valid only for UART_HAL_INSTANCE_1
 *  @param[in] instance UART Instance.
 *  @param[in] uartCfg UART config struct.
 *  @return None
 */
void HAL_UART_RS232Init(uart_hal_instance_t instance, uart_hal_cfg_t *uartCfg);

/**
 *  @brief Deinitialize UART.
 *  @param[in] instance UART instance
 *  @return None
 */
void HAL_UART_DeInit(uart_hal_instance_t instance);

/**
 *  @brief Initialize UART Config struct to default values.
 *  @param[in] uartCfg UART config struct.
 *  @return None
 */
void HAL_UART_ConfigStructInit(uart_hal_cfg_t *uartCfg);

/**
 *  @brief Function for FIFO initialization.
 *  @param[in] instance UART instance.
 *  @param[in] fifoCfg Fifo config struct.
 *  @return None
 */
void HAL_UART_ConfigureFIFO(uart_hal_instance_t instance,
							uart_hal_fifo_cfg_t *fifoCfg);

/**
 *  @brief Initialize UART FIFO struct.
 *  @param[in] fifoCfg Fifo config struct.
 *  @return None
 */
void HAL_UART_FIFOConfigStructInit(uart_hal_fifo_cfg_t *fifoCfg);

/**
 *  @brief Send byte via UART.
 *  @param[in] instance UART Instance.
 *  @param[in] txByte Transmit byte.
 *  @return Transfer status.
 */
lStatus_t HAL_UART_SendByte(uart_hal_instance_t instance, const uint8_t txByte);

/**
 *  @brief Receive byte via UART.
 *  @param[in] instance UART Instance.
 *  @param[out] rxByte Receive byte.
 *  @return Transfer status.
 */
lStatus_t HAL_UART_ReceiveByte(uart_hal_instance_t instance, uint8_t *rxByte);

/**
 *  @brief Send data via UART.
 *  @param[in] instance UART instance.
 *  @param[in] txData Transmit data.
 *  @param[in] dataSize Transmit data size.
 *  @param[in] timeoutMs Transfer timeout [ms].
 *  @return Transfer status.
 */
lStatus_t HAL_UART_Send(uart_hal_instance_t instance, uint8_t *txData,
						uint32_t dataSize, uint32_t timeoutMs);

/**
 *  @brief Receive data via UART.
 *  @param[in] instance UART instance.
 *  @param[in] rxData Receive data.
 *  @param[in] dataSize Receive data size.
 *  @param[in] timeoutMs Transfer timeout [ms].
 *  @return Transfer status.
 */
lStatus_t HAL_UART_Receive(uart_hal_instance_t instance, uint8_t *rxData,
						   uint32_t dataSize, uint32_t timeoutMs);

/**
 *  @brief Enable Global UART interrupt.
 *  @param[in] instance UART instance.
 */
void HAL_UART_EnableInterrupt(uart_hal_instance_t instance);

/**
 *  @brief Create UART transfer handle.
 *  @param[in] instance UART instance.
 *  @param[in] handle UART Transfer handle.
 *  @param[in] callback User UART callback
 *  @param[in] userData User data (which shall be passed to callback)
 *  @return None
 */
void HAL_UART_TransferCreateHandle(uart_hal_instance_t instance,
								   uart_hal_handle_t *handle,
								   uart_hal_callback_t callback,
								   void *userData);

/**
 *  @brief Send data using ISR
 *  @param[in] instance UART instance
 *  @param[in] handle Pointer to the UART transfer handle
 *  @param[in] txData pointer to transmit data
 *  @param[in] dataSize transmit data size
 *  @return Transfer status.
 */
lStatus_t HAL_UART_SendISR(uart_hal_instance_t instance,
						   uart_hal_handle_t *handle, uint8_t *txData,
						   uint32_t dataSize);

/**
 * @brief Receive data using Interrupt transfer
 * @param[in] instance: UART instance
 * @param[in] handle: Pointer to the UART transfer handle
 * @param[in] rxData: pointer to receive data
 * @param[in] dataSize: transmit data size
 * @return Transfer status.
 */
lStatus_t HAL_UART_ReceiveISR(uart_hal_instance_t instance,
							  uart_hal_handle_t *handle, uint8_t *rxData,
							  uint32_t dataSize);

#ifdef __cplusplus
}
#endif

#endif /* UART_H */