#pragma once
#include <stdint.h>
#include "atl/Bit.h"

// https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/blob/master/Adafruit_PWMServoDriver.cpp
// https://github.com/adafruit/Adafruit_BusIO/blob/master/Adafruit_I2CDevice.cpp

#define CheckResult(result)      \
    if (I2cT::HasFailed(result)) \
        return result;

#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01
#define PCA9685_SUBADR1 0x02
#define PCA9685_SUBADR2 0x03
#define PCA9685_SUBADR3 0x04
#define PCA9685_ALLCALLADR 0x05
#define PCA9685_LED0_ON_L 0x06
#define PCA9685_LED0_ON_H 0x07
#define PCA9685_LED0_OFF_L 0x08
#define PCA9685_LED0_OFF_H 0x09
// ... till LED15, 4 registers per LED
#define PCA9685_ALL_LED_ON_L 0xFA
#define PCA9685_ALL_LED_ON_H 0xFB
#define PCA9685_ALL_LED_OFF_L 0xFC
#define PCA9685_ALL_LED_OFF_H 0xFD
#define PCA9685_PRESCALE 0xFE

// mode 1 bits
#define PCA9685_MODE1_RESTART 7
#define PCA9685_MODE1_EXTCLK 6
#define PCA9685_MODE1_AI 5
#define PCA9685_MODE1_SLEEP 4
#define PCA9685_MODE1_SUB1 3
#define PCA9685_MODE1_SUB2 2
#define PCA9685_MODE1_SUB3 1
#define PCA9685_MODE1_ALLCALL 0
// mode 2 bits
#define PCA9685_MODE2_INVRT 4
#define PCA9685_MODE2_OCH 3
#define PCA9685_MODE2_OUTDRV 2
#define PCA9685_MODE2_OUTNE_1 1
#define PCA9685_MODE2_OUTNE_0 0
// prescale value range
#define PCA9685_PRESCALE_MIN 3
#define PCA9685_PRESCALE_MAX 255

#define PCA9685_I2C_DEFAULT_ADDRESS 0x40
#define PCA9685_FREQUENCY 25000000

template <class I2cT, const uint8_t Address>
class PCA9685
{
public:
    PCA9685() {}

    enum class Pins
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

    enum class ClockSource : uint8_t
    {
        Internal = 0,
        External = 1
    };

    // does not open Twi
    void Open(uint16_t prescale, ClockSource clockSource = ClockSource::Internal)
    {
        // set sleep mode
        // set prescale
        // set clock source
        // set auto increment
        // clear sleep mode
    }

    // dutyCycle value: 0-4095
    bool Write(Pins pin, uint16_t dutyCycle)
    {
        return Write(pin, 0, dutyCycle);
    }
    // on/off value: 0-4095
    bool Write(Pins pin, uint16_t on, uint16_t off)
    {
        if (on > 4095 || off > 4095)
            return false;

        // TODO
        // fullOn LEDn_ON_H:4 = 0b1
        // fullOff LEDn_OFF_H:4 = 0b1

        TwiResult result = I2cT::Start(Address, false);
        CheckResult(result);
        result = I2cT::Write(PCA9685_LED0_ON_L + (uint8_t)pin * 4);
        CheckResult(result);
        result = I2cT::Write(on & 0xFF);
        CheckResult(result);
        result = I2cT::Write(on >> 8);
        CheckResult(result);
        result = I2cT::Write(off & 0xFF);
        CheckResult(result);
        result = I2cT::Write(off >> 8);
        CheckResult(result);
        I2cT::Stop();

        return true;
    }

    void Sleep()
    {
        uint8_t mode1 = ReadMode1();
        BitFlag::Set(mode1, PCA9685_MODE1_SLEEP, true);
        WriteMode1(mode1);
    }
    void WakeUp()
    {
        uint8_t mode1 = ReadMode1();
        BitFlag::Set(mode1, PCA9685_MODE1_SLEEP, false);
        WriteMode1(mode1);
    }

    enum class OutputMode : uint8_t
    {
        OpenDrain = 0,
        PushPull = 1
    };
    void setOutputMode(OutputMode)
    {
    }

private:
    uint8_t ReadPrescale()
    {
        return 0;
    }
};
