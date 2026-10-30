/**
 * @file    loadcell6_def.h
 * @brief   Load Cell 6 Definitions.
 * @version 1.0.0
 * @date    17.10.2025
 * @author  LisumLab
 */

/*!
 * @file load6cell_def.h
 * @brief This file contains API for Load Cell 6 Click Driver adapted for
 * LPC1768.
 */

#ifndef LOADCELL6_DEF_H
#define LOADCELL6_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

// Standard library includes
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief Load Cell 6 description register.
 * @details Specified register for description of Load Cell 6 Click driver.
 */
#define LOADCELL6_REG_STAT     0x00
#define LOADCELL6_REG_CTRL1    0x01
#define LOADCELL6_REG_CTRL2    0x02
#define LOADCELL6_REG_CTRL3    0x03
#define LOADCELL6_REG_CTRL4    0x04
#define LOADCELL6_REG_CTRL5    0x05
#define LOADCELL6_REG_DATA     0x06
#define LOADCELL6_REG_SOC_SPI  0x07
#define LOADCELL6_REG_SCG_SPI  0x08
#define LOADCELL6_REG_SCOC_SPI 0x09
#define LOADCELL6_REG_SCGC_SPI 0x0A
#define LOADCELL6_REG_RAM      0x0C
#define LOADCELL6_REG_SYNC_SPI 0x0D
#define LOADCELL6_REG_SOC_ADC  0x15
#define LOADCELL6_REG_SCG_ADC  0x16
#define LOADCELL6_REG_SCOC_ADC 0x17
#define LOADCELL6_REG_SCGC_ADC 0x18

/**
 * @brief Load Cell 6 commands description.
 * @details Specified commands for description of Load Cell 6 Click driver.
 */
#define LOADCELL6_CMD_RATE0           0x01
#define LOADCELL6_CMD_RATE1           0x02
#define LOADCELL6_CMD_RATE2           0x04
#define LOADCELL6_CMD_RATE3           0x08
#define LOADCELL6_CMD_IMPD            0x10
#define LOADCELL6_CMD_CAL             0x20
#define LOADCELL6_CMD_START           0x80
#define LOADCELL6_CMD_REG_ACCESS_MODE 0x40

/**
 * @brief Load Cell 6 STAT register bits description.
 * @details Specified STAT register bits for description of Load Cell 6 Click
 * driver.
 */
#define LOADCELL6_STAT_RDY     0x0001
#define LOADCELL6_STAT_MSTAT   0x0002
#define LOADCELL6_STAT_DOR     0x0004
#define LOADCELL6_STAT_SYSGOR  0x0008
#define LOADCELL6_STAT_RATE0   0x0010
#define LOADCELL6_STAT_RATE1   0x0020
#define LOADCELL6_STAT_RATE2   0x0040
#define LOADCELL6_STAT_RATE3   0x0080
#define LOADCELL6_STAT_AOR     0x0100
#define LOADCELL6_STAT_RDERR   0x0200
#define LOADCELL6_STAT_PDSTAT0 0x0400
#define LOADCELL6_STAT_PDSTAT1 0x0800
#define LOADCELL6_STAT_ERROR   0x4000
#define LOADCELL6_STAT_INRESET 0x8000

#define LOADCELL6_63KSPS 0x05 // 63ksps
#define LOADCELL6_125SPS 0x06 // 125sps
#define LOADCELL6_250SPS 0x07 // 250sps
#define LOADCELL6_500SPS 0x08 // 500sps
#define LOADCELL6_1KSPS  0x09  // 1ksps
#define LOADCELL6_2KSPS  0x0A  // 2ksps
#define LOADCELL6_4KSPS  0x0B  // 4ksps
#define LOADCELL6_8KSPS  0x0C  // 8ksps
#define LOADCELL6_16KSPS 0x0D // 16ksps
#define LOADCELL6_32KSPS 0x0E // 32ksps
#define LOADCELL6_64KSPS 0x0F // 64ksps

/**
 * @brief Load Cell 6 CTRL1 register bits description.
 * @details Specified CTRL1 register bits for description of Load Cell 6 Click
 * driver.
 */
