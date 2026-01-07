#pragma once
#include <stdint.h>

#include "atl/SpinWait.h"
#include "twi.h"
#include "Debug.h"

/*
 *   This code is based on vl53l0x-arduino: Copyright (c) 2017-2022 Pololu Corporation.
 *   https://github.com/pololu/vl53l0x-arduino/tree/master
 *
 *   Which in turn is based on the official API (SDK) source code:
 *   Copyright © 2016, STMicroelectronics International N.V. All rights reserved.
 */

// ShutdownPinT (XSHUT) used for initialization of multiple VL53L0X instances (active low).
// If only using 1 VL53L0X instance, you can pass in a DummyPin class and not connect XSHUT.
template <class I2cT, class ShutdownPinT, const uint8_t Address>
class VL53L0X
{
    static_assert((Address & 0x80) == 0, "VL53L0X Address highest bit (7) must be cleared. It is not used in I2C.");

    enum class Registers
    {
        SysRangeStart = 0x00,

        SystemThreshHigh = 0x0C,
        SystemThreshLow = 0x0E,

        SystemSequenceConfig = 0x01,
        SystemRangeConfig = 0x09,
        SystemInterMeasurementPeriod = 0x04,

        SystemInterruptConfigGpio = 0x0A,

        GpioHvMuxActiveHigh = 0x84,

        SystemInterruptClear = 0x0B,

        ResultInterruptStatus = 0x13,
        ResultRangeStatus = 0x14,
        ResultRangeStatusPlus10 = 0x1E,

        ResultCoreAmbientWindowEventsRtn = 0xBC,
        ResultCoreRangingTotalEventsRtn = 0xC0,
        ResultCoreAmbientWindowEventsRef = 0xD0,
        ResultCoreRangingTotalEventsRef = 0xD4,
        ResultPeakSignalRateRef = 0xB6,

        AlgoPartToPartRangeOffsetMm = 0x28,

        I2cSlaveDeviceAddress = 0x8A,

        MsrcConfigControl = 0x60,

        PreRangeConfigMinSnr = 0x27,
        PreRangeConfigValidPhaseLow = 0x56,
        PreRangeConfigValidPhaseHigh = 0x57,
        PreRangeMinCountRateRtnLimit = 0x64,

        FinalRangeConfigMinSnr = 0x67,
        FinalRangeConfigValidPhaseLow = 0x47,
        FinalRangeConfigValidPhaseHigh = 0x48,
        FinalRangeConfigMinCountRateRtnLimit = 0x44,

        PreRangeConfigSigmaThreshHi = 0x61,
        PreRangeConfigSigmaThreshLo = 0x62,

        PreRangeConfigVcselPeriod = 0x50,
        PreRangeConfigTimeoutMacropHi = 0x51,
        PreRangeConfigTimeoutMacropLo = 0x52,

        SystemHistogramBin = 0x81,
        HistogramConfigInitialPhaseSelect = 0x33,
        HistogramConfigReadoutCtrl = 0x55,

        FinalRangeConfigVcselPeriod = 0x70,
        FinalRangeConfigTimeoutMacropHi = 0x71,
        FinalRangeConfigTimeoutMacropLo = 0x72,
        CrosstalkCompensationPeakRateMcps = 0x20,

        MsrcConfigTimeoutMacrop = 0x46,

        SoftResetGo2SoftResetN = 0xBF,
        IdentificationModelId = 0xC0,
        IdentificationRevisionId = 0xC2,

        OscCalibrateVal = 0xF8,

        GlobalConfigVcselWidth = 0x32,
        GlobalConfigSpadEnablesRef0 = 0xB0,
        GlobalConfigSpadEnablesRef1 = 0xB1,
        GlobalConfigSpadEnablesRef2 = 0xB2,
        GlobalConfigSpadEnablesRef3 = 0xB3,
        GlobalConfigSpadEnablesRef4 = 0xB4,
        GlobalConfigSpadEnablesRef5 = 0xB5,

        GlobalConfigRefEnStartSelect = 0xB6,
        DynamicSpadNumRequestedRefSpad = 0x4E,
        DynamicSpadRefEnStartOffset = 0x4F,
        PowerManagementGo1PowerForce = 0x80,

        VhvConfigPadSclSdaExtsupHv = 0x89,

        AlgoPhasecalLim = 0x30,
        AlgoPhasecalConfigTimeout = 0x30,
    };

    enum class VcselPeriodType
    {
        VcselPeriodPreRange,
        VcselPeriodFinalRange
    };

    uint16_t const Budget_StartOverhead = 1910;
    uint16_t const Budget_EndOverhead = 960;
    uint16_t const Budget_MsrcOverhead = 660;
    uint16_t const Budget_TccOverhead = 590;
    uint16_t const Budget_DssOverhead = 690;
    uint16_t const Budget_PreRangeOverhead = 660;
    uint16_t const Budget_FinalRangeOverhead = 550;

