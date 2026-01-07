#pragma once
#include <stdint.h>

/**
 * A static class to drive the PCF8574 IO expander IC over I2C.
 * Addresses range from 0x20 to 0x27.
 * To use the /INT pin (active-low) of the PCF8574 connect it to an input pin
 * and trigger an interrupt on the falling edge.
 *
 * I2cT is the Twi type with both Transmit and Recieve.
 * Address is the address configured (A0-A2 pins) on PCF8574.
 */
template <class I2cT, const uint8_t Address>
class PCF8574
{
    static_assert((Address & 0x80) == 0, "PCF8574 Address highest bit (7) must be cleared. It is not used in I2C.");

public:
    const uint8_t DefaultAddress = 0x20;

    static bool TryRead(uint8_t *outData)
    {
        return I2cT::TryReadDirect8(Address, outData) == TwiResult::Ok;
    }

    static bool TryWrite(uint8_t data)
    {
        return I2cT::WriteDirect8(Address, data) == TwiResult::Ok;
    }

private:
    PCF8574() {}
};
