/**
 *	@file     spi.h
 *  @brief    HAL SPI library.
 *  @author   LisumLab
 */

#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef HAL_SPI_MAX_SLAVE_NUM
#define HAL_SPI_MAX_SLAVE_NUM 10
#endif

#ifndef HAL_SPI_NVIC_PRIORITY
#define HAL_SPI_NVIC_PRIORITY 6
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief SPI Instance. */
typedef enum {
	SPI_HAL_INSTANCE_0 = 0,
} spi_hal_instance_t;

/*! @brief SPI Data Size in bits. */
typedef enum {
	SPI_HAL_DATABIT_16 = 0,	 //!< Data is consisted of 16 bits
	SPI_HAL_DATABIT_8 = 8,	 //!< Data is consisted of 8 bits
	SPI_HAL_DATABIT_9 = 9,	 //!< Data is consisted of 9 bits
	SPI_HAL_DATABIT_10 = 10, //!< Data is consisted of 10 bits
	SPI_HAL_DATABIT_11 = 11, //!< Data is consisted of 11 bits
	SPI_HAL_DATABIT_12 = 12, //!< Data is consisted of 12 bits
	SPI_HAL_DATABIT_13 = 13, //!< Data is consisted of 13 bits
	SPI_HAL_DATABIT_14 = 14, //!< Data is consisted of 14 bits
	SPI_HAL_DATABIT_15 = 15	 //!< Data is consisted of 15 bits
} spi_hal_databit_t;

/*! @brief SPI Clock Phase. */
typedef enum {
	SPI_HAL_CPHA_FIRST = 0,		 //!< First clock edge
	SPI_HAL_CPHA_SECOND = 1 << 3 //!< Second clock edge
} spi_hal_cpha_t;

/*! @brief SPI Clock Polarity. */
typedef enum {
	SPI_HAL_CPOL_HI = 0,	 //!< High level
	SPI_HAL_CPOL_LO = 1 << 4 //!< Low level
} spi_hal_cpol_t;

/*! @brief SPI Operational Mode. */
typedef enum {
	SPI_HAL_SLAVE_MODE = 0,		 //!< Device operates as Slave
	SPI_HAL_MASTER_MODE = 1 << 5 //!< Device operates as Master
} spi_hal_mode_t;

/*! @brief SPI Data bit order. */
typedef enum {
	SPI_HAL_DATA_MSB_FIRST = 0,		//!< Data is transmitted MSB First
	SPI_HAL_DATA_LSB_FIRST = 1 << 6 //!< Data is transmitted LSB First
} spi_hal_data_order_t;

/*! @brief SPI Slave Pin Values. */
typedef enum {
	SPI_HAL_SLAVE_PIN_RESET = 0, //!< Low logical level
	SPI_HAL_SLAVE_PIN_SET = 1	 //!< High logical level
} spi_hal_ssel_pin_value_t;

/*! @brief SPI Slave ID Number. */
typedef uint8_t spi_hal_slave_id_t;

/*! @brief SPI Config Struct. */
typedef struct {
	spi_hal_databit_t databit;
	spi_hal_cpha_t cpha;
	spi_hal_cpol_t cpol;
	spi_hal_mode_t mode;
	spi_hal_data_order_t dataOrder;
	uint32_t clockRate;
	/** Clock rate,in Hz, should not exceed
			  (SPI peripheral clock)/8 */
} spi_hal_cfg_t;

/*! @brief SPI Slave information. */
typedef struct {
	uint8_t portNum;
	uint8_t pinNum;
} spi_hal_slave_t;

/*! @brief SPI Transfer Handle. */
typedef struct _spi_hal_handle spi_hal_handle_t;

/**
 * @brief Function prototype for SPI Transfer callback function
 *
 * @param instance: SPI Instance.
 * @param handle: SPI Transfer handle.
 * @param status: Transfer status.
 * @param userData: User Data.
 */
typedef void (*spi_hal_callback_t)(spi_hal_instance_t instance,
								   spi_hal_handle_t *handle, lStatus_t status,
								   void *userData);

/*! @brief SPI Transfer Handle. */
struct _spi_hal_handle {
	const uint8_t *volatile txData; //!< Transmit data
	uint8_t *volatile rxData;		//!< Receive data
	uint32_t counter;				//!< Number of current received/sent bytes
	uint32_t length;				//!< Transfer length