    struct SequenceStepEnables
    {
        bool tcc;
        bool msrc;
        bool dss;
        bool preRange;
        bool finalRange;
    };

    struct SequenceStepTimeouts
    {
        uint16_t preRangeVcselPeriod_pclks;
        uint16_t finalRangeVcselPeriod_pclks;

        uint16_t msrcDssTcc_mclks;
        uint16_t preRange_mclks;
        uint16_t finalRange_mclks;

        uint32_t msrcDssTcc_us;
        uint32_t preRange_us;
        uint32_t finalRange_us;
    };

public:
    static const uint8_t DefaultAddress = 0x29;
    static const uint8_t DebugComponentId = 53;

    VL53L0X()
        : _stopVariable(0), _measurementTimingBudget_us(0), _shutdownPin(false), _address(DefaultAddress)
    {
    }

    bool Open()
    {
        // shutdown is active low
        _shutdownPin.Write(true);
        SpinWait(100);

        uint8_t data = 0;
        if (!TryReadRegister8(Registers::IdentificationModelId, &data))
        {
            // retry with non-default address.
            _address = Address;
            if (!TryReadRegister8(Registers::IdentificationModelId, &data))
                return false;
        }
        if (data != 0xEE)
        {
            Error("Invalid Model Id");
            return false;
        }
        Trace("Open-DataInit");
        // VL53L0X_DataInit() begin

        // sensor uses 1V8 mode for I/O by default; switch to 2V8 mode
        if (!TryReadRegister8(Registers::VhvConfigPadSclSdaExtsupHv, &data))
            return false;
        // set bit 0
        if (!TryWriteRegister8(Registers::VhvConfigPadSclSdaExtsupHv, data | 0x01))
            return false;

        // "Set I2C standard mode"
        if (!TryWriteRegister8(0x88, 0x00))
            return false;
        if (!TryWriteRegister8(0x80, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x00, 0x00))
            return false;
        if (!TryReadRegister8(0x91, &_stopVariable))
            return false;
        if (!TryWriteRegister8(0x00, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x80, 0x00))
            return false;

        // disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
        if (!TryReadRegister8(Registers::MsrcConfigControl, &data))
            return false;
        if (!TryWriteRegister8(Registers::MsrcConfigControl, data | 0x12))
            return false;

        // set final range signal rate limit to 0.25 MCPS (million counts per second)
        if (!TryWriteSignalRateLimit(0.25))
            return false;

        if (!TryWriteRegister8(Registers::SystemSequenceConfig, 0xFF))
            return false;
        // VL53L0X_DataInit() end
        // VL53L0X_StaticInit() begin
        Trace("Open-StaticInit");

        uint8_t spad_count = 0;
        bool spad_type_is_aperture = false;
        if (!TryReadSpadInfo(&spad_count, &spad_type_is_aperture))
            return false;

        // The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
        // the API, but the same data seems to be more easily readable from
        // GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
        uint8_t ref_spad_map[6];
        if (!TryReadRegisterMulti(Registers::GlobalConfigSpadEnablesRef0, ref_spad_map, 6))
            return false;

        // -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)
        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(Registers::DynamicSpadRefEnStartOffset, 0x00))
            return false;
        if (!TryWriteRegister8(Registers::DynamicSpadNumRequestedRefSpad, 0x2C))
            return false;
        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(Registers::GlobalConfigRefEnStartSelect, 0xB4))
            return false;

        // 12 is the first aperture spad
        uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0;
        uint8_t spads_enabled = 0;

        for (uint8_t i = 0; i < 48; i++)
        {
            if (i < first_spad_to_enable || spads_enabled == spad_count)
            {
                // This bit is lower than the first one that should be enabled, or
                // (reference_spad_count) bits have already been enabled, so zero this bit
                ref_spad_map[i / 8] &= ~(1 << (i % 8));
            }
            else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
            {
                spads_enabled++;
            }
        }

        if (!TryWriteRegisterMulti(Registers::GlobalConfigSpadEnablesRef0, ref_spad_map, 6))
            return false;
        // -- VL53L0X_set_reference_spads() end

        // -- VL53L0X_load_tuning_settings() begin
        // DefaultTuningSettings from vl53l0x_tuning.h

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x00, 0x00))
            return false;

        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x09, 0x00))
            return false;
        if (!TryWriteRegister8(0x10, 0x00))
            return false;
        if (!TryWriteRegister8(0x11, 0x00))
            return false;

        if (!TryWriteRegister8(0x24, 0x01))
            return false;
        if (!TryWriteRegister8(0x25, 0xFF))
            return false;
        if (!TryWriteRegister8(0x75, 0x00))
            return false;

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x4E, 0x2C))
            return false;
        if (!TryWriteRegister8(0x48, 0x00))
            return false;
        if (!TryWriteRegister8(0x30, 0x20))
            return false;

        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x30, 0x09))
            return false;
        if (!TryWriteRegister8(0x54, 0x00))
            return false;
        if (!TryWriteRegister8(0x31, 0x04))
            return false;
        if (!TryWriteRegister8(0x32, 0x03))
            return false;
        if (!TryWriteRegister8(0x40, 0x83))
            return false;
        if (!TryWriteRegister8(0x46, 0x25))
            return false;
        if (!TryWriteRegister8(0x60, 0x00))
            return false;
        if (!TryWriteRegister8(0x27, 0x00))
            return false;
        if (!TryWriteRegister8(0x50, 0x06))
            return false;
        if (!TryWriteRegister8(0x51, 0x00))
            return false;
        if (!TryWriteRegister8(0x52, 0x96))
            return false;
        if (!TryWriteRegister8(0x56, 0x08))
            return false;
        if (!TryWriteRegister8(0x57, 0x30))
            return false;
        if (!TryWriteRegister8(0x61, 0x00))
            return false;
        if (!TryWriteRegister8(0x62, 0x00))
            return false;
        if (!TryWriteRegister8(0x64, 0x00))
            return false;
        if (!TryWriteRegister8(0x65, 0x00))
            return false;
        if (!TryWriteRegister8(0x66, 0xA0))
            return false;

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x22, 0x32))
            return false;
        if (!TryWriteRegister8(0x47, 0x14))
            return false;
        if (!TryWriteRegister8(0x49, 0xFF))
            return false;
        if (!TryWriteRegister8(0x4A, 0x00))
            return false;

        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x7A, 0x0A))
            return false;
        if (!TryWriteRegister8(0x7B, 0x00))
            return false;
        if (!TryWriteRegister8(0x78, 0x21))
            return false;

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x23, 0x34))
            return false;
        if (!TryWriteRegister8(0x42, 0x00))
            return false;
        if (!TryWriteRegister8(0x44, 0xFF))
            return false;
        if (!TryWriteRegister8(0x45, 0x26))
            return false;
        if (!TryWriteRegister8(0x46, 0x05))
            return false;
        if (!TryWriteRegister8(0x40, 0x40))
            return false;
        if (!TryWriteRegister8(0x0E, 0x06))
            return false;
        if (!TryWriteRegister8(0x20, 0x1A))
            return false;
        if (!TryWriteRegister8(0x43, 0x40))
            return false;

        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x34, 0x03))
            return false;
        if (!TryWriteRegister8(0x35, 0x44))
            return false;

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x31, 0x04))
            return false;
        if (!TryWriteRegister8(0x4B, 0x09))
            return false;
        if (!TryWriteRegister8(0x4C, 0x05))
            return false;
        if (!TryWriteRegister8(0x4D, 0x04))
            return false;

        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x44, 0x00))
            return false;
        if (!TryWriteRegister8(0x45, 0x20))
            return false;
        if (!TryWriteRegister8(0x47, 0x08))
            return false;
        if (!TryWriteRegister8(0x48, 0x28))
            return false;
        if (!TryWriteRegister8(0x67, 0x00))
            return false;
        if (!TryWriteRegister8(0x70, 0x04))
            return false;
        if (!TryWriteRegister8(0x71, 0x01))
            return false;
        if (!TryWriteRegister8(0x72, 0xFE))
            return false;
        if (!TryWriteRegister8(0x76, 0x00))
            return false;
        if (!TryWriteRegister8(0x77, 0x00))
            return false;

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x0D, 0x01))
            return false;

        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x80, 0x01))
            return false;
        if (!TryWriteRegister8(0x01, 0xF8))
            return false;

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x8E, 0x01))
            return false;
        if (!TryWriteRegister8(0x00, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x80, 0x00))
            return false;

        // -- VL53L0X_load_tuning_settings() end

        Trace("Open-SetGpioConfig");
        // "Set interrupt config to new sample ready"
        // -- VL53L0X_SetGpioConfig() begin

        if (!TryWriteRegister8(Registers::SystemInterruptConfigGpio, 0x04))
            return false;
        if (!TryReadRegister8(Registers::GpioHvMuxActiveHigh, &data))
            return false;
        if (!TryWriteRegister8(Registers::GpioHvMuxActiveHigh, data & ~0x10)) // active lo
            return false;
        if (!TryWriteRegister8(Registers::SystemInterruptClear, 0x01))
            return false;

        // -- VL53L0X_SetGpioConfig() end

        // store for internal reuse
        if (!TryReadMeasurementTimingBudget(&_measurementTimingBudget_us))
            return false;

        // "Disable MSRC and TCC by default"
        // MSRC = Minimum Signal Rate Check
        // TCC = Target CentreCheck
        // -- VL53L0X_SetSequenceStepEnable() begin

        if (!TryWriteRegister8(Registers::SystemSequenceConfig, 0xE8))
            return false;

        // -- VL53L0X_SetSequenceStepEnable() end

        // "Recalculate timing budget"
        if (!TryWriteMeasurementTimingBudget(_measurementTimingBudget_us))
            return false;

        // VL53L0X_StaticInit() end

        Trace("Open-Calibration");

        // VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())
        // -- VL53L0X_perform_vhv_calibration() begin

        if (!TryWriteRegister8(Registers::SystemSequenceConfig, 0x01))
            return false;
        if (!PerformSingleRefCalibration(0x40))
            return false;

        // -- VL53L0X_perform_vhv_calibration() end
        // -- VL53L0X_perform_phase_calibration() begin

        if (!TryWriteRegister8(Registers::SystemSequenceConfig, 0x02))
            return false;
        if (!PerformSingleRefCalibration(0x00))
            return false;

        // -- VL53L0X_perform_phase_calibration() end

        // "restore the previous Sequence Config"
        if (!TryWriteRegister8(Registers::SystemSequenceConfig, 0xE8))
            return false;

        // VL53L0X_PerformRefCalibration() end

        Trace("Open-Set Address");
        // set I2c address
        if (!TryWriteRegister8(Registers::I2cSlaveDeviceAddress, Address))
            return false;
        _address = Address;

        return true;
    }

    void Enable(bool enable = true)
    {
        _shutdownPin.Write(enable);
    }

    // Start continuous ranging measurements. If period_ms (optional) is 0 or not
    // given, continuous back-to-back mode is used (the sensor takes measurements as
    // often as possible); otherwise, continuous timed mode is used, with the given
    // inter-measurement period in milliseconds determining how often the sensor
    // takes a measurement.
    // based on VL53L0X_StartMeasurement()
    bool StartContinuous(uint32_t period_ms = 0)
    {
        Trace("StartContinuous");
        if (!TryWriteRegister8(0x80, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x00, 0x00))
            return false;
        if (!TryWriteRegister8(0x91, _stopVariable))
            return false;
        if (!TryWriteRegister8(0x00, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x80, 0x00))
            return false;

        if (period_ms != 0)
        {
            // continuous timed mode

            // VL53L0X_SetInterMeasurementPeriodMilliSeconds() begin

            uint16_t osc_calibrate_val = 0;
            if (!TryReadRegister16(Registers::OscCalibrateVal, &osc_calibrate_val))
                return false;

            if (osc_calibrate_val != 0)
            {
                period_ms *= osc_calibrate_val;
            }

            if (!TryWriteRegister32(Registers::SystemInterMeasurementPeriod, period_ms))
                return false;

            // VL53L0X_SetInterMeasurementPeriodMilliSeconds() end

            if (!TryWriteRegister8(Registers::SysRangeStart, 0x04)) // VL53L0X_REG_SYSRANGE_MODE_TIMED
                return false;
        }
        else
        {
            // continuous back-to-back mode
            if (!TryWriteRegister8(Registers::SysRangeStart, 0x02)) // VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK
                return false;
        }

        return true;
    }

    // Stop continuous measurements
    // based on VL53L0X_StopMeasurement()
    bool StopContinuous()
    {
        Trace("StopContinuous");
        if (!TryWriteRegister8(Registers::SysRangeStart, 0x01)) // VL53L0X_REG_SYSRANGE_MODE_SINGLESHOT
            return false;

        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x00, 0x00))
            return false;
        if (!TryWriteRegister8(0x91, 0x00))
            return false;
        if (!TryWriteRegister8(0x00, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x00))
            return false;

        return true;
    }

    // returns true when a measurement has been done. Call ReadRange_mm().
    bool isMeasurementReady()
    {
        uint8_t status = 0;
        if (!TryReadRegister8(Registers::ResultInterruptStatus, &status))
            return false;
        if ((status & 0x07) == 0)
            return false;

        return true;
    }

    // Returns a range reading in millimeters when continuous mode is active.
    // Returns false when no measurement was ready (or an error occurred).
    bool ReadRange_mm(uint16_t *outRange)
    {
        if (!isMeasurementReady())
            return false;

        // assumptions: Linearity Corrective Gain is 1000 (default);
        // fractional ranging is not enabled
        if (!TryReadRegister16(Registers::ResultRangeStatusPlus10, outRange))
            return false;

        if (!TryWriteRegister8(Registers::SystemInterruptClear, 0x01))
            return false;

        return true;
    }

