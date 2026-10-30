/**
 * @file    EPOS_od.h
 * @brief   EPOS Object Dictionary.
 * @version 1.0.0
 * @date    12.02.2025
 * @author  LisumLab
 */

#ifndef EPOS_OD_H
#define EPOS_OD_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

#include "canopen.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern const canopen_od_entry_t ObjDeviceType;
extern const canopen_od_entry_t ObjErrorRegister;
extern const canopen_od_entry_t ObjNumberOfErrors;
extern const canopen_od_entry_t ObjErrorHistory1;
extern const canopen_od_entry_t ObjErrorHistory2;
extern const canopen_od_entry_t ObjErrorHistory3;
extern const canopen_od_entry_t ObjErrorHistory4;
extern const canopen_od_entry_t ObjErrorHistory5;
extern const canopen_od_entry_t ObjCOBIDSync;
extern const canopen_od_entry_t ObjSaveAllParameters;
extern const canopen_od_entry_t ObjRestoreAllDefaultParameters;
extern const canopen_od_entry_t ObjCOBIDEmcy;
extern const canopen_od_entry_t ObjConsumer1HeartbeatTime;
extern const canopen_od_entry_t ObjConsumer2HeartbeatTime;
extern const canopen_od_entry_t ObjProducerHeartbeatTime;
extern const canopen_od_entry_t ObjVendorID;
extern const canopen_od_entry_t ObjProductCode;
extern const canopen_od_entry_t ObjRevisionNumber;
extern const canopen_od_entry_t ObjSerialNumber;
extern const canopen_od_entry_t ObjCommunicationError;
extern const canopen_od_entry_t ObjCOBIDSDOClientToServer;
extern const canopen_od_entry_t ObjCOBIDSDOServerToClient;
extern const canopen_od_entry_t ObjCOBIDUsedByRxPDO1;
extern const canopen_od_entry_t ObjTransmissionTypeRxPDO1;
extern const canopen_od_entry_t ObjCOBIDUsedByRxPDO2;
extern const canopen_od_entry_t ObjTransmissionTypeRxPDO2;
extern const canopen_od_entry_t ObjCOBIDUsedByRxPDO3;
extern const canopen_od_entry_t ObjTransmissionTypeRxPDO3;
extern const canopen_od_entry_t ObjCOBIDUsedByRxPDO4;
extern const canopen_od_entry_t ObjTransmissionTypeRxPDO4;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO1;
extern const canopen_od_entry_t ObjFirstMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjSecondMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjThirdMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjFourthMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjFifthMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjSixthMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjEightMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjNinthMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjTenthMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO1;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO2;
extern const canopen_od_entry_t ObjFirstMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjSecondMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjThirdMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjFourthMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjFifthMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjSixthMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjEightMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjNinthMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjTenthMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO2;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO3;
extern const canopen_od_entry_t ObjFirstMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjSecondMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjThirdMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjFourthMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjFifthMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjSixthMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjEightMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjNinthMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjTenthMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO3;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInRxPDO4;
extern const canopen_od_entry_t ObjFirstMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjSecondMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjThirdMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjFourthMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjFifthMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjSixthMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjSeventhMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjEightMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjNinthMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjTenthMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjEleventhMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInRxPDO4;
extern const canopen_od_entry_t ObjCOBIDUsedByTxPDO1;
extern const canopen_od_entry_t ObjTransmissionTypeTxPDO1;
extern const canopen_od_entry_t ObjInhibitTimeTxPDO1;
extern const canopen_od_entry_t ObjCOBIDUsedByTxPDO2;
extern const canopen_od_entry_t ObjTransmissionTypeTxPDO2;
extern const canopen_od_entry_t ObjInhibitTimeTxPDO2;
extern const canopen_od_entry_t ObjCOBIDUsedByTxPDO3;
extern const canopen_od_entry_t ObjTransmissionTypeTxPDO3;
extern const canopen_od_entry_t ObjInhibitTimeTxPDO3;
extern const canopen_od_entry_t ObjCOBIDUsedByTxPDO4;
extern const canopen_od_entry_t ObjTransmissionTypeTxPDO4;
extern const canopen_od_entry_t ObjInhibitTimeTxPDO4;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO1;
extern const canopen_od_entry_t ObjFirstMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjSecondMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjThirdMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjFourthMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjFifthMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjSixthMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjEightMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjNinthMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjTenthMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO1;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO2;
extern const canopen_od_entry_t ObjFirstMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjSecondMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjThirdMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjFourthMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjFifthMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjSixthMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjEightMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjNinthMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjTenthMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO2;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO3;
extern const canopen_od_entry_t ObjFirstMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjSecondMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjThirdMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjFourthMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjFifthMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjSixthMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjEightMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjNinthMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjTenthMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO3;
extern const canopen_od_entry_t ObjNumberOfMappedObjectsInTxPDO4;
extern const canopen_od_entry_t ObjFirstMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjSecondMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjThirdMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjFourthMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjFifthMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjSixthMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjSeventhMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjEightMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjNinthMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjTenthMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjEleventhMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjTwelfthMappedObjectInTxPDO4;
extern const canopen_od_entry_t ObjProgramNumber1Control;
extern const canopen_od_entry_t ObjProgramNumber1Identification;
extern const canopen_od_entry_t ObjProgramNumber1FlashStatusIdentification;
extern const canopen_od_entry_t ObjNodeID;
extern const canopen_od_entry_t ObjCANBitRate;
extern const canopen_od_entry_t ObjRS232BitRate;
extern const canopen_od_entry_t ObjRS232FrameTimeout;
extern const canopen_od_entry_t ObjUSBFrameTimeout;
extern const canopen_od_entry_t ObjCANBitRateDisplay;
extern const canopen_od_entry_t ObjActiveFieldbus;
extern const canopen_od_entry_t ObjExtension1SoftwareVersion;
extern const canopen_od_entry_t ObjExtension1HardwareVersion;
extern const canopen_od_entry_t ObjExtension1ApplicationNumber;
extern const canopen_od_entry_t ObjExtension1ApplicationVersion;
extern const canopen_od_entry_t ObjCustomPersistentMemory1;
extern const canopen_od_entry_t ObjCustomPersistentMemory2;
extern const canopen_od_entry_t ObjCustomPersistentMemory3;
extern const canopen_od_entry_t ObjCustomPersistentMemory4;
extern const canopen_od_entry_t ObjPowerSupplyVoltage;
extern const canopen_od_entry_t ObjSensorsConfiguration;
extern const canopen_od_entry_t ObjControlStructure;
extern const canopen_od_entry_t ObjCommutationSensors;
extern const canopen_od_entry_t ObjAxisConfigurationMiscellaneous;
extern const canopen_od_entry_t ObjMainSensorResolution;
extern const canopen_od_entry_t ObjMaxSystemSpeed;
extern const canopen_od_entry_t ObjNominalCurrent;
extern const canopen_od_entry_t ObjOutputCurrentLimit;
extern const canopen_od_entry_t ObjNumberOfPolePairs;
extern const canopen_od_entry_t ObjThermalTimeConstantWinding;
extern const canopen_od_entry_t ObjTorqueConstant;
extern const canopen_od_entry_t ObjElectricalResistance;
extern const canopen_od_entry_t ObjElectricalInductance;
extern const canopen_od_entry_t ObjGearReductionNumber;
extern const canopen_od_entry_t ObjGearReductionDenominator;
extern const canopen_od_entry_t ObjMaxGearInputSpeed;
extern const canopen_od_entry_t ObjGearMiscellaneousConfiguration;
extern const canopen_od_entry_t ObjDigitalIncrementalEncoder1NumberOfPulses;
extern const canopen_od_entry_t ObjDigitalIncrementalEncoder1Type;
extern const canopen_od_entry_t ObjDigitalIncrementalEncoder1IndexPosition;
extern const canopen_od_entry_t ObjAnalogIncrementalEncoderType;
extern const canopen_od_entry_t ObjAnalogIncrementalEncoderResolution;
extern const canopen_od_entry_t ObjAnalogIncrementalEncoderIndexPosition;
extern const canopen_od_entry_t ObjSSIDataRate;
extern const canopen_od_entry_t ObjSSINumberOfDataBits;
extern const canopen_od_entry_t ObjSSIEncodingType;
extern const canopen_od_entry_t ObjSSITimeoutTime;
extern const canopen_od_entry_t ObjSSISpecialBitsData;
extern const canopen_od_entry_t ObjSSIRefreshFrequency;
extern const canopen_od_entry_t ObjSSIPowerUpTime;
extern const canopen_od_entry_t ObjSSIPositionRawValue;
extern const canopen_od_entry_t ObjSSICommutationOffsetValue;
extern const canopen_od_entry_t ObjSSIPositionBits;
extern const canopen_od_entry_t ObjSSISpecialBitsLeadingData;
extern const canopen_od_entry_t ObjSSIPositionRawValueComplete;
extern const canopen_od_entry_t ObjDigitalHallSensorType;
extern const canopen_od_entry_t ObjDigitalHallSensorPattern;
extern const canopen_od_entry_t ObjDigitalIncrementalEncoder2NumberOfPulses;
extern const canopen_od_entry_t ObjDigitalIncrementalEncoder2Type;
extern const canopen_od_entry_t ObjDigitalIncrementalEncoder2IndexPosition;
extern const canopen_od_entry_t ObjCurrentControllerPGain;
extern const canopen_od_entry_t ObjCurrentControllerIGain;
extern const canopen_od_entry_t ObjPositionControllerPGain;
extern const canopen_od_entry_t ObjPositionControllerIGain;
extern const canopen_od_entry_t ObjPositionControllerDGain;
extern const canopen_od_entry_t ObjPositionControllerFFVelocityGain;
extern const canopen_od_entry_t ObjPositionControllerFFAccelerationGain;
extern const canopen_od_entry_t ObjVelocityControllerPGain;
extern const canopen_od_entry_t ObjVelocityControllerIGain;
extern const canopen_od_entry_t ObjVelocityControllerFFVelocityGain;
extern const canopen_od_entry_t ObjVelocityControllerFFAccelerationGain;
extern const canopen_od_entry_t ObjVelocityObserverPositionCorrectionGain;
extern const canopen_od_entry_t ObjVelocityObserverVelocityCorrectionGain;
extern const canopen_od_entry_t ObjVelocityObserverLoadCorrectionGain;
extern const canopen_od_entry_t ObjVelocityObserverFriction;
extern const canopen_od_entry_t ObjVelocityObserverInertia;
extern const canopen_od_entry_t ObjMainLoopPGainLowBandwidth;
extern const canopen_od_entry_t ObjMainLoopPGainHighBandwidth;
extern const canopen_od_entry_t ObjMainLoopGainSchedulingWeight;
extern const canopen_od_entry_t ObjMainLoopFilterCoefficientA;
extern const canopen_od_entry_t ObjMainLoopFilterCoefficientB;
extern const canopen_od_entry_t ObjMainLoopFilterCoefficientC;
extern const canopen_od_entry_t ObjMainLoopFilterCoefficientD;
extern const canopen_od_entry_t ObjMainLoopFilterCoefficientE;
extern const canopen_od_entry_t ObjAuxiliaryLoopPGain;
extern const canopen_od_entry_t ObjAuxiliaryLoopIGain;
extern const canopen_od_entry_t ObjAuxiliaryLoopFFVelocityGain;
extern const canopen_od_entry_t ObjAuxiliaryLoopFFAccelerationGain;
extern const canopen_od_entry_t ObjAuxiliaryLoopObserverPoisitionCorrectionGain;
extern const canopen_od_entry_t ObjAuxiliaryLoopObserverVelocityCorrectionGain;
extern const canopen_od_entry_t ObjAuxiliaryLoopObserverLoadCorrectionGain;
extern const canopen_od_entry_t ObjAuxiliaryLoopObserverFriction;
extern const canopen_od_entry_t ObjAuxiliaryLoopObserverInertia;
extern const canopen_od_entry_t ObjDualLoopConfigurationMiscellaneous;
extern const canopen_od_entry_t ObjHomePosition;
extern const canopen_od_entry_t ObjHomeOffsetMoveDistance;
extern const canopen_od_entry_t ObjCurrentThresholdForHomingMode;
extern const canopen_od_entry_t ObjCurrentDemandValue;
extern const canopen_od_entry_t ObjCurrentActualValueAveraged;
extern const canopen_od_entry_t ObjCurrentActualValue;
extern const canopen_od_entry_t ObjTorqueActualValueAveraged;
extern const canopen_od_entry_t ObjVelocityActualValueAveraged;
extern const canopen_od_entry_t ObjStandstillWindow;
extern const canopen_od_entry_t ObjStandstillWindowTime;
extern const canopen_od_entry_t ObjStandstillWindowTimeout;
extern const canopen_od_entry_t ObjDigitalInputLogicState;
extern const canopen_od_entry_t ObjDigitalInputPolarity;
extern const canopen_od_entry_t ObjDigitalInput1Configuration;
extern const canopen_od_entry_t ObjDigitalInput2Configuration;
extern const canopen_od_entry_t ObjDigitalInput3Configuration;
extern const canopen_od_entry_t ObjDigitalInput4Configuration;
extern const canopen_od_entry_t ObjHighSpeedDigitalInput1Configuration;
extern const canopen_od_entry_t ObjHighSpeedDigitalInput2Configuration;
extern const canopen_od_entry_t ObjHighSpeedDigitalInput3Configuration;
extern const canopen_od_entry_t ObjHighSpeedDigitalInput4Configuration;
extern const canopen_od_entry_t ObjDigitalOutputsLogicState;
extern const canopen_od_entry_t ObjDigitalOutputsPolarity;
extern const canopen_od_entry_t ObjDigitalOutput1Configuration;
extern const canopen_od_entry_t ObjDigitalOutput2Configuration;
extern const canopen_od_entry_t ObjHighSpeedDigitalOutput1Configuration;
extern const canopen_od_entry_t ObjHoldingBrakeRiseTime;
extern const canopen_od_entry_t ObjHoldingBrakeFallTime;
extern const canopen_od_entry_t ObjAnalogInput1Voltage;
extern const canopen_od_entry_t ObjAnalogInput2Voltage;
extern const canopen_od_entry_t ObjAnalogInput1AdjustmentOffset;
extern const canopen_od_entry_t ObjAnalogInput1AdjustmentGainFactor;
extern const canopen_od_entry_t ObjAnalogInput2AdjustmentOffset;
extern const canopen_od_entry_t ObjAnalogInput2AdjustmentGainFactor;
extern const canopen_od_entry_t ObjAnalogOutput1Voltage;
extern const canopen_od_entry_t ObjAnalogOutput2Voltage;
extern const canopen_od_entry_t ObjAnalogOutput1Configuration;
extern const canopen_od_entry_t ObjAnalogOutput2Configuration;
extern const canopen_od_entry_t ObjAnalogOutputGeneralPurposeA;
extern const canopen_od_entry_t ObjAnalogOutputGeneralPurposeB;
extern const canopen_od_entry_t ObjI2tLevelMotor;
extern const canopen_od_entry_t ObjI2tLevelController;
extern const canopen_od_entry_t ObjThermalPowerStage;
extern const canopen_od_entry_t ObjAbortConnectionOptionCode;
extern const canopen_od_entry_t ObjErrorCode;
extern const canopen_od_entry_t ObjControlword;
extern const canopen_od_entry_t ObjStatusword;
extern const canopen_od_entry_t ObjQuickStopOptioncode;
extern const canopen_od_entry_t ObjShutdownOptionCode;
extern const canopen_od_entry_t ObjDisableOperationOptionCode;
extern const canopen_od_entry_t ObjFaultReactionOptionCode;
extern const canopen_od_entry_t ObjModesOfOperation;
extern const canopen_od_entry_t ObjModesOfOperationDisplay;
extern const canopen_od_entry_t ObjPositionDemandValue;
extern const canopen_od_entry_t ObjPositionActualValue;
extern const canopen_od_entry_t ObjFollowingErrorWindow;
extern const canopen_od_entry_t ObjFollowingErrorTimeout;
extern const canopen_od_entry_t ObjVelocityDemandValue;
extern const canopen_od_entry_t ObjVelocityActualValue;
extern const canopen_od_entry_t ObjTargetTorque;
extern const canopen_od_entry_t ObjMotorRatedTorque;
extern const canopen_od_entry_t ObjTorqueActualValue;
extern const canopen_od_entry_t ObjTargetPosition;
extern const canopen_od_entry_t ObjMinPositionRangeLimit;
extern const canopen_od_entry_t ObjMaxPositionRangeLimit;
extern const canopen_od_entry_t ObjMinPositionLimit;
extern const canopen_od_entry_t ObjMaxPositionLimit;
extern const canopen_od_entry_t ObjMaxProfileVelocity;
extern const canopen_od_entry_t ObjMaxMotorSpeed;
extern const canopen_od_entry_t ObjProfileVelocity;
extern const canopen_od_entry_t ObjProfileAcceleration;
extern const canopen_od_entry_t ObjProfileDeceleration;
extern const canopen_od_entry_t ObjQuickStopDeceleration;
extern const canopen_od_entry_t ObjMotionProfileType;
extern const canopen_od_entry_t ObjHomingMethod;
extern const canopen_od_entry_t ObjSpeedForSwitchSearch;
extern const canopen_od_entry_t ObjSpeedForZeroSearch;
extern const canopen_od_entry_t ObjHomingAcceleration;
extern const canopen_od_entry_t ObjSIUnitPosition;
extern const canopen_od_entry_t ObjSIUnitVelocity;
extern const canopen_od_entry_t ObjSIUnitAcceleration;
extern const canopen_od_entry_t ObjPositionOffset;
extern const canopen_od_entry_t ObjVelocityOffset;
extern const canopen_od_entry_t ObjTorqueOffset;
extern const canopen_od_entry_t ObjInterpolationTimePeriodValue;
extern const canopen_od_entry_t ObjInterpolationTimeIndex;
extern const canopen_od_entry_t ObjMaxAcceleration;
extern const canopen_od_entry_t ObjPositionActualValueSensor1;
extern const canopen_od_entry_t ObjPositionActualValueSensor2;
extern const canopen_od_entry_t ObjPositionActualValueSensor3;
extern const canopen_od_entry_t ObjVelocityActualValueSensor1;
extern const canopen_od_entry_t ObjVelocityActualValueSensor2;
extern const canopen_od_entry_t ObjVelocityActualValueSensor3;
extern const canopen_od_entry_t ObjVelocityActualValueAveragedSensor1;
extern const canopen_od_entry_t ObjVelocityActualValueAveragedSensor2;
extern const canopen_od_entry_t ObjVelocityActualValueAveragedSensor3;
extern const canopen_od_entry_t ObjFollowingErrorActualValue;
extern const canopen_od_entry_t ObjDigitalInputs;
extern const canopen_od_entry_t ObjPhysicalOutputs;
extern const canopen_od_entry_t ObjTargetVelocity;
extern const canopen_od_entry_t ObjMotorType;
extern const canopen_od_entry_t ObjSupportedDriveModes;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* EPOS_OD_H */