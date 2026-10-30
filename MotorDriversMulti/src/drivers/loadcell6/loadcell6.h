/**
 * @file    loadcell6.h
 * @brief   Load Cell 6 Definitions.
 * @version 1.0.0
 * @date    17.10.2025
 * @author  LisumLab
 */

#ifndef LOADCELL6_H
#define LOADCELL6_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

// Standard library includes
#include <stdbool.h>
#include <stdint.h>

#include "loadcell6_def.h"

// LPC1768 HAL includes
#include "common.h"
#include "gpio.h"
#include "spi.h"
#include "util.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define LOADCELL6_CS_ASSERT(ctx)                                               \
	HAL_GPIO_SetPinValue((ctx)->cs_port, (ctx)->cs_pin, GPIO_HAL_RESET)
#define LOADCELL6_CS_DEASSERT(ctx)                                             \
	HAL_GPIO_SetPinValue((ctx)->cs_port, (ctx)->cs_pin, GPIO_HAL_SET)

/**
 * @brief RDY pin active level configuration.
 * Set to 1 if RDY is active-low (0 = ready), or 0 if active-high (1 = ready).
 */
#ifndef LOADCELL6_RDY_ACTIVE_LOW
#define LOADCELL6_RDY_ACTIVE_LOW 0
#endif

/*******************************************************************************
 * Typedef
 ******************************************************************************/

/**
 * @brief Load Cell 6 Click context object.
 * @details Context object definition of Load Cell 6 Click driver for LPC1768.
 */
typedef struct {

	// SPI configuration
	spi_hal_instance_t spi_instance; /**< SPI instance to use */
	uint32_t spi_clock;				 /**< SPI clock frequency */

	// GPIO pin configuration
	gpio_hal_instance_t rst_port; /**< Reset pin GPIO port */
	uint8_t rst_pin;			  /**< Reset pin number */

	gpio_hal_instance_t syn_port; /**< Sync pin GPIO port */
	uint8_t syn_pin;			  /**< Sync pin number */

	gpio_hal_instance_t rdy_port; /**< Ready pin GPIO port */
	uint8_t rdy_pin;			  /**< Ready pin number */

	gpio_hal_instance_t cs_port; /**< Chip select pin GPIO port */
	uint8_t cs_pin;				 /**< Chip select pin number */

	// SPI settings
	spi_hal_databit_t data_bits; /**< SPI data bits */
	spi_hal_cpha_t cpha;		 /**< SPI clock phase */
	spi_hal_cpol_t cpol;		 /**< SPI clock polarity */
	spi_hal_mode_t mode;		 /**< SPI mode */

	// Status
	bool initialized; /**< Initialization status */

} loadcell6_t;

/**
 * @brief Load Cell 6 Click load cell data object.
 * @details Load cell data object definition of Load Cell 6 Click driver.
 */
typedef struct {
	float tare;
	uint8_t tare_ok;
	float weight_coeff_100g;
	uint8_t weight_data_100g_ok;
	float weight_coeff_200g;
	uint8_t weight_data_200g_ok;
	float weight_coeff_500g;
	uint8_t weight_data_500g_ok;
	float weight_coeff_1000g;
	uint8_t weight_data_1000g_ok;
	float weight_coeff_5000g;
	uint8_t weight_data_5000g_ok;
	float weight_coeff_10000g;
	uint8_t weight_data_10000g_ok;
} loadcell6_data_t;

/**
 * @brief Load Cell 6 Click return value data.
 * @details Predefined enum values for driver return values.
 */
typedef enum {
	LOADCELL6_OK = 0,					/**< Success */
	LOADCELL6_ERROR = -1,				/**< General error */
	LOADCELL6_ERROR_SPI = -2,			/**< SPI communication error */
	LOADCELL6_ERROR_CONFIG = -3,		/**< Configuration error */
	LOADCELL6_ERROR_GPIO = -4,			/**< GPIO configuration error */
	LOADCELL6_ERROR_TIMEOUT = -5,		/**< Timeout error */
	LOADCELL6_ERROR_INVALID_PARAM = -6, /**< Invalid parameter */
	LOADCELL6_ERROR_INVALID_ARG = -7	/**< Invalid argument */
} loadcell6_return_value_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

loadcell6_return_value_t loadcell6_set_rate_speed(loadcell6_t* ctx,
												  uint8_t rate);

loadcell6_return_value_t loadcell6_set_rate_contsc(loadcell6_t* ctx,
												   uint8_t rate);

loadcell6_return_value_t loadcell6_read_data(loadcell6_t* ctx, uint8_t reg,
											 uint8_t* data_out);

/**
 * @brief Load Cell 6 initialization function.
 * @details This function initializes all necessary pins and peripherals
 * used for this Click board on LPC1768.
 * @param[out] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @return @li @c LOADCELL6_OK - Success,
 *         @li @c LOADCELL6_ERROR - Error.
 *         @li @c LOADCELL6_ERROR_INVALID_SPI - Invalid SPI instance.
 *         @li @c LOADCELL6_ERROR_INVALID_ARG - Invalid argument.
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_init(loadcell6_t* ctx);

/**
 * @brief Load Cell 6 default configuration function.
 * @details This function executes a default configuration of Load Cell 6
 * Click board for LPC1768.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @return @li @c LOADCELL6_OK - Success,
 *         @li @c LOADCELL6_ERROR - Error.
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note This function sets up the device with default operating parameters.
 */
loadcell6_return_value_t loadcell6_default_cfg(loadcell6_t* ctx);

