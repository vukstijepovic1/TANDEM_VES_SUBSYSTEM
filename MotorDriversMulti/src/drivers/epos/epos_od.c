/**
 * @file    epos_od.c
 * @brief   EPOS Object Dictionary.
 * @version	1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "epos_od.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/****************************************************************************
 *      Device Type - EPOS4 Firmware Specification page 66
 ***************************************************************************/
const canopen_od_entry_t ObjDeviceType = {
	.idx = 0x1000,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Error Register - EPOS4 Firmware Specification page 66
 ***************************************************************************/
const canopen_od_entry_t ObjErrorRegister = {
	.idx = 0x1001,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read Only
};

/****************************************************************************
 *      Error History - EPOS4 Firmware Specification page 67
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfErrors = {
	.idx = 0x1003,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjErrorHistory1 = {
	.idx = 0x1003,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjErrorHistory2 = {
	.idx = 0x1003,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjErrorHistory3 = {
	.idx = 0x1003,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjErrorHistory4 = {
	.idx = 0x1003,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjErrorHistory5 = {
	.idx = 0x1003,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      COB-ID SYNC - EPOS4 Firmware Specification page 68
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDSync = {
	.idx = 0x1005,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - CONST
};

/** @note: ManufacturerDeviceName isn't supported - Data Type is String */

/****************************************************************************
 *      Store Parameters - EPOS4 Firmware Specification page 69
 ***************************************************************************/
const canopen_od_entry_t ObjSaveAllParameters = {
	.idx = 0x1010,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Restore Default Parameters - EPOS4 Firmware Specification page 70
 ***************************************************************************/
const canopen_od_entry_t ObjRestoreAllDefaultParameters = {
	.idx = 0x1011,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      COB-ID EMCY - EPOS4 Firmware Specification page 71
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDEmcy = {
	.idx = 0x1014,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Consumer Heartbeat Time - EPOS4 Firmware Specification page 72
 ***************************************************************************/
const canopen_od_entry_t ObjConsumer1HeartbeatTime = {
	.idx = 0x1016,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjConsumer2HeartbeatTime = {
	.idx = 0x1016,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Producer Heartbeat Time - EPOS4 Firmware Specification page 73
 ***************************************************************************/
const canopen_od_entry_t ObjProducerHeartbeatTime = {
	.idx = 0x1017,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Identity Object - EPOS4 Firmware Specification page 73
 ***************************************************************************/
const canopen_od_entry_t ObjVendorID = {
	.idx = 0x1018,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjProductCode = {
	.idx = 0x1018,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjRevisionNumber = {
	.idx = 0x1018,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjSerialNumber = {
	.idx = 0x1018,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Error Behaviour
 ***************************************************************************/
const canopen_od_entry_t ObjCommunicationError = {
	.idx = 0x1029,
	.subidx = 0x01,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      SDO Server parameter - EPOS4 Firmware Specification page 78
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDSDOClientToServer = {
	.idx = 0x1200,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjCOBIDSDOServerToClient = {
	.idx = 0x1200,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Receive PDO1 Parameter - EPOS4 Firmware Specification page 79
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByRxPDO1 = {
	.idx = 0x1400,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeRxPDO1 = {
	.idx = 0x1400,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Receive PDO2 Paramter - EPOS4 Firmware Specification page 81
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByRxPDO2 = {
	.idx = 0x1401,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeRxPDO2 = {
	.idx = 0x1401,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Receive PDO3 Paramter - EPOS4 Firmware Specification page 82
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByRxPDO3 = {
	.idx = 0x1402,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeRxPDO3 = {
	.idx = 0x1402,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Receive PDO4 Parameter - EPOS4 Firmware Specification page 83
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByRxPDO4 = {
	.idx = 0x1403,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeRxPDO4 = {
	.idx = 0x1403,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Receive PDO1 Mapping - EPOS4 Firmware Specification page 84
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO1 = {
	.idx = 0x1600,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Receive PDO2 mapping - EPOS4 Firmware Specification page 86
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO2 = {
	.idx = 0x1601,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Receive PDO3 mapping - EPOS4 Firmware Specification page 88
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO3 = {
	.idx = 0x1602,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Receive PDO4 mapping - EPOS4 Firmware Specification page 90
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO4 = {
	.idx = 0x1603,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Transmit PDO1 Parameter - EPOS4 Firmware Specification page 92
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByTxPDO1 = {
	.idx = 0x1800,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeTxPDO1 = {
	.idx = 0x1800,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjInhibitTimeTxPDO1 = {
	.idx = 0x1800,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Transmit PDO2 Parameter - EPOS4 Firmware Specification page 94
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByTxPDO2 = {
	.idx = 0x1801,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeTxPDO2 = {
	.idx = 0x1801,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjInhibitTimeTxPDO2 = {
	.idx = 0x1801,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Transmit PDO3 Parameter - EPOS4 Firmware Specification page 96
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByTxPDO3 = {
	.idx = 0x1802,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeTxPDO3 = {
	.idx = 0x1802,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjInhibitTimeTxPDO3 = {
	.idx = 0x1802,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Transmit PDO4 Parameter - EPOS4 Firmware Specification page 98
 ***************************************************************************/
const canopen_od_entry_t ObjCOBIDUsedByTxPDO4 = {
	.idx = 0x1803,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTransmissionTypeTxPDO4 = {
	.idx = 0x1803,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjInhibitTimeTxPDO4 = {
	.idx = 0x1803,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Transmit PDO1 Mapping - EPOS4 Firmware Specification page 100
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO1 = {
	.idx = 0x1A00,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Transmit PDO2 mapping - EPOS4 Firmware Specification page 102
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO2 = {
	.idx = 0x1A01,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Transmit PDO3 mapping - EPOS4 Firmware Specification page 104
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO3 = {
	.idx = 0x1A02,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Transmit PDO4 mapping - EPOS4 Firmware Specification page 106
 ***************************************************************************/
const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjFirstMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSecondMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjThirdMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFourthMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjFifthMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSixthMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEightMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x08,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNinthMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTenthMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO4 = {
	.idx = 0x1A03,
	.subidx = 0x0C,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Program data - EPOS4 Firmware Specification page 118
 ***************************************************************************/
/** @note: ProgramNumber1 isn't implemented - Data Type = Domain */

/****************************************************************************
 *      Program control - EPOS4 Firmware Specification page 119
 ***************************************************************************/
const canopen_od_entry_t ObjProgramNumber1Control = {
	.idx = 0x1F51,
	.subidx = 0x01,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Program software identification
 *      - EPOS4 Firmware Specification page 120
 ***************************************************************************/
const canopen_od_entry_t ObjProgramNumber1Identification = {
	.idx = 0x1F56,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Flash status identification - EPOS4 Firmware Specification page 121
 ***************************************************************************/
const canopen_od_entry_t ObjProgramNumber1FlashStatusIdentification = {
	.idx = 0x1F57,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Node-ID - EPOS4 Firmware Specification page 123
 ***************************************************************************/
const canopen_od_entry_t ObjNodeID = {
	.idx = 0x2000,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      CAN bit rate - EPOS4 Firmware Specification page 124
 ***************************************************************************/
const canopen_od_entry_t ObjCANBitRate = {
	.idx = 0x2001,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      RS232 bit rate - EPOS4 Firmware Specification page 125
 ***************************************************************************/
const canopen_od_entry_t ObjRS232BitRate = {
	.idx = 0x2002,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      RS232 frame timeout - EPOS4 Firmware Specification page 125
 ***************************************************************************/
const canopen_od_entry_t ObjRS232FrameTimeout = {
	.idx = 0x2005,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      USB frame timeout - EPOS4 Firmware Specification page 126
 ***************************************************************************/
const canopen_od_entry_t ObjUSBFrameTimeout = {
	.idx = 0x2006,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      CAN bit rate display - EPOS4 Firmware Specification page 126
 ***************************************************************************/
const canopen_od_entry_t ObjCANBitRateDisplay = {
	.idx = 0x200A,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read Only
};

/****************************************************************************
 *      Active fieldbus - EPOS4 Firmware Specification page 127
 ***************************************************************************/
const canopen_od_entry_t ObjActiveFieldbus = {
	.idx = 0x2010,
	.subidx = 0x00,
	.data_size = 1 //  UNSIGNED8 - Read Only
};

/****************************************************************************
 *      Additional identity - EPOS4 Firmware Specification page 127
 ***************************************************************************/

/** @note: SerialNumberComplete isn't supported - Data Type = UNSIGNED64 */

/****************************************************************************
 *      Extension 1 identity - EPOS4 Firmware Specification page 128
 ***************************************************************************/
const canopen_od_entry_t ObjExtension1SoftwareVersion = {
	.idx = 0x2101,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

const canopen_od_entry_t ObjExtension1HardwareVersion = {
	.idx = 0x2101,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

const canopen_od_entry_t ObjExtension1ApplicationNumber = {
	.idx = 0x2101,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

const canopen_od_entry_t ObjExtension1ApplicationVersion = {
	.idx = 0x2101,
	.subidx = 0x04,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

/** @note: Extension1SerialNumber isn't supported - Data Type = UNSIGNED64 */

/** @note: Extension1Type isn't supported - Data Type = UNSIGNED64 */

/****************************************************************************
 *      Custom persistent memory - EPOS4 Firmware Specification page 130
 ***************************************************************************/
const canopen_od_entry_t ObjCustomPersistentMemory1 = {
	.idx = 0x210C,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjCustomPersistentMemory2 = {
	.idx = 0x210C,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjCustomPersistentMemory3 = {
	.idx = 0x210C,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjCustomPersistentMemory4 = {
	.idx = 0x210C,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Power supply - EPOS4 Firmware Specification page 131
 ***************************************************************************/
const canopen_od_entry_t ObjPowerSupplyVoltage = {
	.idx = 0x2200,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

/****************************************************************************
 *      Axis configuration - EPOS4 Firmware Specification page 132
 ***************************************************************************/
const canopen_od_entry_t ObjSensorsConfiguration = {
	.idx = 0x3000,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjControlStructure = {
	.idx = 0x3000,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjCommutationSensors = {
	.idx = 0x3000,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAxisConfigurationMiscellaneous = {
	.idx = 0x3000,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMainSensorResolution = {
	.idx = 0x3000,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjMaxSystemSpeed = {
	.idx = 0x3000,
	.subidx = 0x06,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Motor data - EPOS4 Firmware Specification page 145
 ***************************************************************************/
const canopen_od_entry_t ObjNominalCurrent = {
	.idx = 0x3001,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjOutputCurrentLimit = {
	.idx = 0x3001,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjNumberOfPolePairs = {
	.idx = 0x3001,
	.subidx = 0x03,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjThermalTimeConstantWinding = {
	.idx = 0x3001,
	.subidx = 0x04,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjTorqueConstant = {
	.idx = 0x3001,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Electrical system parameters - EPOS4 Firmware Specification page 148
 ***************************************************************************/
const canopen_od_entry_t ObjElectricalResistance = {
	.idx = 0x3002,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjElectricalInductance = {
	.idx = 0x3002,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Gear configuration - EPOS4 Firmware Specification page 149
 ***************************************************************************/
const canopen_od_entry_t ObjGearReductionNumber = {
	.idx = 0x3003,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjGearReductionDenominator = {
	.idx = 0x3003,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMaxGearInputSpeed = {
	.idx = 0x3003,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjGearMiscellaneousConfiguration = {
	.idx = 0x3003,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Digital incremental encoder 1
 *      - EPOS4 Firmware Specification page 149
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalIncrementalEncoder1NumberOfPulses = {
	.idx = 0x3010,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjDigitalIncrementalEncoder1Type = {
	.idx = 0x3010,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjDigitalIncrementalEncoder1IndexPosition = {
	.idx = 0x3010,
	.subidx = 0x04,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Analog incremental encoder - EPOS4 Firmware Specification page 153
 ***************************************************************************/
const canopen_od_entry_t ObjAnalogIncrementalEncoderType = {
	.idx = 0x3011,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjAnalogIncrementalEncoderResolution = {
	.idx = 0x3011,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAnalogIncrementalEncoderIndexPosition = {
	.idx = 0x3011,
	.subidx = 0x03,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      SSI absolute encoder - EPOS4 Firmware Specification page 155
 ***************************************************************************/
const canopen_od_entry_t ObjSSIDataRate = {
	.idx = 0x3012,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjSSINumberOfDataBits = {
	.idx = 0x3012,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSSIEncodingType = {
	.idx = 0x3012,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjSSITimeoutTime = {
	.idx = 0x3012,
	.subidx = 0x05,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjSSISpecialBitsData = {
	.idx = 0x3012,
	.subidx = 0x06,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

const canopen_od_entry_t ObjSSIRefreshFrequency = {
	.idx = 0x3012,
	.subidx = 0x07,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjSSIPowerUpTime = {
	.idx = 0x3012,
	.subidx = 0x08,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjSSIPositionRawValue = {
	.idx = 0x3012,
	.subidx = 0x09,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

const canopen_od_entry_t ObjSSICommutationOffsetValue = {
	.idx = 0x3012,
	.subidx = 0x0A,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSSIPositionBits = {
	.idx = 0x3012,
	.subidx = 0x0B,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSSISpecialBitsLeadingData = {
	.idx = 0x3012,
	.subidx = 0x0C,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

/** @note: PositionRawValueComplete isn't supported - Data Type is UNSIGNED64 */

const canopen_od_entry_t ObjSSIPositionRawValueComplete = {
	.idx = 0x3012, .subidx = 0x0D, .data_size = 8};

/****************************************************************************
 *      Digital Hall sensor - EPOS4 Firmware Specification page 161
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalHallSensorType = {
	.idx = 0x301A,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjDigitalHallSensorPattern = {
	.idx = 0x301A,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

/****************************************************************************
 *      Digital incremental encoder 2
 *      - EPOS4 Firmware Specification page 162
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalIncrementalEncoder2NumberOfPulses = {
	.idx = 0x3020,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjDigitalIncrementalEncoder2Type = {
	.idx = 0x3020,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjDigitalIncrementalEncoder2IndexPosition = {
	.idx = 0x3020,
	.subidx = 0x04,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Current control parameter set
 *      - EPOS4 Firmware Specification page 164
 ***************************************************************************/
const canopen_od_entry_t ObjCurrentControllerPGain = {
	.idx = 0x30A0,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjCurrentControllerIGain = {
	.idx = 0x30A0,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Position control parameter set
 *      - EPOS4 Firmware Specification page 165
 ***************************************************************************/
const canopen_od_entry_t ObjPositionControllerPGain = {
	.idx = 0x30A1,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjPositionControllerIGain = {
	.idx = 0x30A1,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjPositionControllerDGain = {
	.idx = 0x30A1,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjPositionControllerFFVelocityGain = {
	.idx = 0x30A1,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjPositionControllerFFAccelerationGain = {
	.idx = 0x30A1,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Velocity control parameter set
 *      - EPOS4 Firmware Specification page 167
 ***************************************************************************/
const canopen_od_entry_t ObjVelocityControllerPGain = {
	.idx = 0x30A2,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjVelocityControllerIGain = {
	.idx = 0x30A2,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjVelocityControllerFFVelocityGain = {
	.idx = 0x30A2,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjVelocityControllerFFAccelerationGain = {
	.idx = 0x30A2,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Velocity observer parameter set
 *      - EPOS4 Firmware Specification page 169
 ***************************************************************************/
const canopen_od_entry_t ObjVelocityObserverPositionCorrectionGain = {
	.idx = 0x30A3,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjVelocityObserverVelocityCorrectionGain = {
	.idx = 0x30A3,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjVelocityObserverLoadCorrectionGain = {
	.idx = 0x30A3,
	.subidx = 0x03,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjVelocityObserverFriction = {
	.idx = 0x30A3,
	.subidx = 0x04,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjVelocityObserverInertia = {
	.idx = 0x30A3,
	.subidx = 0x05,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Dual loop position controler parameter set
 *      - EPOS4 Firmware Specification page 171
 ***************************************************************************/
const canopen_od_entry_t ObjMainLoopPGainLowBandwidth = {
	.idx = 0x30AE,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMainLoopPGainHighBandwidth = {
	.idx = 0x30AE,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMainLoopGainSchedulingWeight = {
	.idx = 0x30AE,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjMainLoopFilterCoefficientA = {
	.idx = 0x30AE,
	.subidx = 0x10,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMainLoopFilterCoefficientB = {
	.idx = 0x30AE,
	.subidx = 0x11,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMainLoopFilterCoefficientC = {
	.idx = 0x30AE,
	.subidx = 0x12,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMainLoopFilterCoefficientD = {
	.idx = 0x30AE,
	.subidx = 0x13,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjMainLoopFilterCoefficientE = {
	.idx = 0x30AE,
	.subidx = 0x14,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopPGain = {
	.idx = 0x30AE,
	.subidx = 0x20,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopIGain = {
	.idx = 0x30AE,
	.subidx = 0x21,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopFFVelocityGain = {
	.idx = 0x30AE,
	.subidx = 0x22,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopFFAccelerationGain = {
	.idx = 0x30AE,
	.subidx = 0x23,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopObserverPoisitionCorrectionGain = {
	.idx = 0x30AE,
	.subidx = 0x30,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopObserverVelocityCorrectionGain = {
	.idx = 0x30AE,
	.subidx = 0x31,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopObserverLoadCorrectionGain = {
	.idx = 0x30AE,
	.subidx = 0x32,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopObserverFriction = {
	.idx = 0x30AE,
	.subidx = 0x33,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjAuxiliaryLoopObserverInertia = {
	.idx = 0x30AE,
	.subidx = 0x34,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjDualLoopConfigurationMiscellaneous = {
	.idx = 0x30AE,
	.subidx = 0x40,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Home position - EPOS4 Firmware Specification page 177
 ***************************************************************************/
const canopen_od_entry_t ObjHomePosition = {
	.idx = 0x30B0,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Home offset move distance - EPOS4 Firmware Specification page 178
 ***************************************************************************/
const canopen_od_entry_t ObjHomeOffsetMoveDistance = {
	.idx = 0x30B1,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Current threshold for homing mode
 *      - EPOS4 Firmware Specification page 178
 ***************************************************************************/
const canopen_od_entry_t ObjCurrentThresholdForHomingMode = {
	.idx = 0x30B2,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Current demand value - EPOS4 Firmware Specification page 178
 ***************************************************************************/
const canopen_od_entry_t ObjCurrentDemandValue = {
	.idx = 0x30D0,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Current actual values - EPOS4 Firmware Specification page 179
 ***************************************************************************/
const canopen_od_entry_t ObjCurrentActualValueAveraged = {
	.idx = 0x30D1,
	.subidx = 0x01,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjCurrentActualValue = {
	.idx = 0x30D1,
	.subidx = 0x02,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Torque actual values - EPOS4 Firmware Specification page 180
 ***************************************************************************/
const canopen_od_entry_t ObjTorqueActualValueAveraged = {
	.idx = 0x30D2,
	.subidx = 0x01,
	.data_size = 2 //  INTEGER16 - Read Only
};

/****************************************************************************
 *      Velocity actual values - EPOS4 Firmware Specification page 181
 ***************************************************************************/
const canopen_od_entry_t ObjVelocityActualValueAveraged = {
	.idx = 0x30D3,
	.subidx = 0x01,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Standstill window configuration
 *      - EPOS4 Firmware Specification page 182
 ***************************************************************************/
const canopen_od_entry_t ObjStandstillWindow = {
	.idx = 0x30E0,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjStandstillWindowTime = {
	.idx = 0x30E0,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjStandstillWindowTimeout = {
	.idx = 0x30E0,
	.subidx = 0x03,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Digital input properties - EPOS4 Firmware Specification page 184
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalInputLogicState = {
	.idx = 0x3141,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

const canopen_od_entry_t ObjDigitalInputPolarity = {
	.idx = 0x3141,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Configuration of Digital inputs
 *      - EPOS4 Firmware Specification page 185
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalInput1Configuration = {
	.idx = 0x3142,
	.subidx = 0x01,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjDigitalInput2Configuration = {
	.idx = 0x3142,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjDigitalInput3Configuration = {
	.idx = 0x3142,
	.subidx = 0x03,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjDigitalInput4Configuration = {
	.idx = 0x3142,
	.subidx = 0x04,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjHighSpeedDigitalInput1Configuration = {
	.idx = 0x3142,
	.subidx = 0x05,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjHighSpeedDigitalInput2Configuration = {
	.idx = 0x3142,
	.subidx = 0x06,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjHighSpeedDigitalInput3Configuration = {
	.idx = 0x3142,
	.subidx = 0x07,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjHighSpeedDigitalInput4Configuration = {
	.idx = 0x3142,
	.subidx = 0x08,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Digital output properties - EPOS4 Firmware Specification page 187
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalOutputsLogicState = {
	.idx = 0x3150,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

const canopen_od_entry_t ObjDigitalOutputsPolarity = {
	.idx = 0x3150,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Configuration of digital outputs
 *      - EPOS4 Firmware Specification page 188
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalOutput1Configuration = {
	.idx = 0x3151,
	.subidx = 0x01,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjDigitalOutput2Configuration = {
	.idx = 0x3151,
	.subidx = 0x02,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjHighSpeedDigitalOutput1Configuration = {
	.idx = 0x3151,
	.subidx = 0x03,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Holding brake parameters - EPOS4 Firmware Specification page 189
 ***************************************************************************/
const canopen_od_entry_t ObjHoldingBrakeRiseTime = {
	.idx = 0x3158,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjHoldingBrakeFallTime = {
	.idx = 0x3158,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Analog input properties - EPOS4 Firmware Specification page 191
 ***************************************************************************/
const canopen_od_entry_t ObjAnalogInput1Voltage = {
	.idx = 0x3160,
	.subidx = 0x01,
	.data_size = 2 // INTEGER16 - Read Only
};

const canopen_od_entry_t ObjAnalogInput2Voltage = {
	.idx = 0x3160,
	.subidx = 0x02,
	.data_size = 2 // INTEGER16 - Read Only
};

/****************************************************************************
 *      Analog input adjustment - EPOS4 Firmware Specification page 192
 ***************************************************************************/
const canopen_od_entry_t ObjAnalogInput1AdjustmentOffset = {
	.idx = 0x3163,
	.subidx = 0x01,
	.data_size = 2 // INTEGER16 - Read/Write
};

const canopen_od_entry_t ObjAnalogInput1AdjustmentGainFactor = {
	.idx = 0x3163,
	.subidx = 0x02,
	.data_size = 2 // UNSIGNED16 - Read/Write
};

const canopen_od_entry_t ObjAnalogInput2AdjustmentOffset = {
	.idx = 0x3163,
	.subidx = 0x03,
	.data_size = 2 // INTEGER16 - Read/Write
};

const canopen_od_entry_t ObjAnalogInput2AdjustmentGainFactor = {
	.idx = 0x3163,
	.subidx = 0x04,
	.data_size = 2 // UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Analog output properties - EPOS4 Firmware Specification page 193
 ***************************************************************************/
const canopen_od_entry_t ObjAnalogOutput1Voltage = {
	.idx = 0x3180,
	.subidx = 0x01,
	.data_size = 2 // INTEGER16 - Read Only
};

const canopen_od_entry_t ObjAnalogOutput2Voltage = {
	.idx = 0x3180,
	.subidx = 0x02,
	.data_size = 2 // INTEGER16 - Read Only
};

/****************************************************************************
 *      Configuration of analog outputs
 *      - EPOS4 Firmware Specification page 194
 ***************************************************************************/
const canopen_od_entry_t ObjAnalogOutput1Configuration = {
	.idx = 0x3181,
	.subidx = 0x01,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjAnalogOutput2Configuration = {
	.idx = 0x3181,
	.subidx = 0x01,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

/****************************************************************************
 *      Analog output general purpose
 *      - EPOS4 Firmware Specification page 195
 ***************************************************************************/
const canopen_od_entry_t ObjAnalogOutputGeneralPurposeA = {
	.idx = 0x3182,
	.subidx = 0x01,
	.data_size = 2 //  INTEGER16 - Read/Write
};

const canopen_od_entry_t ObjAnalogOutputGeneralPurposeB = {
	.idx = 0x3182,
	.subidx = 0x02,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Motor protection - EPOS4 Firmware Specification page 196
 ***************************************************************************/
const canopen_od_entry_t ObjI2tLevelMotor = {
	.idx = 0x3200,
	.subidx = 0x01,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

const canopen_od_entry_t ObjI2tLevelController = {
	.idx = 0x3200,
	.subidx = 0x02,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

/****************************************************************************
 *      Thermal controller protection
 *      - EPOS4 Firmware Specification page 197
 ***************************************************************************/
const canopen_od_entry_t ObjThermalPowerStage = {
	.idx = 0x3201,
	.subidx = 0x01,
	.data_size = 2 //  INTEGER16 - Read Only
};

/****************************************************************************
 *      Abort connection option code - EPOS4 Firmware Specification page 197
 ***************************************************************************/
const canopen_od_entry_t ObjAbortConnectionOptionCode = {
	.idx = 0x6007,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Error code - EPOS4 Firmware Specification page 198
 ***************************************************************************/
const canopen_od_entry_t ObjErrorCode = {
	.idx = 0x603F,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

/****************************************************************************
 *      Controlword - EPOS4 Firmware Specification page 199
 ***************************************************************************/
const canopen_od_entry_t ObjControlword = {
	.idx = 0x6040,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Statusword - EPOS4 Firmware Specification page 200
 ***************************************************************************/
const canopen_od_entry_t ObjStatusword = {
	.idx = 0x6041,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read Only
};

/****************************************************************************
 *      Quick stop option code - EPOS4 Firmware Specification page 201
 ***************************************************************************/
const canopen_od_entry_t ObjQuickStopOptioncode = {
	.idx = 0x605A,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Shutdown option code - EPOS4 Firmware Specification page 201
 ***************************************************************************/
const canopen_od_entry_t ObjShutdownOptionCode = {
	.idx = 0x605B,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Disable operation option code
 *      - EPOS4 Firmware Specification page 202
 ***************************************************************************/
const canopen_od_entry_t ObjDisableOperationOptionCode = {
	.idx = 0x605C,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Fault reaction option code - EPOS4 Firmware Specification page 202
 ***************************************************************************/
const canopen_od_entry_t ObjFaultReactionOptionCode = {
	.idx = 0x605E,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Modes of operation - EPOS4 Firmware Specification page 203
 ***************************************************************************/
const canopen_od_entry_t ObjModesOfOperation = {
	.idx = 0x6060,
	.subidx = 0x00,
	.data_size = 1 //  INTEGER8 - Read/Write
};

/****************************************************************************
 *      Modes of operation display - EPOS4 Firmware Specification page 203
 ***************************************************************************/
const canopen_od_entry_t ObjModesOfOperationDisplay = {
	.idx = 0x6061,
	.subidx = 0x00,
	.data_size = 1 //  INTEGER8 - Read Only
};

/****************************************************************************
 *      Position demand value - EPOS4 Firmware Specification page 204
 ***************************************************************************/
const canopen_od_entry_t ObjPositionDemandValue = {
	.idx = 0x6062,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Position actual value - EPOS4 Firmware Specification page 204
 ***************************************************************************/
const canopen_od_entry_t ObjPositionActualValue = {
	.idx = 0x6064,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Following error window - EPOS4 Firmware Specification page 204
 ***************************************************************************/
const canopen_od_entry_t ObjFollowingErrorWindow = {
	.idx = 0x6065,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Following error timeout - EPOS4 Firmware Specification page 205
 ***************************************************************************/
const canopen_od_entry_t ObjFollowingErrorTimeout = {
	.idx = 0x6066,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Velocity demand value - EPOS4 Firmware Specification page 205
 ***************************************************************************/
const canopen_od_entry_t ObjVelocityDemandValue = {
	.idx = 0x606B,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Velocity actual value - EPOS4 Firmware Specification page 205
 ***************************************************************************/
const canopen_od_entry_t ObjVelocityActualValue = {
	.idx = 0x606C,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Target torque - EPOS4 Firmware Specification page 206
 ***************************************************************************/
const canopen_od_entry_t ObjTargetTorque = {
	.idx = 0x6071,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Motor rated torque - EPOS4 Firmware Specification page 206
 ***************************************************************************/
const canopen_od_entry_t ObjMotorRatedTorque = {
	.idx = 0x6076,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Torque actual value - EPOS4 Firmware Specification page 206
 ***************************************************************************/
const canopen_od_entry_t ObjTorqueActualValue = {
	.idx = 0x6077,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Target position - EPOS4 Firmware Specification page 207
 ***************************************************************************/
const canopen_od_entry_t ObjTargetPosition = {
	.idx = 0x607A,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Position range limit - EPOS4 Firmware Specification page 208
 ***************************************************************************/
const canopen_od_entry_t ObjMinPositionRangeLimit = {
	.idx = 0x607B,
	.subidx = 0x01,
	.data_size = 4 //  INTEGER32 - Read/Write
};

const canopen_od_entry_t ObjMaxPositionRangeLimit = {
	.idx = 0x607B,
	.subidx = 0x02,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Software position limit - EPOS4 Firmware Specification page 209
 ***************************************************************************/
const canopen_od_entry_t ObjMinPositionLimit = {
	.idx = 0x607D,
	.subidx = 0x01,
	.data_size = 4 //  INTEGER32 - Read/Write
};

const canopen_od_entry_t ObjMaxPositionLimit = {
	.idx = 0x607D,
	.subidx = 0x02,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Max profile velocity - EPOS4 Firmware Specification page 210
 ***************************************************************************/
const canopen_od_entry_t ObjMaxProfileVelocity = {
	.idx = 0x607F,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Max motor speed - EPOS4 Firmware Specification page 210
 ***************************************************************************/
const canopen_od_entry_t ObjMaxMotorSpeed = {
	.idx = 0x6080,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Profile velocity - EPOS4 Firmware Specification page 211
 ***************************************************************************/
const canopen_od_entry_t ObjProfileVelocity = {
	.idx = 0x6081,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Profile acceleration - EPOS4 Firmware Specification page 211
 ***************************************************************************/
const canopen_od_entry_t ObjProfileAcceleration = {
	.idx = 0x6083,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Profile deceleration - EPOS4 Firmware Specification page 211
 ***************************************************************************/
const canopen_od_entry_t ObjProfileDeceleration = {
	.idx = 0x6084,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Quick stop deceleration - EPOS4 Firmware Specification page 212
 ***************************************************************************/
const canopen_od_entry_t ObjQuickStopDeceleration = {
	.idx = 0x6085,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Motion profile type - EPOS4 Firmware Specification page 212
 ***************************************************************************/
const canopen_od_entry_t ObjMotionProfileType = {
	.idx = 0x6086,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Homing method - EPOS4 Firmware Specification page 213
 ***************************************************************************/
const canopen_od_entry_t ObjHomingMethod = {
	.idx = 0x6098,
	.subidx = 0x00,
	.data_size = 1 //  INTEGER8 - Read/Write
};

/****************************************************************************
 *      Homing speeds - EPOS4 Firmware Specification page 214
 ***************************************************************************/
const canopen_od_entry_t ObjSpeedForSwitchSearch = {
	.idx = 0x6099,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

const canopen_od_entry_t ObjSpeedForZeroSearch = {
	.idx = 0x6099,
	.subidx = 0x02,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Homing acceleration - EPOS4 Firmware Specification page 215
 ***************************************************************************/
const canopen_od_entry_t ObjHomingAcceleration = {
	.idx = 0x609A,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      SI unit position - EPOS4 Firmware Specification page 215
 ***************************************************************************/
const canopen_od_entry_t ObjSIUnitPosition = {
	.idx = 0x60A8,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      SI unit velocity - EPOS4 Firmware Specification page 216
 ***************************************************************************/
const canopen_od_entry_t ObjSIUnitVelocity = {
	.idx = 0x60A9,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      SI unit acceleration - EPOS4 Firmware Specification page 217
 ***************************************************************************/
const canopen_od_entry_t ObjSIUnitAcceleration = {
	.idx = 0x60AA,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Position offset - EPOS4 Firmware Specification page 217
 ***************************************************************************/
const canopen_od_entry_t ObjPositionOffset = {
	.idx = 0x60B0,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Velocity offset - EPOS4 Firmware Specification page 218
 ***************************************************************************/
const canopen_od_entry_t ObjVelocityOffset = {
	.idx = 0x60B1,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Torque offset - EPOS4 Firmware Specification page 218
 ***************************************************************************/
const canopen_od_entry_t ObjTorqueOffset = {
	.idx = 0x60B2,
	.subidx = 0x00,
	.data_size = 2 //  INTEGER16 - Read/Write
};

/****************************************************************************
 *      Interpolation time period - EPOS4 Firmware Specification page 219
 ***************************************************************************/
const canopen_od_entry_t ObjInterpolationTimePeriodValue = {
	.idx = 0x60C2,
	.subidx = 0x01,
	.data_size = 1 //  UNSIGNED8 - Read/Write
};

const canopen_od_entry_t ObjInterpolationTimeIndex = {
	.idx = 0x60C2,
	.subidx = 0x02,
	.data_size = 1 //  INTEGER8 - Read/Write
};

/****************************************************************************
 *      Max acceleration - EPOS4 Firmware Specification page 220
 ***************************************************************************/
const canopen_od_entry_t ObjMaxAcceleration = {
	.idx = 0x60C5,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read/Write
};

/****************************************************************************
 *      Additional position actual values
 *      - EPOS4 Firmware Specification page 221
 ***************************************************************************/
const canopen_od_entry_t ObjPositionActualValueSensor1 = {
	.idx = 0x60E4,
	.subidx = 0x01,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjPositionActualValueSensor2 = {
	.idx = 0x60E4,
	.subidx = 0x02,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjPositionActualValueSensor3 = {
	.idx = 0x60E4,
	.subidx = 0x03,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Additional velocity actual values
 *      - EPOS4 Firmware Specification page 222
 ***************************************************************************/
const canopen_od_entry_t ObjVelocityActualValueSensor1 = {
	.idx = 0x60E5,
	.subidx = 0x01,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjVelocityActualValueSensor2 = {
	.idx = 0x60E5,
	.subidx = 0x02,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjVelocityActualValueSensor3 = {
	.idx = 0x60E5,
	.subidx = 0x03,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjVelocityActualValueAveragedSensor1 = {
	.idx = 0x60E5,
	.subidx = 0x09,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjVelocityActualValueAveragedSensor2 = {
	.idx = 0x60E5,
	.subidx = 0x0A,
	.data_size = 4 //  INTEGER32 - Read Only
};

const canopen_od_entry_t ObjVelocityActualValueAveragedSensor3 = {
	.idx = 0x60E5,
	.subidx = 0x0B,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Following error actual value
 *      - EPOS4 Firmware Specification page 224
 ***************************************************************************/
const canopen_od_entry_t ObjFollowingErrorActualValue = {
	.idx = 0x60F4,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read Only
};

/****************************************************************************
 *      Digital inputs - EPOS4 Firmware Specification page 225
 ***************************************************************************/
const canopen_od_entry_t ObjDigitalInputs = {
	.idx = 0x60FD,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Digital outputs - EPOS4 Firmware Specification page 226
 ***************************************************************************/
const canopen_od_entry_t ObjPhysicalOutputs = {
	.idx = 0x60FE,
	.subidx = 0x01,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/****************************************************************************
 *      Target velocity - EPOS4 Firmware Specification page 227
 ***************************************************************************/
const canopen_od_entry_t ObjTargetVelocity = {
	.idx = 0x60FF,
	.subidx = 0x00,
	.data_size = 4 //  INTEGER32 - Read/Write
};

/****************************************************************************
 *      Motor type - EPOS4 Firmware Specification page 227
 ***************************************************************************/
const canopen_od_entry_t ObjMotorType = {
	.idx = 0x6402,
	.subidx = 0x00,
	.data_size = 2 //  UNSIGNED16 - Read/Write
};

/****************************************************************************
 *      Supported drive modes - EPOS4 Firmware Specification page 228
 ***************************************************************************/
const canopen_od_entry_t ObjSupportedDriveModes = {
	.idx = 0x6502,
	.subidx = 0x00,
	.data_size = 4 //  UNSIGNED32 - Read Only
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/****************************** static functions ******************************/

/********************************* End Of File ********************************/