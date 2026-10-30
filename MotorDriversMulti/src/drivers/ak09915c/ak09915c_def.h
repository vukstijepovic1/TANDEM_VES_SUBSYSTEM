/**
 * @file    ak09915c_def.h
 * @brief   AK09915C Magnetometer driver defines.
 * @version 1.0.0
 * @date    02.08.2024
 * @author  LisumLab
 */

#ifndef AK09915C_DEF_H
#define AK09915C_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* Device information */
#define AK09915C_DEF_SENSITIVITY 0.15f // AK09915C sensor sensitivity

#define AK09915C_DEF_SLAVE_ADDR 0x0CU // I2C 7bit Slave address

#define AK09915C_DEF_COMPANY_ID 0x48U // Company ID
#define AK09915C_DEF_DEVICE_ID 0x10U  // Device ID

#define AK09915C_DEF_DIS_I2C 0x1BU // Disable I2C command

#define AK09915C_DEF_TWAIT_US 100 // Operational mode switch time [us]

/* Self-test Judgment data */
#define AK09915C_DEF_ST_X_MIN (int16_t)(-200)
#define AK09915C_DEF_ST_X_MAX (int16_t)(200)

#define AK09915C_DEF_ST_Y_MIN (int16_t)(-200)
#define AK09915C_DEF_ST_Y_MAX (int16_t)(200)

#define AK09915C_DEF_ST_Z_MIN (int16_t)(-800)
#define AK09915C_DEF_ST_Z_MAX (int16_t)(-200)

/* Register Addresses */
#define AK09915C_DEF_REG_WIA1 0x00U	  // Company ID - R
#define AK09915C_DEF_REG_WIA2 0x01U	  // Device ID - R
#define AK09915C_DEF_REG_RSV 0x02U	  // Reserved - R
#define AK09915C_DEF_REG_INFO 0x03U	  // Information - R
#define AK09915C_DEF_REG_ST1 0x10U	  // Status 1 - R
#define AK09915C_DEF_REG_HXL 0x11U	  // X-Axis Data L - R
#define AK09915C_DEF_REG_HXH 0x12U	  // X-Axis Data H - R
#define AK09915C_DEF_REG_HYL 0x13U	  // Y-Axis Data L - R
#define AK09915C_DEF_REG_HYH 0x14U	  // Y-Axis Data H - R
#define AK09915C_DEF_REG_HZL 0x15U	  // Z-Axis Data L - R
#define AK09915C_DEF_REG_HZH 0x16U	  // Z-Axis Data H - R
#define AK09915C_DEF_REG_TMPS 0x17U	  // Dummy - R
#define AK09915C_DEF_REG_ST2 0x18U	  // Status 2 - R
#define AK09915C_DEF_REG_CNTL1 0x30U  // Control 1 - RW
#define AK09915C_DEF_REG_CNTL2 0x31U  // Control 2 - RW
#define AK09915C_DEF_REG_CNTL3 0x32U  // Control 3 - RW
#define AK09915C_DEF_REG_TS1 0x33U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_TS2 0x34U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_TS3 0x35U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_I2CDIS 0x36U // I2C Disable - RW
#define AK09915C_DEF_REG_TS4 0x37U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_ASAX 0x60U	  // Dummy - R
#define AK09915C_DEF_REG_ASAY 0x61U	  // Dummy - R
#define AK09915C_DEF_REG_ASAZ 0x62U	  // Dummy - R
#define AK09915C_DEF_REG_TPH1 0xC0U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_TPH2 0xC1U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_RR 0xC2U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_SYT 0xC3U	  // Test - DON'T ACCESS - RW
#define AK09915C_DEF_REG_DT 0xC4U	  // Test - DON'T ACCESS - RW

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

#endif /* AK09915C_DEF_H */