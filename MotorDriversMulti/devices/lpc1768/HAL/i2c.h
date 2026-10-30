/**
 *	@file     i2c.h
 *  @brief    HAL I2C library.
 *  @details  v1.1
 *  @author   LisumLab
 */

#ifndef I2C_H
#define I2C_H

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

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief I2C Instance. */
typedef enum {
	I2C_HAL_INSTANCE_0 = 0,
	I2C_HAL_INSTANCE_1 = 1,
	I2C_HAL_INSTANCE_2 = 2,
} i2c_hal_instance_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize I2C
 *
 * @param instance: I2C Instance.
 * @param clockrate: Clockrate for I2C, expressed in Hz.
 * @retval
 */
lStatus_t HAL_I2C_Init(i2c_hal_instance_t instance, uint32_t clockrate);

/**
 * @brief Deinitialize I2C
 *
 * @param instance: I2C Instance.
 * @retval None
 */
void HAL_I2C_DeInit(i2c_hal_instance_t instance);

/**
 * @brief Function for writing data to the I2C line, Master -> Slave direction
 *
 * @param instance: I2C Instance.
 * @param slaveAddress: Address to the specific Slave.
 * @param pData: Pointer to the data send buffer.
 * @param size: Size of the data that will be sent.
 * @param timeoutMs: Transfer timeout [ms].
 * @return Transfer Status.
 */
lStatus_t HAL_I2C_MasterTransmit(i2c_hal_instance_t instance,
								 uint32_t slaveAddress, uint8_t *pData,
								 uint32_t size, uint32_t timeoutMs);

/**
 * @brief Function for reading data from the I2C line, Master <- Slave direction
 *
 * @param instance: I2C Instance.
 * @param slaveAddress: Address to the specific Slave.
 * @param pData: Pointer to the receive data buffer.
 * @param size: Size of the receive data buffer.
 * @param timeoutMs: Transfer timeout [ms].
 * @return Transfer Status.
 */
lStatus_t HAL_I2C_MasterReceive(i2c_hal_instance_t instance,
								uint32_t slaveAddress, uint8_t *pData,
								uint32_t size, uint32_t timeoutMs);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H */