private:
    // read by init and used when starting measurement;
    // is StopVariable field of VL53L0X_DevData_t structure in API
    uint8_t _stopVariable;
    uint32_t _measurementTimingBudget_us;
    ShutdownPinT _shutdownPin;
    uint8_t _address;

    bool TryReadRegister8(Registers reg, uint8_t *outData)
    {
        TwiResult result = I2cT::TryReadRegister8(_address, (uint8_t)reg, outData);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }
    bool TryReadRegister8(int reg, uint8_t *outData)
    {
        TwiResult result = I2cT::TryReadRegister8(_address, (uint8_t)reg, outData);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }

    bool TryReadRegister16(Registers reg, uint16_t *outData)
    {
        // TwiResult result = I2cT::TryReadRegister16(_address, (uint8_t)reg, outData);
        TwiResult result = _TryReadRegister16(_address, (uint8_t)reg, outData);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }

    // TODO: move this into Twi.h
    // This TryReadRegister16 reads last byte as NACK instead of ACK.

#define _PromoteFailure(result)  \
    if (I2cT::HasFailed(result)) \
    {                            \
        I2cT::Abort();           \
        return result;           \
    }

    static TwiResult _TryReadRegister16(uint8_t address, uint8_t reg, uint16_t *outData)
    {
        TwiResult result = I2cT::Start(address, false);
        _PromoteFailure(result);

        result = I2cT::Write(reg);
        _PromoteFailure(result);

        result = I2cT::Start(address, true);
        _PromoteFailure(result);

        uint8_t data = 0;
        if (!I2cT::TryReadAck(&data))
            return TwiResult::Timeout;

        *outData = (uint16_t)data << 8;
        if (!I2cT::TryReadNack(&data))
            return TwiResult::Timeout;

        *outData |= data;

        result = I2cT::Stop();
        _PromoteFailure(result);

        return TwiResult::Ok;
    }

    bool TryReadRegisterMulti(Registers reg, uint8_t *outData, uint8_t count)
    {
        TwiResult result = I2cT::TryReadRegisterMulti(_address, (uint8_t)reg, outData, count);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }

    bool TryWriteRegister8(Registers reg, uint8_t data)
    {
        TwiResult result = I2cT::WriteRegister8(_address, (uint8_t)reg, data);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }
    bool TryWriteRegister8(int reg, int data)
    {
        TwiResult result = I2cT::WriteRegister8(_address, (uint8_t)reg, (uint8_t)data);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }

    bool TryWriteRegister16(Registers reg, uint16_t data)
    {
        TwiResult result = I2cT::WriteRegister16(_address, (uint8_t)reg, data);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }
    bool TryWriteRegister16(int reg, int data)
    {
        TwiResult result = I2cT::WriteRegister16(_address, (uint8_t)reg, (uint16_t)data);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }

    bool TryWriteRegister32(Registers reg, uint32_t data)
    {
        TwiResult result = I2cT::WriteRegisterMulti(_address, (uint8_t)reg, (uint8_t *)&data, 4);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }

    bool TryWriteRegisterMulti(Registers reg, uint8_t *data, uint8_t count)
    {
        TwiResult result = I2cT::WriteRegisterMulti(_address, (uint8_t)reg, data, count);
        LogTwiError(result);
        return (result == TwiResult::Ok);
    }

    // Set the return signal rate limit check value in units of MCPS (mega counts
    // per second). "This represents the amplitude of the signal reflected from the
    // target and detected by the device"; setting this limit presumably determines
    // the minimum measurement necessary for the sensor to report a valid reading.
    // Setting a lower limit increases the potential range of the sensor but also
    // seems to increase the likelihood of getting an inaccurate reading because of
    // unwanted reflections from objects other than the intended target.
    // Defaults to 0.25 MCPS as initialized by the ST API and this library.
    bool TryWriteSignalRateLimit(float limit_Mcps)
    {
        Trace("TryWriteSignalRateLimit");
        if (limit_Mcps < 0 || limit_Mcps > 511.99)
            return false;

        // Q9.7 fixed point format (9 integer bits, 7 fractional bits)
        return TryWriteRegister16(Registers::FinalRangeConfigMinCountRateRtnLimit, (uint16_t)(limit_Mcps * (1 << 7)));
    }

    // Get reference SPAD (single photon avalanche diode) count and type
    // based on VL53L0X_get_info_from_device(),
    // but only gets reference SPAD count and type
    bool TryReadSpadInfo(uint8_t *count, bool *type_is_aperture)
    {
        Trace("TryReadSpadInfo");
        uint8_t tmp;

        if (!TryWriteRegister8(0x80, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x00, 0x00))
            return false;

        if (!TryWriteRegister8(0xFF, 0x06))
            return false;
        if (!TryReadRegister8(0x83, &tmp))
            return false;
        if (!TryWriteRegister8(0x83, tmp | 0x04))
            return false;
        if (!TryWriteRegister8(0xFF, 0x07))
            return false;
        if (!TryWriteRegister8(0x81, 0x01))
            return false;

        if (!TryWriteRegister8(0x80, 0x01))
            return false;

        if (!TryWriteRegister8(0x94, 0x6b))
            return false;
        if (!TryWriteRegister8(0x83, 0x00))
            return false;

        if (!TryReadRegister8(0x83, &tmp))
            return false;
        while (tmp == 0x00)
        {
            if (!TryReadRegister8(0x83, &tmp))
                return false;
        }

        if (!TryWriteRegister16(0x83, 0x01))
            return false;
        if (!TryReadRegister8(0x92, &tmp))
            return false;

        // assign out params
        *count = tmp & 0x7f;
        *type_is_aperture = (tmp >> 7) & 0x01;

        if (!TryWriteRegister8(0x81, 0x00))
            return false;
        if (!TryWriteRegister8(0xFF, 0x06))
            return false;
        if (!TryReadRegister8(0x83, &tmp))
            return false;
        if (!TryWriteRegister8(0x83, tmp & ~0x04))
            return false;
        if (!TryWriteRegister8(0xFF, 0x01))
            return false;
        if (!TryWriteRegister8(0x00, 0x01))
            return false;
        if (!TryWriteRegister8(0xFF, 0x00))
            return false;
        if (!TryWriteRegister8(0x80, 0x00))
            return false;

        return true;
    }

    // Get the measurement timing budget in microseconds
    // based on VL53L0X_get_measurement_timing_budget_micro_seconds()
    // in us
    bool TryReadMeasurementTimingBudget(uint32_t *outData)
    {
        Trace("TryReadMeasurementTimingBudget");

        SequenceStepEnables enables;
        SequenceStepTimeouts timeouts;
        if (!TryReadSequenceStepEnables(&enables))
            return false;
        if (!TryReadSequenceStepTimeouts(&enables, &timeouts))
            return false;

        uint32_t budget_us = Budget_StartOverhead + Budget_EndOverhead;

        if (enables.tcc)
        {
            budget_us += (timeouts.msrcDssTcc_us + Budget_TccOverhead);
        }

        if (enables.dss)
        {
            budget_us += 2 * (timeouts.msrcDssTcc_us + Budget_DssOverhead);
        }
        else if (enables.msrc)
        {
            budget_us += (timeouts.msrcDssTcc_us + Budget_MsrcOverhead);
        }

        if (enables.preRange)
        {
            budget_us += (timeouts.preRange_us + Budget_PreRangeOverhead);
        }

        if (enables.finalRange)
        {
            budget_us += (timeouts.finalRange_us + Budget_FinalRangeOverhead);
        }

        *outData = budget_us;
        return true;
    }

    // Set the measurement timing budget in microseconds, which is the time allowed
    // for one measurement; the ST API and this library take care of splitting the
    // timing budget among the sub-steps in the ranging sequence. A longer timing
    // budget allows for more accurate measurements. Increasing the budget by a
    // factor of N decreases the range measurement standard deviation by a factor of
    // sqrt(N). Defaults to about 33 milliseconds; the minimum is 20 ms.
    // based on VL53L0X_set_measurement_timing_budget_micro_seconds()
    bool TryWriteMeasurementTimingBudget(uint32_t budget_us)
    {
        Trace("TryWriteMeasurementTimingBudget");

        SequenceStepEnables enables;
        SequenceStepTimeouts timeouts;
        if (!TryReadSequenceStepEnables(&enables))
            return false;
        if (!TryReadSequenceStepTimeouts(&enables, &timeouts))
            return false;

        uint32_t used_budget_us = Budget_StartOverhead + Budget_EndOverhead;

        if (enables.tcc)
            used_budget_us += (timeouts.msrcDssTcc_us + Budget_TccOverhead);

        if (enables.dss)
            used_budget_us += 2 * (timeouts.msrcDssTcc_us + Budget_DssOverhead);
        else if (enables.msrc)
            used_budget_us += (timeouts.msrcDssTcc_us + Budget_MsrcOverhead);

        if (enables.preRange)
            used_budget_us += (timeouts.preRange_us + Budget_PreRangeOverhead);

        if (enables.finalRange)
        {
            used_budget_us += Budget_FinalRangeOverhead;

            // "Note that the final range timeout is determined by the timing
            // budget and the sum of all other timeouts within the sequence.
            // If there is no room for the final range timeout, then an error
            // will be set. Otherwise the remaining time will be applied to
            // the final range."

            if (used_budget_us > budget_us)
                // "Requested timeout too big."
                return false;

            uint32_t final_range_timeout_us = budget_us - used_budget_us;

            // set_sequence_step_timeout() begin
            // (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

            // "For the final range timeout, the pre-range timeout
            //  must be added. To do this both final and pre-range
            //  timeouts must be expressed in macro periods MClks
            //  because they have different vcsel periods."

            uint32_t final_range_timeout_mclks =
                TimeoutMicrosecondsToMclks(final_range_timeout_us, timeouts.finalRangeVcselPeriod_pclks);

            if (enables.preRange)
                final_range_timeout_mclks += timeouts.preRange_mclks;

            if (!TryWriteRegister16(Registers::FinalRangeConfigTimeoutMacropHi, EncodeTimeout(final_range_timeout_mclks)))
                return false;

            // set_sequence_step_timeout() end
        }

        return true;
    }

    // Get sequence step enables
    // based on VL53L0X_GetSequenceStepEnables()
    bool TryReadSequenceStepEnables(SequenceStepEnables *enables)
    {
        Trace("TryReadSequenceStepEnables()");

        uint8_t sequence_config = 0;
        if (!TryReadRegister8(Registers::SystemSequenceConfig, &sequence_config))
            return false;

        enables->tcc = (sequence_config >> 4) & 0x1;
        enables->dss = (sequence_config >> 3) & 0x1;
        enables->msrc = (sequence_config >> 2) & 0x1;
        enables->preRange = (sequence_config >> 6) & 0x1;
        enables->finalRange = (sequence_config >> 7) & 0x1;

        return true;
    }

    // Get sequence step timeouts
    // based on get_sequence_step_timeout(),
    // but gets all timeouts instead of just the requested one, and also stores
    // intermediate values
    bool TryReadSequenceStepTimeouts(SequenceStepEnables const *enables, SequenceStepTimeouts *timeouts)
    {
        Trace("TryReadSequenceStepTimeouts()");

        if (!TryReadVcselPulsePeriod(VcselPeriodType::VcselPeriodPreRange, &timeouts->preRangeVcselPeriod_pclks))
            return false;

        uint8_t data8 = 0;
        if (!TryReadRegister8(Registers::MsrcConfigTimeoutMacrop, &data8))
            return false;
        timeouts->msrcDssTcc_mclks = data8 + 1;
        timeouts->msrcDssTcc_us =
            TimeoutMclksToMicroseconds(timeouts->msrcDssTcc_mclks, timeouts->preRangeVcselPeriod_pclks);

        uint16_t data16 = 0;
        if (!TryReadRegister16(Registers::PreRangeConfigTimeoutMacropHi, &data16))
            return false;

        timeouts->preRange_mclks = DecodeTimeout(data16);
        timeouts->preRange_us =
            TimeoutMclksToMicroseconds(timeouts->preRange_mclks, timeouts->preRangeVcselPeriod_pclks);

        if (!TryReadVcselPulsePeriod(VcselPeriodType::VcselPeriodFinalRange, &timeouts->finalRangeVcselPeriod_pclks))
            return false;
        if (!TryReadRegister16(Registers::FinalRangeConfigTimeoutMacropHi, &data16))
            return false;
        timeouts->finalRange_mclks = DecodeTimeout(data16);

        if (enables->preRange)
            timeouts->finalRange_mclks -= timeouts->preRange_mclks;

        timeouts->finalRange_us =
            TimeoutMclksToMicroseconds(timeouts->finalRange_mclks, timeouts->finalRangeVcselPeriod_pclks);

        return true;
    }

    // Get the VCSEL pulse period in PCLKs for the given period type.
    // based on VL53L0X_get_vcsel_pulse_period()
    bool TryReadVcselPulsePeriod(VcselPeriodType type, uint16_t *outPeriod)
    {
        *outPeriod = 0;
        uint8_t period = 0;

        Trace("TryReadVcselPulsePeriod()");

        if (type == VcselPeriodType::VcselPeriodPreRange)
        {
            if (!TryReadRegister8(Registers::PreRangeConfigVcselPeriod, &period))
                return false;

            *outPeriod = DecodeVcselPeriod(period);
            return true;
        }
        else if (type == VcselPeriodType::VcselPeriodFinalRange)
        {
            if (!TryReadRegister8(Registers::FinalRangeConfigVcselPeriod, &period))
                return false;

            *outPeriod = DecodeVcselPeriod(period);
            return true;
        }

        *outPeriod = 255;
        return false;
    }

    // based on VL53L0X_perform_single_ref_calibration()
    bool PerformSingleRefCalibration(uint8_t vhv_init_byte)
    {
        Trace("PerformSingleRefCalibration");
        // VL53L0X_REG_SYSRANGE_MODE_START_STOP
        if (!TryWriteRegister8(Registers::SysRangeStart, 0x01 | vhv_init_byte))
            return false;

        uint8_t data = 0;
        if (!TryReadRegister8(Registers::ResultInterruptStatus, &data))
            return false;
        while ((data & 0x07) == 0)
        {
            if (!TryReadRegister8(Registers::ResultInterruptStatus, &data))
                return false;
        }

        if (!TryWriteRegister8(Registers::SystemInterruptClear, 0x01))
            return false;

        if (!TryWriteRegister8(Registers::SysRangeStart, 0x00))
            return false;

        return true;
    }

    // Decode VCSEL (vertical cavity surface emitting laser) pulse period in PCLKs
    // from register value. Based on VL53L0X_decode_vcsel_period()
    static uint8_t DecodeVcselPeriod(uint8_t reg_val)
    {
        return (reg_val + 1) << 1;
    }

    // Calculate macro period in *nanoseconds* from VCSEL period in PCLKs
    // based on VL53L0X_calc_macro_period_ps()
    // PLL_period_ps = 1655; macro_period_vclks = 2304
    static uint32_t CalcMacroPeriod(uint8_t vcsel_period_pclks)
    {
        return (((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000;
    }

    // Convert sequence step timeout from MCLKs to microseconds with given VCSEL period in PCLKs
    // based on VL53L0X_calc_timeout_us()
    static uint32_t TimeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks)
    {
        uint32_t macro_period_ns = CalcMacroPeriod(vcsel_period_pclks);
        return ((timeout_period_mclks * macro_period_ns) + 500) / 1000;
    }

    // Convert sequence step timeout from microseconds to MCLKs with given VCSEL period in PCLKs
    // based on VL53L0X_calc_timeout_mclks()
    static uint32_t TimeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks)
    {
        uint32_t macro_period_ns = CalcMacroPeriod(vcsel_period_pclks);
        return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
    }

    // Decode sequence step timeout in MCLKs from register value
    // based on VL53L0X_decode_timeout()
    // Note: the original function returned a uint32_t, but the return value is
    // always stored in a uint16_t.
    static uint16_t DecodeTimeout(uint16_t reg_val)
    {
        // format: "(LSByte * 2^MSByte) + 1"
        return (uint16_t)((reg_val & 0x00FF) << (uint16_t)((reg_val & 0xFF00) >> 8)) + 1;
    }

    // Encode sequence step timeout register value from timeout in MCLKs
    // based on VL53L0X_encode_timeout()
    static uint16_t EncodeTimeout(uint32_t timeout_mclks)
    {
        // format: "(LSByte * 2^MSByte) + 1"

        uint32_t ls_byte = 0;
        uint16_t ms_byte = 0;

        if (timeout_mclks > 0)
        {
            ls_byte = timeout_mclks - 1;

            while ((ls_byte & 0xFFFFFF00) > 0)
            {
                ls_byte >>= 1;
                ms_byte++;
            }

            return (ms_byte << 8) | (ls_byte & 0xFF);
        }
        else
        {
            return 0;
        }
    }

#ifdef DEBUG
    void LogTwiError(TwiResult result)
    {
        switch (result)
        {
        case TwiResult::AddressFailed:
            Error("I2C Address Failed");
            break;
        case TwiResult::DataFailed:
            Error("I2C Data Failed");
            break;
        case TwiResult::InvalidParameter:
            Error("I2C Invalid Parameter");
            break;
        case TwiResult::StartReadFailed:
            Error("I2C Start Read Failed");
            break;
        case TwiResult::StartWriteFailed:
            Error("I2C Start Write Failed");
            break;
        case TwiResult::Timeout:
            Error("I2C Timeout");
            break;
        default:
            break;
        }
    }
    void Error(const char *msg)
    {
        WriteDebugBuffer(msg);
        LogError<DebugComponentId>(_debugBuffer);
    }
    void Trace(const char *msg)
    {
        WriteDebugBuffer(msg);
        LogTrace<DebugComponentId>(_debugBuffer);
    }
    void WriteDebugBuffer(const char *msg)
    {
        _debugBuffer.Clear();
        _debugBuffer.Write("VL53L0X<0x");
        _debugBuffer.WriteInt(Address, 16);
        _debugBuffer.Write("|0x");
        _debugBuffer.WriteInt(_address, 16);
        _debugBuffer.Write("> ");
        _debugBuffer.Write(msg);
    }

    StringWriter<100> _debugBuffer;
#else
    static void LogTwiError(TwiResult result) {}
    static void Error(const char *msg) {}
    static void Trace(const char *msg) {}
#endif //~DEBUG
};