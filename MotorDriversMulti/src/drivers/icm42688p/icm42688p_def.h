/**
 * @file    icm42688p_def.h
 * @brief   ICM42688P Accelerometer & Gyroscope Driver defines.
 * @version	1.0.0
 * @date    23.01.2025
 * @author  LisumLab
 */

#ifndef ICM42688P_DEF_H
#define ICM42688P_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Defines
 ******************************************************************************/

/* Create register macro. */
#define ICM42688P_DEF_CREATE_REG(addr, bank) (addr | (bank << 8))
/* Get register address macro. */
#define ICM42688P_DEF_GET_ADDR(reg) (reg & 0xFFU)
/* Get register bank macro. */
#define ICM42688P_DEF_GET_BANK(reg) ((reg >> 8) & 0xFFU)

/* ICM42688P I2C slave address. */
#define ICM42688P_DEF_SLAVE_ADDR 0x68
/* ICM42688P Who Am I register content.*/
#define ICM42688P_DEF_WHO_AM_I 0x47

/* Sotware reset period - ICM42688P Datasheet Revision 1.8 page 65. */
#define ICM42688P_DEF_SOFT_RESET_PERIOD_MS 1
/*
 * Gyroscope needs to be kept on for a minimum of 45ms when transitioning
 * from OFF to any other modes - ICM42688P Datasheet Revision 1.8 page 77
 */
#define ICM42688P_DEF_GYRO_TRANSITION_PERIOD_MS 45

/* ICM42688P Banks */
#define ICM42688P_DEF_BANK0 0x00
#define ICM42688P_DEF_BANK1 0x01
#define ICM42688P_DEF_BANK2 0x02
#define ICM42688P_DEF_BANK3 0x03
#define ICM42688P_DEF_BANK4 0x04