	spi_hal_callback_t callback; //!< Transfer callback
	void *userData;				 //!< Transfer callback user data

	volatile uint8_t txState; //!< Transmit state
	volatile uint8_t rxState; //!< Receive state
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize SPI.
 *
 * @param instance: SPI instance.
 * @param spiCfg: SPI configuration struct.
 * @retval Status.
 */
lStatus_t HAL_SPI_Init(spi_hal_instance_t instance, spi_hal_cfg_t *spiCfg);

/**
 * @brief Deinitialize SPI.
 *
 * @param instance: SPI instance.
 * @return None
 */
void HAL_SPI_DeInit(spi_hal_instance_t instance);

/**
 * @brief Set SPI clock.
 *
 * @param instance: SPI instance.
 * @param target_clock: Target clock [Hz].
 * @return None
 */
void HAL_SPI_SetClock(spi_hal_instance_t instance, const uint32_t target_clock);

/**
 * @brief Get default SPI configuration.
 *
 * @param spiCfg: SPI configuration struct
 * @return None
 */
void HAL_SPI_ConfigStructInit(spi_hal_cfg_t *spiCfg);

/**
 * @brief Initialize Slave Select Pin.
 *
 * @param instance: SPI Instance.
 * @param slave: SPI Slave information.
 * @retval SPI Slave ID.
 */
spi_hal_slave_id_t HAL_SPI_SlaveInit(spi_hal_instance_t instance,
									 spi_hal_slave_t *slave);

/**
 * @brief Set Slave Select Pin state.
 *
 * @param instance: SPI Instance.
 * @param slaveId: SPI Slave ID.
 * @param slavePinValue: Slave Select Pin value:
 *                       SPI_HAL_SLAVE_PIN_RESET -> SSEL_Pin_Level = LOW
 *                       SPI_HAL_SLAVE_PIN_SET -> SSEL_Pin_Level = HIGH
 * @return None
 */
void HAL_SPI_SlavePinState(spi_hal_instance_t instance,
						   spi_hal_slave_id_t slaveId,
						   spi_hal_ssel_pin_value_t slavePinValue);

/**
 * @brief Transmit data via SPI.
 *
 * @param instance: SPI instance.
 * @param txData: Transmit data.
 * @param txSize: Transmit data size.
 * @param timeoutMs: Timeout period [ms].
 * @retval Transfer Status.
 */
lStatus_t HAL_SPI_Transmit(spi_hal_instance_t instance, void *txData,
						   uint32_t txSize, uint32_t timeoutMs);

/**
 * @brief Receive data via SPI.
 *
 * @param instance: SPI instance.
 * @param rxData: Receive data.
 * @param txSize: Receive data size.
 * @param timeoutMs: Timeout period [ms].
 * @retval Transfer Status.
 */
lStatus_t HAL_SPI_Receive(spi_hal_instance_t instance, void *rxData,
						  uint32_t txSize, uint32_t timeoutMs);

/**
 * @brief Create SPI Transfer Handle.
 *
 * @param instance: SPI instance.
 * @param handle: SPI transfer handle.
 * @param callback: Transfer Callback.
 * @param userData: Transfer Callback user data.
 * @retval None
 */
void HAL_SPI_TransferCreateHandle(spi_hal_instance_t instance,
								  spi_hal_handle_t *handle,
								  spi_hal_callback_t callback, void *userData);

/**
 * @brief Transmit data via SPI ISR.
 *
 * @param instance: SPI instance.
 * @param handle: SPI transfer handle.
 * @param txData: Transmit data.
 * @param txSize: Transmit data size.
 * @retval Transfer Status.
 */
lStatus_t HAL_SPI_TransmitISR(spi_hal_instance_t instance,
							  spi_hal_handle_t *handle, uint8_t *txData,
							  uint32_t txSize);

/**
 * @brief Receive data via SPI ISR.
 *
 * @param instance: SPI instance.
 * @param handle: SPI transfer handle.
 * @param rxData: Receive data.
 * @param rxSize: Receive data size.
 * @retval Transfer Status.
 */
lStatus_t HAL_SPI_ReceiveISR(spi_hal_instance_t instance,
							 spi_hal_handle_t *handle, uint8_t *rxData,
							 uint32_t rxSize);

/** @todo: Add Abort Receive/Transmit & HAL_SPI_Transfer functions */

#ifdef __cplusplus
}
#endif

#endif /* SPI_H */