#define LOADCELL6_CTRL1_CONTSC 0x01
#define LOADCELL6_CTRL1_SCYCLE 0x02
#define LOADCELL6_CTRL1_FORMAT 0x04
#define LOADCELL6_CTRL1_U_B    0x08
#define LOADCELL6_CTRL1_PD0    0x10
#define LOADCELL6_CTRL1_PD1    0x20
#define LOADCELL6_CTRL1_SYNC   0x40
#define LOADCELL6_CTRL1_EXTCK  0x80

/**
 * @brief Load Cell 6 CTRL2 register bits description.
 * @details Specified CTRL2 register bits for description of Load Cell 6 Click
 * driver.
 */
#define LOADCELL6_CTRL2_PGAIN_x1   0x00
#define LOADCELL6_CTRL2_PGAIN_x2   0x01
#define LOADCELL6_CTRL2_PGAIN_x4   0x02
#define LOADCELL6_CTRL2_PGAIN_x8   0x03
#define LOADCELL6_CTRL2_PGAIN_x16  0x04
#define LOADCELL6_CTRL2_PGAIN_x32  0x05
#define LOADCELL6_CTRL2_PGAIN_x64  0x06
#define LOADCELL6_CTRL2_PGAIN_x128 0x07
#define LOADCELL6_CTRL2_PGAGEN     0x08
#define LOADCELL6_CTRL2_LPMODE     0x10
#define LOADCELL6_CTRL2_BUFEN      0x20
#define LOADCELL6_CTRL2_DGAIN_x1   0x00
#define LOADCELL6_CTRL2_DGAIN_x2   0x40
#define LOADCELL6_CTRL2_DGAIN_x4   0x80
#define LOADCELL6_CTRL2_DGAIN_x8   0xC0
#define LOADCELL6_CTRL2_DGAIN0     0x40
#define LOADCELL6_CTRL2_DGAIN1     0x80

/**
 * @brief Load Cell 6 CTRL3 register bits description.
 * @details Specified CTRL3 register bits for description of Load Cell 6 Click
 * driver.
 */
#define LOADCELL6_CTRL3_DATA32   0x08
#define LOADCELL6_CTRL3_MODBITS  0x10
#define LOADCELL6_CTRL3_ENMSYNC  0x20
#define LOADCELL6_CTRL3_RESERVED 0x41

/**
 * @brief Load Cell 6 CTRL4 register bits description.
 * @details Specified CTRL4 register bits for description of Load Cell 6 Click
 * driver.
 */
#define LOADCELL6_CTRL4_DIO1 0x01
#define LOADCELL6_CTRL4_DIO2 0x02
#define LOADCELL6_CTRL4_DIO3 0x04
#define LOADCELL6_CTRL4_DIO4 0x08
#define LOADCELL6_CTRL4_DIR1 0x10
#define LOADCELL6_CTRL4_DIR2 0x20
#define LOADCELL6_CTRL4_DIR3 0x40
#define LOADCELL6_CTRL4_DIR4 0x80

/**
 * @brief Load Cell 6 CTRL5 register bits description.
 * @details Specified CTRL5 register bits for description of Load Cell 6 Click
 * driver.
 */
#define LOADCELL6_CTRL5_NOSCO  0x01
#define LOADCELL6_CTRL5_NOSCG  0x02
#define LOADCELL6_CTRL5_NOSYSO 0x04
#define LOADCELL6_CTRL5_NOSYSG 0x08
#define LOADCELL6_CTRL5_CAL0   0x40
#define LOADCELL6_CTRL5_CAL1   0x80

/**
 * @brief Load Cell 6 weight data description.
 * @details Specified weight data for description of Load Cell 6 Click driver.
 */
#define LOADCELL6_DATA_NO_DATA  0
#define LOADCELL6_DATA_OK       1
#define LOADCELL6_WEIGHT_ZERO   0
#define LOADCELL6_WEIGHT_100G   100
#define LOADCELL6_WEIGHT_200G   200
#define LOADCELL6_WEIGHT_500G   500
#define LOADCELL6_WEIGHT_1000G  1000
#define LOADCELL6_WEIGHT_5000G  5000
#define LOADCELL6_WEIGHT_10000G 10000

#ifdef __cplusplus
}
#endif

#endif // LOADCELL6_DEF_H