/* ICM42688P Register Bank 0 */
#define ICM42688P_DEF_DEVICE_CONFIG_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x11, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_DRIVE_CONFIG_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x13, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_CONFIG_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x14, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_CONFIG_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x16, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_TEMP_DATA1_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x1D, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_TEMP_DATA0_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x1E, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_DATA_X1_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x1F, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_DATA_X0_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x20, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_DATA_Y1_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x21, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_DATA_Y0_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x22, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_DATA_Z1_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x23, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_DATA_Z0_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x24, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_DATA_X1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x25, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_DATA_X0_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x26, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_DATA_Y1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x27, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_DATA_Y0_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x28, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_DATA_Z1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x29, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_DATA_Z0_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x2A, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_TMST_FSYNCH_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x2B, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_TMST_FSYNCL_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x2C, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_STATUS_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x2D, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_COUNTH_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x2E, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_COUNTL_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x2F, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_DATA_REG                                            \
	ICM42688P_DEF_CREATE_REG(0x30, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_APEX_DATA0_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x31, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_APEX_DATA1_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x32, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_APEX_DATA2_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x33, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_APEX_DATA3_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x34, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_APEX_DATA4_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x35, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_APEX_DATA5_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x36, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_STATUS2_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x37, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_STATUS3_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x38, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_SIGNAL_PATH_RESET_REG                                    \
	ICM42688P_DEF_CREATE_REG(0x4B, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INTF_CONFIG0_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x4C, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INTF_CONFIG1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x4D, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_PWR_MGMT0_REG                                            \
	ICM42688P_DEF_CREATE_REG(0x4E, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_CONFIG0_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x4F, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_CONFIG0_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x50, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_CONFIG1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x51, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_GYRO_ACCEL_CONFIG0_REG                                   \
	ICM42688P_DEF_CREATE_REG(0x52, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_ACCEL_CONFIG1_REG                                        \
	ICM42688P_DEF_CREATE_REG(0x53, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_TMST_CONFIG_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x54, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_APEX_CONFIG0_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x56, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_SMD_CONFIG_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x57, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_CONFIG1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x5F, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_CONFIG2_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x60, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_CONFIG3_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x61, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FSYNC_CONFIG_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x62, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_CONFIG0_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x63, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_CONFIG1_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x64, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_SOURCE0_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x65, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_SOURCE3_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x68, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_INT_SOURCE4_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x69, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_LOST_PKT0_REG                                       \
	ICM42688P_DEF_CREATE_REG(0x6C, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_FIFO_LOST_PKT1_REG                                       \
	ICM42688P_DEF_CREATE_REG(0x6D, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_SELF_TEST_CONFIG_REG                                     \
	ICM42688P_DEF_CREATE_REG(0x70, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_WHO_AM_I_REG                                             \
	ICM42688P_DEF_CREATE_REG(0x75, ICM42688P_DEF_BANK0)
#define ICM42688P_DEF_REG_BANK_SEL_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x76, ICM42688P_DEF_BANK0)

/* ICM42688P Register Bank 1 */
#define ICM42688P_DEF_SENSOR_CONFIG0_REG                                       \
	ICM42688P_DEF_CREATE_REG(0x03, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC2_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x0B, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC3_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x0C, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC4_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x0D, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC5_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x0E, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC6_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x0F, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC7_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x10, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC8_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x11, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC9_REG                                  \
	ICM42688P_DEF_CREATE_REG(0x12, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_GYRO_CONFIG_STATIC10_REG                                 \
	ICM42688P_DEF_CREATE_REG(0x13, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_XG_ST_DATA_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x5F, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_YG_ST_DATA_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x60, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_ZG_ST_DATA_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x61, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_TMSTVAL0_REG                                             \
	ICM42688P_DEF_CREATE_REG(0x62, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_TMSTVAL1_REG                                             \
	ICM42688P_DEF_CREATE_REG(0x63, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_TMSTVAL2_REG                                             \
	ICM42688P_DEF_CREATE_REG(0x64, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_INT_CONFIG4_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x7A, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_INT_CONFIG5_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x7B, ICM42688P_DEF_BANK1)
#define ICM42688P_DEF_INT_CONFIG6_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x7C, ICM42688P_DEF_BANK1)

/* ICM42688P Register Bank 2 */
#define ICM42688P_DEF_ACCEL_CONFIG_STATIC2_REG                                 \
	ICM42688P_DEF_CREATE_REG(0x03, ICM42688P_DEF_BANK2)
#define ICM42688P_DEF_ACCEL_CONFIG_STATIC3_REG                                 \
	ICM42688P_DEF_CREATE_REG(0x04, ICM42688P_DEF_BANK2)
#define ICM42688P_DEF_ACCEL_CONFIG_STATIC4_REG                                 \
	ICM42688P_DEF_CREATE_REG(0x05, ICM42688P_DEF_BANK2)
#define ICM42688P_DEF_XA_ST_DATA_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x3B, ICM42688P_DEF_BANK2)
#define ICM42688P_DEF_YA_ST_DATA_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x3C, ICM42688P_DEF_BANK2)
#define ICM42688P_DEF_ZA_ST_DATA_REG                                           \
	ICM42688P_DEF_CREATE_REG(0x3D, ICM42688P_DEF_BANK2)

/* ICM42688P Register Bank 3 */
#define ICM42688P_DEF_CLKDIV_REG                                               \
	ICM42688P_DEF_CREATE_REG(0x2A, ICM42688P_DEF_BANK3)

/* ICM42688P Register Bank 4 */
#define ICM42688P_DEF_APEX_CONFIG1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x40, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG2_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x41, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG3_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x42, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG4_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x43, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG5_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x44, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG6_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x45, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG7_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x46, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG8_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x47, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_APEX_CONFIG9_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x48, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_ACCEL_WOM_X_THR_REG                                      \
	ICM42688P_DEF_CREATE_REG(0x4A, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_ACCEL_WOM_Y_THR_REG                                      \
	ICM42688P_DEF_CREATE_REG(0x4B, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_ACCEL_WOM_Z_THR_REG                                      \
	ICM42688P_DEF_CREATE_REG(0x4C, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_INT_SOURCE6_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x4D, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_INT_SOURCE7_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x4E, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_INT_SOURCE8_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x4F, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_INT_SOURCE9_REG                                          \
	ICM42688P_DEF_CREATE_REG(0x50, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_INT_SOURCE10_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x51, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER0_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x77, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER1_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x78, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER2_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x79, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER3_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x7A, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER4_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x7B, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER5_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x7C, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER6_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x7D, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER7_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x7E, ICM42688P_DEF_BANK4)
#define ICM42688P_DEF_OFFSET_USER8_REG                                         \
	ICM42688P_DEF_CREATE_REG(0x7F, ICM42688P_DEF_BANK4)

/*
 * ICM42688P Gyro Sensitivity Scale Factor [(deg/s)/LSB]
 * ICM42688P Datasheet Revision 1.8 page 11.
 */
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_2000 (1.0f / 16.4f)
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_1000 (1.0f / 32.8f)
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_500 (1.0f / 65.5f)
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_250 (1.0f / 131.0f)
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_125 (1.0f / 262.0f)
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_62_5 (1.0f / 524.3f)
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_31_25 (1.0f / 1048.6f)
#define ICM42688P_DEF_GYRO_SENS_FS_SEL_15_625 (1.0f / 2097.2f)

/*
 * ICM42688P Accelerometer Sensitivity Scale Factor [(g)/LSB]
 * ICM42688P Datasheet Revision 1.8 page 12.
 */
#define ICM42688P_DEF_ACCEL_SENS_FS_SEL_16 (1.0f / 2048.0f)
#define ICM42688P_DEF_ACCEL_SENS_FS_SEL_8 (1.0f / 4096.0f)
#define ICM42688P_DEF_ACCEL_SENS_FS_SEL_4 (1.0f / 8192.0f)
#define ICM42688P_DEF_ACCEL_SENS_FS_SEL_2 (1.0f / 16384.0f)

/*
 * ICM42688P Temperature Factor
 * ICM42688P Datasheet Revision 1.8 page 67.
 */
#define ICM42688P_DEF_TEMP_FACTOR_16_BITS (1.0f / 132.48f)
#define ICM42688P_DEF_TEMP_FACTOR_8_BITS (1.0f / 2.07f)
#define ICM42688P_DEF_TEMP_OFFSET 25.0f

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

#endif /* ICM42688P_DEF_H */