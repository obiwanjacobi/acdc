#pragma once
#include <stdint.h>
#include "atl/Bit.h"
#include "atl/Debug.h"
#include "atl/StringUtils.h"

// prescale value range
#define PCA9685_PRESCALE_MIN 3
#define PCA9685_PRESCALE_MAX 255

#define PCA9685_I2C_DEFAULT_ADDRESS 0x40
#define PCA9685_FREQUENCY 25000000

enum class PCA9685_Pins : uint8_t
{
    Pin0 = 0,
    Pin1 = 1,
    Pin2 = 2,
    Pin3 = 3,
    Pin4 = 4,
    Pin5 = 5,
    Pin6 = 6,
    Pin7 = 7,
    Pin8 = 8,
    Pin9 = 9,
    Pin10 = 10,
    Pin11 = 11,
    Pin12 = 12,
    Pin13 = 13,
    Pin14 = 14,
    Pin15 = 15
};

template <class I2cT, const uint8_t Address>
class PCA9685 : public I2cT
{
    static_assert((Address & 0x80) == 0, "PCA9685 Address highest bit (7) must be cleared. It is not used in I2C.");

#define PropagateResult(result, value) \
    if (I2cT::HasFailed(result))       \
        return value;

    const uint8_t DefaultAddress = 0x40;

public:
    enum class ClockSource : uint8_t
    {
        Internal = 0,
        External = 1
    };

    // does not open Twi/I2c
    // prescale:
    //  100 = ~70Hz
    //  70 = ~100Hz
    //  50 = ~137Hz
    //  20 = ~342Hz
    //  12 = ~552Hz
    //  10 = ~655Hz
    //  0 = ~1800Hz
    static bool Open(uint16_t prescale = 0, ClockSource clockSource = ClockSource::Internal)
    {
        // TODO: check prescale against valid range

        // set sleep mode and auto increment
        uint8_t mode1 = getMask(Mode1::Sleep);
        if (!WriteMode1(mode1))
            return false;

        // set clock source
        if (clockSource == ClockSource::External)
        {
            mode1 |= getMask(Mode1::ExternalClock);
            if (!WriteMode1(mode1))
                return false;
        }

        // set prescale
        if (!WriteReg(Register::Prescale, prescale))
            return false;

        // clear sleep mode
        mode1 &= ~getMask(Mode1::Sleep);
        mode1 |= getMask(Mode1::AutoIncrement) | getMask(Mode1::Restart);
        if (!WriteMode1(mode1))
            return false;

        return true;
    }

    // dutyCycle value: 0-4095
    static bool Write(PCA9685_Pins pin, uint16_t dutyCycle)
    {
        return Write(pin, 0, dutyCycle);
    }
    // on/off value: 0-4095
    static bool Write(PCA9685_Pins pin, uint16_t on, uint16_t off)
    {
        if (on > 4095 || off > 4095)
            return false;

        // TODO
        // fullOn LEDn_ON_H:4 = 0b1
        // fullOff LEDn_OFF_H:4 = 0b1

        TwiResult result = I2cT::Start(Address, false);
        PropagateResult(result, false);
        result = I2cT::Write((uint8_t)Register::Led0OnL + (uint8_t)pin * 4);
        PropagateResult(result, false);
        result = I2cT::Write(on & 0xFF);
        PropagateResult(result, false);
        result = I2cT::Write(on >> 8);
        PropagateResult(result, false);
        result = I2cT::Write(off & 0xFF);
        PropagateResult(result, false);
        result = I2cT::Write(off >> 8);
        PropagateResult(result, false);
        result = I2cT::Stop();
        PropagateResult(result, false);

        return true;
    }

    static void Sleep()
    {
        uint8_t mode1 = 0;
        if (TryReadMode1(&mode1))
        {
            BitFlag::Set(mode1, (uint8_t)Mode1::Sleep, true);
            WriteMode1(mode1);
        }
    }
    static void WakeUp()
    {
        uint8_t mode1 = 0;
        if (TryReadMode1(&mode1))
        {
            BitFlag::Set(mode1, (uint8_t)Mode1::Sleep, false);
            WriteMode1(mode1);
        }
    }

