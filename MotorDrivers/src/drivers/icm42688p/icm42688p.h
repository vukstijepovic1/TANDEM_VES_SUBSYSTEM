/**
 * @file    icm42688p.h
 * @brief   ICM42688P Accelerometer & Gyroscope Driver.
 * @version	1.0.0
 * @date    23.01.2025
 * @author  LisumLab
 */

#ifndef ICM42688P_H
#define ICM42688P_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "icm42688p_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief ICM42688P SPI Mode. */
typedef enum icm42688p_spi_mode_t {
	ICM42688P_SPI_MODE_0_3 = 0, //!< SPI 0,3 Mode
	ICM42688P_SPI_MODE_1_2 = 1	//!< SPI 1,2 Mode
} icm42688p_spi_mode_t;

/*! @brief ICM42688P Gyroscope full scale (FS) select. */
typedef enum icm42688p_gyro_fs_sel_t {
	ICM42688P_GYRO_FS_SEL_2000 = 0,	 //!< +- 2000 dps
	ICM42688P_GYRO_FS_SEL_1000 = 1,	 //!< +- 1000 dps
	ICM42688P_GYRO_FS_SEL_500 = 2,	 //!< +- 500 dps
	ICM42688P_GYRO_FS_SEL_250 = 3,	 //!< +- 250 dps
	ICM42688P_GYRO_FS_SEL_125 = 4,	 //!< +- 125 dps
	ICM42688P_GYRO_FS_SEL_62_5 = 5,	 //!< +- 62.5 dps
	ICM42688P_GYRO_FS_SEL_31_25 = 6, //!< +- 32.25 dps
	ICM42688P_GYRO_FS_SEL_15_625 = 7 //!< +- 15.625 dps
} icm42688p_gyro_fs_sel_t;

/*! @brief ICM42688P Gyroscope output data rate (ODR). */
typedef enum icm42688p_gyro_odr_t {
	ICM42688P_GYRO_ODR_0 = 0,	  //!< 0 Hz (Measurements are disabled)
	ICM42688P_GYRO_ODR_32000 = 1, //!< 32 KHz
	ICM42688P_GYRO_ODR_16000 = 2, //!< 16 KHz
	ICM42688P_GYRO_ODR_8000 = 3,  //!< 8 KHz
	ICM42688P_GYRO_ODR_4000 = 4,  //!< 4 KHz
	ICM42688P_GYRO_ODR_2000 = 5,  //!< 2 KHz
	ICM42688P_GYRO_ODR_1000 = 6,  //!< 1 KHz
	ICM42688P_GYRO_ODR_200 = 7,	  //!< 200 Hz
	ICM42688P_GYRO_ODR_100 = 8,	  //!< 100 Hz
	ICM42688P_GYRO_ODR_50 = 9,	  //!< 50 Hz
	ICM42688P_GYRO_ODR_25 = 10,	  //!< 25 Hz
	ICM42688P_GYRO_ODR_12_5 = 11, //!< 12.5 Hz
	ICM42688P_GYRO_ODR_500 = 15	  //!< 500 Hz
} icm42688p_gyro_odr_t;

/*! @brief ICM42688P Accelerometer full scale (FS) select. */
typedef enum icm42688p_accel_fs_sel_t {
	ICM42688P_ACCEL_FS_SEL_16 = 0, //!< +- 16g
	ICM42688P_ACCEL_FS_SEL_8 = 1,  //!< +- 8g
	ICM42688P_ACCEL_FS_SEL_4 = 2,  //!< +- 4g
	ICM42688P_ACCEL_FS_SEL_2 = 3   //!< +- 2g
} icm42688p_accel_fs_sel_t;

/*! @brief ICM42688P Accelerometer output data rate (ODR). */
typedef enum icm42688p_accel_odr_t {
	ICM42688P_ACCEL_ODR_0 = 0,	   //!< 0 Hz (Measurements are disabled)
	ICM42688P_ACCEL_ODR_32000 = 1, //!< 32 KHz
	ICM42688P_ACCEL_ODR_16000 = 2, //!< 16 KHz
	ICM42688P_ACCEL_ODR_8000 = 3,  //!< 8 KHz
	ICM42688P_ACCEL_ODR_4000 = 4,  //!< 4 KHz
	ICM42688P_ACCEL_ODR_2000 = 5,  //!< 2 KHz
	ICM42688P_ACCEL_ODR_1000 = 6,  //!< 1 KHz
	ICM42688P_ACCEL_ODR_200 = 7,   //!< 200 Hz
	ICM42688P_ACCEL_ODR_100 = 8,   //!< 100 Hz
	ICM42688P_ACCEL_ODR_50 = 9,	   //!< 50 Hz
	ICM42688P_ACCEL_ODR_25 = 10,   //!< 25 Hz
	ICM42688P_ACCEL_ODR_12_5 = 11, //!< 12.5 Hz
	ICM42688P_ACCEL_ODR_500 = 15   //!< 500 Hz
} icm42688p_accel_odr_t;

/*! @brief ICM42688P config. */
typedef struct icm42688p_cfg_t {
	icm42688p_spi_mode_t spi_mode;
	icm42688p_gyro_fs_sel_t gyro_fs;
	icm42688p_gyro_odr_t gyro_odr;
	icm42688p_accel_fs_sel_t accel_fs;
	icm42688p_accel_odr_t accel_odr;
} icm42688p_cfg_t;

