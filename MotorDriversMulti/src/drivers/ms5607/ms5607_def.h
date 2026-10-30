/**
 * @file    ms5607_def.h
 * @brief   MS5607 Barometer driver defines.
 * @version 1.1.0
 * @date    29.01.2024
 * @author  LisumLab
 */

#ifndef MS5607_DEF_H
#define MS5607_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* Device information */
#define MS5607_DEF_SLAVE_ADDR 0x76U // I2C 7bit Slave address
#define MS5607_DEF_RESET_TIME_MS 3	// Device Reset Time [ms]

/* Conversion Timeout */
#define MS5607_CONV_256_TIME_US 600	  // Timeout for 256 OSR [us]
#define MS5607_CONV_512_TIME_US 1170  // Timeout for 512 OSR [us]
#define MS5607_CONV_1024_TIME_US 2280 // Timeout for 1024 OSR [us]
#define MS5607_CONV_2048_TIME_US 4540 // Timeout for 2048 OSR [us]
#define MS5607_CONV_4096_TIME_US 9040 // Timeout for 4096 OSR [us]

/* Commands */
#define MS5607_DEF_CMD_ADC_READ 0x00	 // ADC Read
#define MS5607_DEF_CMD_RESET 0x1E		 // Reset
#define MS5607_DEF_CMD_CONV_D1_256 0x40	 // Convert D1 (OSR = 256)
#define MS5607_DEF_CMD_CONV_D1_512 0x42	 // Convert D1 (OSR = 512)
#define MS5607_DEF_CMD_CONV_D1_1024 0x44 // Convert D1 (OSR = 1024)
#define MS5607_DEF_CMD_CONV_D1_2048 0x46 // Convert D1 (OSR = 2048)
#define MS5607_DEF_CMD_CONV_D1_4096 0x48 // Convert D1 (OSR = 4096)
#define MS5607_DEF_CMD_CONV_D2_256 0x50	 // Convert D2 (OSR = 256)
#define MS5607_DEF_CMD_CONV_D2_512 0x52	 // Convert D2 (OSR = 512)
#define MS5607_DEF_CMD_CONV_D2_1024 0x54 // Convert D2 (OSR = 1024)
#define MS5607_DEF_CMD_CONV_D2_2048 0x56 // Convert D2 (OSR = 2048)
#define MS5607_DEF_CMD_CONV_D2_4096 0x58 // Convert D2 (OSR = 4096)
#define MS5607_DEF_CMD_PROM_START 0xA0	 // PROM Read

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MS5607_DEF_H */