/**
 * @file    bms_def.h
 * @brief   Daly BMS driver defines.
 * @version	1.0.0
 * @date    18.05.2025
 * @author  LisumLab
 */

#ifndef BMS_DEF_H
#define BMS_DEF_H

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

/* Daly BMS MODBUS Slave Address */
#define BMS_DEF_SLAVE_ADDR 0xD2
/* MODBUS Read command ID */
#define BMS_DEF_READ_CMD 0x03
/* MODBUS Write command ID */
#define BMS_DEF_WRITE_CMD 0x06
/* MODBUS Multi Write command ID */
#define BMS_DEF_WRITE_MULTI_CMD 0x10

/* Daly BMS registers */
#define BMS_DEF_REG_ADDR_BAT_1_VOLT 0x00
#define BMS_DEF_REG_ADDR_BAT_2_VOLT 0x01
#define BMS_DEF_REG_ADDR_BAT_3_VOLT 0x02
#define BMS_DEF_REG_ADDR_BAT_4_VOLT 0x03
#define BMS_DEF_REG_ADDR_BAT_5_VOLT 0x04
#define BMS_DEF_REG_ADDR_BAT_6_VOLT 0x05
#define BMS_DEF_REG_ADDR_BAT_7_VOLT 0x06
#define BMS_DEF_REG_ADDR_BAT_8_VOLT 0x07
#define BMS_DEF_REG_ADDR_BAT_9_VOLT 0x08
#define BMS_DEF_REG_ADDR_BAT_10_VOLT 0x09
#define BMS_DEF_REG_ADDR_BAT_11_VOLT 0x0A
#define BMS_DEF_REG_ADDR_BAT_12_VOLT 0x0B
#define BMS_DEF_REG_ADDR_BAT_13_VOLT 0x0C
#define BMS_DEF_REG_ADDR_BAT_14_VOLT 0x0D
#define BMS_DEF_REG_ADDR_TEMP 0x20
#define BMS_DEF_REG_ADDR_SUM_VOLT 0x28
#define BMS_DEF_REG_ADDR_CURR 0x29
#define BMS_DEF_REG_ADDR_STATE_OF_CHARGE_RO 0x2A
#define BMS_DEF_REG_ADDR_MAX_VOLT 0x2B
#define BMS_DEF_REG_ADDR_MIN_VOLT 0x2C
#define BMS_DEF_REG_ADDR_MAX_MONOMER_TEMP 0x2D
#define BMS_DEF_REG_ADDR_MIN_MONOMER_TEMP 0x2E
#define BMS_DEF_REG_ADDR_CHARGE_STATE 0x2F
#define BMS_DEF_REG_ADDR_REMAINING_CAPACITY 0x30
#define BMS_DEF_REG_ADDR_NUM_OF_BAT 0x31
#define BMS_DEF_REG_ADDR_NUM_OF_TEMP_SENSORS 0x32
#define BMS_DEF_REG_ADDR_CYCLES 0x33
#define BMS_DEF_REG_ADDR_EQUILIBRIUM_STAT 0x34
#define BMS_DEF_REG_ADDR_CHARGE_MOS_STAT 0x35
#define BMS_DEF_REG_ADDR_DISCHARGE_MOS_STAT 0x36
#define BMS_DEF_REG_ADDR_AVG_VOLT 0x37
#define BMS_DEF_REG_ADDR_DIFF_VOLT 0x38
#define BMS_DEF_REG_ADDR_POWER 0x39
#define BMS_DEF_REG_ADDR_FAULT_STAT_1 0x3A
#define BMS_DEF_REG_ADDR_FAULT_STAT_2 0x3B
#define BMS_DEF_REG_ADDR_FAULT_STAT_3 0x3C
#define BMS_DEF_REG_ADDR_FAULT_STAT_4 0x3D
#define BMS_DEF_REG_ADDR_RATED_CAPACITY 0x80
#define BMS_DEF_REG_ADDR_CELL_REF_VOLT 0x81
#define BMS_DEF_REG_ADDR_COLLECT_BOARD_NUM 0x82
#define BMS_DEF_REG_ADDR_BOARD_1_CELL_NUM 0x83
#define BMS_DEF_REG_ADDR_BOARD_2_CELL_NUM 0x84
#define BMS_DEF_REG_ADDR_BOARD_3_CELL_NUM 0x85
#define BMS_DEF_REG_ADDR_BOARD_1_TEMP_NUM 0x86
#define BMS_DEF_REG_ADDR_BOARD_2_TEMP_NUM 0x87
#define BMS_DEF_REG_ADDR_BOARD_3_TEMP_NUM 0x88
#define BMS_DEF_REG_ADDR_TYPE_OF_BAT 0x89
#define BMS_DEF_REG_ADDR_SLEEP_WAIT_TIME 0x8A
#define BMS_DEF_REG_ADDR_CELL_VOLT_HIGH_PROT_LEV1 0x8B
#define BMS_DEF_REG_ADDR_CELL_VOLT_HIGH_PROT_LEV2 0x8C
#define BMS_DEF_REG_ADDR_CELL_VOLT_LOW_PROT_LEV1 0x8D
#define BMS_DEF_REG_ADDR_CELL_VOLT_LOW_PROT_LEV2 0x8E
#define BMS_DEF_REG_ADDR_SUM_VOLT_HIGH_PROT_LEV1 0x8F
#define BMS_DEF_REG_ADDR_SUM_VOLT_HIGH_PROT_LEV2 0x90
#define BMS_DEF_REG_ADDR_SUM_VOLT_LOW_PROT_LEV1 0x91
#define BMS_DEF_REG_ADDR_SUM_VOLT_LOW_PROT_LEV2 0x92
#define BMS_DEF_REG_ADDR_CHARGE_OVERCURR_PROT_LEV1 0x93
#define BMS_DEF_REG_ADDR_CHARGE_OVERCURR_PROT_LEV2 0x94
#define BMS_DEF_REG_ADDR_DISCHARGE_OVERCURR_PROT_LEV1 0x95
#define BMS_DEF_REG_ADDR_DISCHARGE_OVERCURR_PROT_LEV2 0x96
#define BMS_DEF_REG_ADDR_CHARGE_HIGH_TEMP_PROT_LEV1 0x97
#define BMS_DEF_REG_ADDR_CHARGE_HIGH_TEMP_PROT_LEV2 0x98
#define BMS_DEF_REG_ADDR_CHARGE_LOW_TEMP_PROT_LEV1 0x99
#define BMS_DEF_REG_ADDR_CHARGE_LOW_TEMP_PROT_LEV2 0x9A
#define BMS_DEF_REG_ADDR_DISCHARGE_HIGH_TEMP_PROT_LEV1 0x9B
#define BMS_DEF_REG_ADDR_DISCHARGE_HIGH_TEMP_PROT_LEV2 0x9C
#define BMS_DEF_REG_ADDR_DISCHARGE_LOW_TEMP_PROT_LEV1 0x9D
#define BMS_DEF_REG_ADDR_DISCHARGE_LOW_TEMP_PROT_LEV2 0x9E
#define BMS_DEF_REG_ADDR_DIFF_VOLT_PROT_LEV1 0x9F
#define BMS_DEF_REG_ADDR_DIFF_VOLT_PROT_LEV2 0xA0
#define BMS_DEF_REG_ADDR_DIFF_TEMP_PROT_LEV1 0xA1
#define BMS_DEF_REG_ADDR_DIFF_TEMP_PROT_LEV2 0xA2
#define BMS_DEF_REG_ADDR_BALANCED_OPEN_START_VOLT 0xA3
#define BMS_DEF_REG_ADDR_BALANCED_OPEN_DIFF_VOLT 0xA4
#define BMS_DEF_REG_ADDR_CHARGE_SWITCH 0xA5
#define BMS_DEF_REG_ADDR_DISCHARGE_SWITCH 0xA6
#define BMS_DEF_REG_ADDR_STATE_OF_CHARGE 0xA7
#define BMS_DEF_REG_ADDR_MOS_TEMP_PROT 0xA8

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

#endif /* BMS_DEF_H */