/*! @brief ICM42688P int mode. */
typedef enum icm42688p_int_mode_t {
	ICM42688P_INT_MODE_PULSED = 0,
	ICM42688P_INT_MODE_LATCHED = 1
} icm42688p_int_mode_t;

/*! @brief ICM42688P interrupt drive circuit. */
typedef enum icm42688p_int_drive_circuit_t {
	ICM42688P_INT_DRIVE_CIRCUIT_OPEN_DRAIN = 0,
	ICM42688P_INT_DRIVE_CIRCUIT_PUSH_PULL = 1
} icm42688p_int_drive_circuit_t;

/*! @brief ICM42688P interrupt polarity. */
typedef enum icm42688p_int_polarity_t {
	ICM42688P_INT_POLARITY_ACTIVE_LOW = 0,
	ICM42688P_INT_POLARITY_ACTIVE_HIGH = 1
} icm42688p_int_polarity_t;

/*! @brief ICM42688P interrupt type. */
typedef enum icm42688p_int_type_t {
	ICM42688P_INT_TYPE_AGC_RDY = 0,
	ICM42688P_INT_TYPE_FIFO_FULL = 1,
	ICM42688P_INT_TYPE_FIFO_THS = 2,
	ICM42688P_INT_TYPE_DATA_RDY = 3,
	ICM42688P_INT_TYPE_RESET_DONE = 4,
	ICM42688P_INT_TYPE_PLL_RDY = 5,
	ICM42688P_INT_TYPE_UI_FSYNC = 6,
	ICM42688P_INT_TYPE_DISABLE = 7
} icm42688p_int_type_t;

/*! @brief ICM42688P interrupt config. */
typedef struct icm42688p_int_cfg_t {
	icm42688p_int_mode_t int_mode;
	icm42688p_int_drive_circuit_t int_drive;
	icm42688p_int_polarity_t int_polarity;
	icm42688p_int_type_t int_type;
} icm42688p_int_cfg_t;

/*! @brief ICM42688P interrupt status. */
typedef struct __attribute__((__packed__)) icm42688p_int_status_t {
	unsigned agc_rdy : 1;	 // AGC Ready interrupt.
	unsigned fifo_full : 1;	 // FIFO buffer is full.
	unsigned fifo_ths : 1;	 // FIFO buffer reaches the threshold value.
	unsigned data_rdy : 1;	 // Data Ready interrupt is generated.
	unsigned reset_done : 1; // Software reset is completed.
	unsigned pll_rdt : 1;	 // PLL Ready interrupt is generated.
	unsigned ui_fsync : 1;	 // UI FSYNC interrupt is generated.
	unsigned res : 1;		 // Reserved
} icm42688p_int_status_t;

/*! @brief ICM42688P device. */
typedef struct icm42688p_t {
	icm42688p_interface_t *interface; // Communication interface - Read only.

	float gyro_sens;  // Gyroscope sensitivity - Read only. [deg/s]
	float accel_sens; // Accelerometer sensitivity - Read only. [g]

	uint8_t bank; // Current bank - Read only.
} icm42688p_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize ICM42688P Accelerometer and Gyroscope sensor.
 *
 * @param[in] dev       ICM42688P device.
 * @param[in] interface Communication interface:
 *                          icm42688p_spi_interface_t
 * @param[in] cfg       ICM42688P device config.
 * @return Initialization status.
 */
lStatus_t icm42688p_init(icm42688p_t *dev, icm42688p_interface_t *interface,
						 icm42688p_cfg_t *cfg);

/**
 * @brief Configure ICM42688P Interrupt.
 *
 * @param[in] dev   ICM42688P device.
 * @param[in] pin1  ICM42688P INT1 Pin config.
 * @param[in] pin1  ICM42688P INT2 Pin config.
 * @return INT Config status.
 */
lStatus_t icm42688p_int_cfg(icm42688p_t *dev, icm42688p_int_cfg_t *pin1,
							icm42688p_int_cfg_t *pin2);

/**
 * @brief Read ICM42688P Die Temperature.
 *
 * @param[in] dev   ICM42688P device.
 * @param[out] temp Temperature [degC].
 * @return Read status.
 */
lStatus_t icm42688p_temp_get(icm42688p_t *dev, float *temp);

/**
 * @brief Read ICM42688P Gyroscope data.
 *
 * @param[in] dev   ICM42688P device.
 * @param[out] x    Gyroscope X axis data [deg/s].
 * @param[out] y    Gyroscope Y axis data [deg/s].
 * @param[out] z    Gyroscope Z axis data [deg/s].
 * @return Read status.
 */
lStatus_t icm42688p_gyro_get(icm42688p_t *dev, float *x, float *y, float *z);

/**
 * @brief Read ICM42688P Accelerometer data.
 *
 * @param[in] dev   ICM42688P device.
 * @param[out] x    Accelerometer X axis data [g].
 * @param[out] y    Accelerometer Y axis data [g].
 * @param[out] z    Accelerometer Z axis data [g].
 * @return Read status.
 */
lStatus_t icm42688p_accel_get(icm42688p_t *dev, float *x, float *y, float *z);

/**
 * @brief Read ICM42688P Interrupt status.
 *
 * @param[in] dev       ICM42688P device.
 * @param[out] status   Interrupt status.
 * @return Read status.
 */
lStatus_t icm42688p_int_status(icm42688p_t *dev,
							   icm42688p_int_status_t *status);

#ifdef __cplusplus
}
#endif

#endif /* ICM42688P_H */