/**
 * @brief Load Cell 6 data writing function.
 * @details This function writes a desired number of data bytes starting from
 * the selected register by using SPI serial interface.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] reg : Start register address.
 * @param[in] data_in : Data to be written.
 * @param[in] len : Number of bytes to be written.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_generic_write(loadcell6_t* ctx, uint8_t reg,
												 uint8_t* data_in, uint8_t len);

/**
 * @brief Load Cell 6 data reading function.
 * @details This function reads a desired number of data bytes starting from
 * the selected register by using SPI serial interface.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] reg : Start register address.
 * @param[out] data_out : Output read data.
 * @param[in] len : Number of bytes to be read.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_generic_read(loadcell6_t* ctx, uint8_t reg,
												uint8_t* data_out, uint8_t len);

/**
 * @brief Load Cell 6 hw reset function.
 * @details This function executes a hardware reset
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_hw_reset(loadcell6_t* ctx);

/**
 * @brief Load Cell 6 get sync function.
 * @details This function get sync state
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6  Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @return @li @c  LOADCELL6_OK - Low pin state,
 *         @li @c  LOADCELL6_ERROR - High pin state.
 *
 * @note None.
 */
uint8_t loadcell6_sync(loadcell6_t* ctx);

/**
 * @brief Load Cell 6 check data ready function.
 * @details This function check data ready state
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @return @li @c  LOADCELL6_OK - Low pin state,
 *         @li @c  LOADCELL6_ERROR - High pin state.
 *
 * @note None.
 */
uint8_t loadcell6_check_data_ready(loadcell6_t* ctx);

/**
 * @brief Load Cell 6 write reg function.
 * @details This function writes a byte of the data to the selected register
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] reg : Start register address.
 * @param[in] data_in : Data to be written.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_write_reg(loadcell6_t* ctx, uint8_t reg,
											 uint8_t data_in);

/**
 * @brief Load Cell 6 read reg function.
 * @details This function reads a byte of data from the selected register
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] reg : Start register address.
 * @param[out] data_out : Output read data.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_read_reg(loadcell6_t* ctx, uint8_t reg,
											uint8_t* data_out);

/**
 * @brief Load Cell 6 read reg data function.
 * @details This function writes a 24-bit data to the selected register
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] reg : Start register address.
 * @param[in] data_in : Data to be written.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_write_reg_data(loadcell6_t* ctx, uint8_t reg,
												  uint32_t data_in);

/**
 * @brief Load Cell 6 read reg data function.
 * @details This function reads a 24-bit data from the selected register
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] reg : Start register address.
 * @param[out] data_out : Output read data.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_read_reg_data(loadcell6_t* ctx, uint8_t reg,
												 uint32_t* data_out);

/**
 * @brief Load Cell 6 set command function.
 * @details This function writes a desired commands
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] cmd : Desired command.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_set_command(loadcell6_t* ctx, uint8_t cmd);

/**
 * @brief Load Cell 6 get status function.
 * @details This function reads status
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[out] status : Status data.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_get_status(loadcell6_t* ctx,
											  uint16_t* status);

/**
 * @brief Load Cell 6 read ADC data function.
 * @details This function reads the ADC data
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[out] data_out : ADC data.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_read_adc(loadcell6_t* ctx,
											uint32_t* data_out);

/**
 * @brief Load Cell 6 tare the scales function.
 * @details This function for the tare scales sets the tara value into the cell
 * data structure of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma
 * ADC with Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[out] cell_data : Pointer to the memory location of the structure where
 * data be stored.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_tare(loadcell6_t* ctx,
										loadcell6_data_t* cell_data);

/**
 * @brief Load Cell 6 calibration function.
 * @details This function for the calibration sets the calibration value into
 * the cell data structure of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps
 * Delta-Sigma ADC with Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[in] cal_val : Etalon weight value
 *         @li @c   100 ( LOADCELL6_WEIGHT_100G )   :   100g etalon,
 *         @li @c   200 ( LOADCELL6_WEIGHT_200G )   :   200g etalon,
 *         @li @c   500 ( LOADCELL6_WEIGHT_500G )   :   500g etalon,
 *         @li @c  1000 ( LOADCELL6_WEIGHT_1000G )  :  1000g etalon,
 *         @li @c  5000 ( LOADCELL6_WEIGHT_5000G )  :  5000g etalon,
 *         @li @c 10000 ( LOADCELL6_WEIGHT_10000G ) : 10000g etalon.
 * @param[out] cell_data : Pointer to the memory location of the structure where
 * data be stored.
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_calibration(loadcell6_t* ctx,
											   uint16_t cal_val,
											   loadcell6_data_t* cell_data);

/**
 * @brief Load Cell 6 get weight function.
 * @details This function gets float value of the scales measurements [ g ]
 * of the MAX11270 24-Bit, 10mW, 130dB SNR, 64ksps Delta-Sigma ADC with
 * Integrated PGA on the Load Cell 6 Click board™.
 * @param[in] ctx : Click context object.
 * See #loadcell6_t object definition for detailed explanation.
 * @param[out] cell_data : Pointer to the memory location of the structure where
 * data be stored.
 * @param[out] weight_g : Float value of the scales measurements [ g ].
 * @return @li @c  LOADCELL6_OK - Success,
 *         @li @c  LOADCELL6_ERROR - Error.
 *
 * See #loadcell6_return_value_t definition for detailed explanation.
 * @note None.
 */
loadcell6_return_value_t loadcell6_get_weight(loadcell6_t* ctx,
											  loadcell6_data_t* cell_data,
											  float* weight_g);

#ifdef __cplusplus
}
#endif

#endif // LOAD6CELL_H