/**
 * @file    EPOS_def.h
 * @brief   EPOS Definitions.
 * @version 1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

#ifndef EPOS_DEF_H
#define EPOS_DEF_H

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

// EPOS Memory management magic
#define EPOS_DEF_SAVE_ALL_PARAMETERS_MAGIC 0x65766173U	  //!< 's' 'a' 'v' 'e'
#define EPOS_DEF_RESTORE_ALL_PARAMETERS_MAGIC 0x64616F6CU //!< 'l' 'o' 'a' 'd'

// EPOS PDO configuration
#define EPOS_DEF_PDO_VALID (uint32_t)(0 << 31)
#define EPOS_DEF_PDO_INVALID (uint32_t)(1 << 31)

#define EPOS_DEF_PDO_RTR_ALLOWED (uint32_t)(0 << 30)
#define EPOS_DEF_PDO_RTR_NOT_ALLOWED (uint32_t)(1 << 30)

#define EPOS_DEF_PDO_SYNCHRONOUS_TRANSMISSION (uint8_t)0x01
#define EPOS_DEF_PDO_ASYNCHRONOUS_ON_RTR_ONLY_TRANSMISSION (uint8_t)0xFD
#define EPOS_DEF_PDO_ASYNCHRONOUS_TRANSMISSION (uint8_t)0xFF

// EPOS Controlword
#define EPOS_DEF_CONTROLWORD_SHUTDOWN (uint16_t)(0b00000110)
#define EPOS_DEF_CONTROLWORD_SWITCH_ON (uint16_t)(0b00000111)
#define EPOS_DEF_CONTROLWORD_DISABLE_VOLTAGE (uint16_t)(0b00000000)
#define EPOS_DEF_CONTROLWORD_QUICK_STOP (uint16_t)(0b00000010)
#define EPOS_DEF_CONTROLWORD_DISABLE_OPERATION (uint16_t)(0b00000111)
#define EPOS_DEF_CONTROLWORD_ENABLE_OPERATION (uint16_t)(0b00001111)
#define EPOS_DEF_CONTROLWORD_FAULT_RESET (uint16_t)(0b10000000)

// EPOS Statusword state mask
#define EPOS_DEF_STATE_MASK (uint16_t)0x006F

// EPOS Unit Notation indices
#define EPOS_DEF_UNIT_DIMENSIONLESS 0x00
#define EPOS_DEF_UNIT_METER 0x01
#define EPOS_DEF_UNIT_KILOGRAM 0x02
#define EPOS_DEF_UNIT_SECOND 0x03
#define EPOS_DEF_UNIT_AMPERE 0x04
#define EPOS_DEF_UNIT_MINUTE 0x47
#define EPOS_DEF_UNIT_SQUARE_SECOND 0x57
#define EPOS_DEF_UNIT_REVOLUTIONS 0xB4
#define EPOS_DEF_UNIT_INCREMENTS 0xB5
#define EPOS_DEF_UNIT_STEPS 0xAC
#define EPOS_DEF_UNIT_RPM 0xC0

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

#endif /* EPOS_DEF_H */