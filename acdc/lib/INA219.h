#pragma once
#include <stdint.h>
#include "../lib/atl/Bit.h"

// config bits
#define INA219_CONFIGURATION_RESET 15

template <class I2cT, const uint8_t Address>
class INA219 : public I2cT
{
    static_assert((Address & 0x80) == 0, "INA219 Address highest bit (7) must be cleared. It is not used in I2C.");

public:
    const uint16_t DefaultAddress = 0x40;

    enum class BusVoltage : uint8_t
    {
        Volt16 = 0x00,
        Volt32 = 0x01
    };
    enum class Sensitivity : uint8_t
    {
        mV40 = 0x00,
        mV80 = 0x01,
        mV160 = 0x02,
        mV320 = 0x03
    };
    enum class AdcMode : uint8_t
    {
        Bit9 = 0x00,
        Bit10 = 0x01,
        Bit11 = 0x02,
        Bit12 = 0x03,

        Average2 = 0x09,
        Average4 = 0x0A,
        Average8 = 0x0B,
        Average16 = 0x0C,
        Average32 = 0x0D,
        Average64 = 0x0E,
        Average128 = 0x0F,
    };
    enum class Mode : uint8_t
    {
        Off = 0x00,
        ShuntTriggered = 0x01,
        BusTriggered = 0x02,
        ShuntAndBusTriggered = 0x03,
        AdcOff = 0x04,
        ShuntContinuous = 0x05,
        BusContinuous = 0x06,
        ShuntAndBusContinuous = 0x07
    };

    // calibration needs a value for power and current to work
    static bool Open(uint16_t calibration = 0,
                     Mode mode = Mode::ShuntAndBusContinuous,
                     Sensitivity sensitivity = Sensitivity::mV320,
                     AdcMode adcShunt = AdcMode::Bit12,
                     AdcMode adcBus = AdcMode::Bit12,
                     BusVoltage busVoltage = BusVoltage::Volt32)
    {
        // uint16_t config = 0b0011100110011111;
        uint16_t config = 0;

        config |= (uint16_t)mode;
        config |= ((uint16_t)adcShunt << 3);
        config |= ((uint16_t)adcBus << 7);
        config |= ((uint16_t)sensitivity << 11);
        config |= ((uint16_t)busVoltage << 13);

        return Write(Register::Configuration, config) &&
               Write(Register::Calibration, calibration);
    }

    static bool TryReadBusVoltage(int16_t *outData)
    {
        // TODO: extract status bits
        int16_t bus = 0;
        if (TryRead(Register::BusVoltage, &bus))
        {
            // lower bits are status bits
            *outData = bus >> 3;
            return true;
        }
        return false;
    }

    static bool TryReadShuntVoltage(int16_t *outData)
    {
        return TryRead(Register::ShuntVoltage, outData);
    }

    static bool TryReadCurrent(int16_t *outData)
    {
        return TryRead(Register::Current, outData);
    }

    static bool TryReadPower(uint16_t *outData)
    {
        return TryRead(Register::Power, outData);
    }

    static bool TryReadConfiguration(uint16_t *outData)
    {
        return TryRead(Register::Configuration, outData);
    }

    static bool TryReadCalibraition(uint16_t *outData)
    {
        return TryRead(Register::Calibration, outData);
    }

    static bool Reset()
    {
        return Write(Register::Configuration, Bit<INA219_CONFIGURATION_RESET>::getMask<uint16_t>());
    }

    static uint8_t getAddress()
    {
        return Address;
    }

private:
    enum class Register : uint8_t
    {
        Configuration = 0x00,
        ShuntVoltage = 0x01,
        BusVoltage = 0x02,
        Power = 0x03,
        Current = 0x04,
        Calibration = 0x05
    };

    static bool TryRead(Register reg, uint16_t *outData)
    {
        return I2cT::TryReadRegister16(Address, reg, outData) == TwiResult::Ok;
    }
    static bool TryRead(Register reg, int16_t *outData)
    {
        uint16_t data;
        bool result = I2cT::TryReadRegister16(Address, (uint8_t)reg, &data) == TwiResult::Ok;
        if (result)
        {
            *outData = static_cast<int16_t>(data);
        }
        return result;
    }
    static bool Write(Register reg, uint16_t data)
    {
        return I2cT::WriteRegister16(Address, (uint8_t)reg, data) == TwiResult::Ok;
    }
};