    // how are the outputs driven
    enum class OutputDriver : uint8_t
    {
        OpenDrain = 0,
        PushPull = 1
    };
    // what happens when /OE is inactive
    enum class OutputEnable : uint8_t
    {
        // set output to zero
        Off,
        // output is high impedance
        HighImpedance,
        // OpenDrain=HighImpedance, PushPull=Off
        OutputDriver,
    };
    // is the output level inverted
    enum class OutputInverted : uint8_t
    {
        NotInverted = 0,
        Inverted = 1,
        // OpenDrain=Inverted, PushPull=NotInverted
        OutputDriver = 2,
    };
    static bool setOutputMode(OutputDriver outputDriver,
                              OutputEnable outputEnable = OutputEnable::OutputDriver,
                              OutputInverted outputInverted = OutputInverted::OutputDriver)
    {
        uint8_t mode2 = 0;
        bool isTotem = outputDriver == OutputDriver::PushPull;

        if (isTotem)
        {
            mode2 |= getMask(Mode2::OutputDriver);
        }

        switch (outputEnable)
        {
        case OutputEnable::OutputDriver:
            mode2 |= getMask(Mode2::OutputEnable0);
            break;
        case OutputEnable::HighImpedance:
            mode2 |= getMask(Mode2::OutputEnable1);
            break;
        default:
            break;
        }

        if (outputInverted == OutputInverted::OutputDriver)
        {
            outputInverted = isTotem ? OutputInverted::NotInverted : OutputInverted::Inverted;
        }
        if (outputInverted == OutputInverted::Inverted)
        {
            mode2 |= getMask(Mode2::Invert);
        }

        return WriteReg(Register::Mode2, mode2);
    }

private:
    enum class Register : uint8_t
    {
        Mode1 = 0x00,
        Mode2 = 0x01,
        SubAdr1 = 0x02,
        SubAdr2 = 0x03,
        SubAdr3 = 0x04,
        AllCallAdr = 0x05,
        Led0OnL = 0x06,
        Led0OnH = 0x07,
        Led0OffL = 0x08,
        Led0OffH = 0x09,
        // ... till LED15, 4 registers per LED
        AllLedOnL = 0xFA,
        AllLedOnH = 0xFB,
        AllLedOffL = 0xFC,
        AllLedOffH = 0xFD,
        Prescale = 0xFE,
    };

    enum class Mode1 : uint8_t
    {
        AllCall = 0,
        Sub3 = 1,
        Sub2 = 2,
        Sub1 = 3,
        Sleep = 4,
        AutoIncrement = 5,
        ExternalClock = 6,
        Restart = 7
    };

    enum class Mode2 : uint8_t
    {
        OutputEnable0 = 0,
        OutputEnable1 = 1,
        OutputDriver = 2,
        OutputChangeOnStop = 3,
        Invert = 4
    };

    static uint8_t getMask(Register bit)
    {
        return BitFlag::getMask<uint8_t>((uint8_t)bit);
    }
    static uint8_t getMask(Mode1 bit)
    {
        return BitFlag::getMask<uint8_t>((uint8_t)bit);
    }
    static uint8_t getMask(Mode2 bit)
    {
        return BitFlag::getMask<uint8_t>((uint8_t)bit);
    }

    PCA9685() {}

    static bool TryReadMode1(uint8_t *outData)
    {
        return TryReadReg(Register::Mode1, outData);
    }

    static bool TryReadReg(Register reg, uint8_t *outData)
    {
        return I2cT::TryReadRegister8(Address, reg, outData) == TwiResult::Ok;
    }

    static bool WriteMode1(uint8_t data)
    {
        return WriteReg(Register::Mode1, data);
    }

    static bool WriteReg(Register reg, uint8_t data)
    {
        return I2cT::WriteRegister8(Address, (uint8_t)reg, data) == TwiResult::Ok;
    }
};
