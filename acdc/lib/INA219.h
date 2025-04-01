#pragma once
#include <stdint.h>
#include "../lib/atl/Bit.h"

// registers
#define INA219_CONFIGURATION 0x00
#define INA219_SHUNT_VOLTAGE 0x01
#define INA219_BUS_VOLTAGE 0x02
#define INA219_POWER 0x03
#define INA219_CURRENT 0x04
#define INA219_CALIBRARION 0x05

// bits
#define INA219_CONFIGURATION_RESET 15

template <class I2cT, const uint8_t Address>
class INA219 : public I2cT
{
    static_assert((Address & 0x80) == 0, "INA219 Address highest bit (7) must be cleared. It is not used in I2C.");

public:
    const uint16_t DefaultAddress = 0x40;

    enum class BusVoltage : uint8_t
    {
        Volt16,
        Volt32
    };
    enum class Sensitivity : uint8_t
    {
        mV40,
        mV80,
        mV160,
        mV320
    };
    enum class AdcMode : uint8_t
    {
        Bit9us84,
        Bit10us148,
        Bit11us276,
        Bit12us532,

        Sample2ms1_06,
        Sample4ms2_13,
        Sample8ms4_26,
        Sample16ms8_51,
        Sample32ms17_02,
        Sample64ms34_05,
        Sample128ms68_10,
    };
    enum class Mode : uint8_t
    {
        Off,
        ShuntTriggered,
        BusTriggered,
        ShuntAndBusTriggered,
        AdcOff,
        ShuntContinuous,
        BusContunous,
        ShuntAndBusContinuous
    };

    // calibration needs a value for power and current to work
    static bool Open(uint16_t calibration,
                     Mode mode = Mode::ShuntAndBusContinuous,
                     AdcMode adcShunt = AdcMode::Bit12us532,
                     AdcMode adcBus = AdcMode::Bit12us532,
                     Sensitivity sensitivity = Sensitivity::mV320,
                     BusVoltage busVoltage = BusVoltage::Volt32)
    {
        uint16_t config = 0b0011100110011111;
        // config |= (uint16_t)mode;
        // config |= ((uint16_t)adcShunt << 3);
        // config |= ((uint16_t)adcBus << 7);
        // config |= ((uint16_t)sensitivity << 11);
        // config |= ((uint16_t)busVoltage << 13);

        // return Write(INA219_CONFIGURATION, config);
        return Write(INA219_CONFIGURATION, config) &&
               Write(INA219_CALIBRARION, calibration);
    }

    static bool TryReadBusVoltage(int16_t *outData)
    {
        // TODO: status bits + shift value down
        return TryRead(INA219_BUS_VOLTAGE, outData);
    }

    static bool TryReadShuntVoltage(int16_t *outData)
    {
        return TryRead(INA219_SHUNT_VOLTAGE, outData);
    }

    static bool TryReadCurrent(int16_t *outData)
    {
        return TryRead(INA219_CURRENT, outData);
    }

    static bool TryReadPower(uint16_t *outData)
    {
        return TryRead(INA219_POWER, outData);
    }

    static bool TryReadConfiguration(uint16_t *outData)
    {
        return TryRead(INA219_CONFIGURATION, outData);
    }

    static bool TryReadCalibraition(uint16_t *outData)
    {
        return TryRead(INA219_CALIBRARION, outData);
    }

    static bool Reset()
    {
        return Write(INA219_CONFIGURATION, Bit<INA219_CONFIGURATION_RESET>::getMask<uint16_t>());
    }

private:
    static bool TryRead(uint8_t reg, uint16_t *outData)
    {
        return I2cT::TryReadRegister16(Address, reg, outData) == TwiResult::Ok;
    }
    static bool TryRead(uint8_t reg, int16_t *outData)
    {
        uint16_t data;
        bool result = I2cT::TryReadRegister16(Address, reg, &data) == TwiResult::Ok;
        if (result)
        {
            *outData = static_cast<int16_t>(data);
        }
        return result;
    }
    static bool Write(uint8_t reg, uint16_t data)
    {
        return I2cT::WriteRegister16(Address, reg, data) == TwiResult::Ok;
    }
};
