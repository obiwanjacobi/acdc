#pragma once
#include <stdint.h>

/** \tparam I2cT represent the I2C interface with WriteDirect8().
 *  \tparam Address is the I2C address.
 *  \tparam Reversed (true) will output the hi bits first.
 */
template <class I2cT, const uint8_t Address>
class I2C_BitArrayWriter
{
    static_assert((Address & 0x80) == 0, "I2C_BitArrayWriter Address highest bit (7) must be cleared. It is not used in I2C.");

public:
    /** Writes the dataBits to the SerialDataPinT.
     *  Writes out lo bits first.
     *  \param dataBits is the BitArray that holds all the bits.
     */
    bool Write(const BitArray<uint8_t> &dataBits)
    {
        return I2cT::WriteDirect8(Address, dataBits.GetAll()) == TwiResult::Ok;
    }

    template <typename T>
    bool Write(const BitArray<T> &dataBits)
    {
        // todo:
        // return I2cT::WriteDirectMultiple(Address, dataBits.getMaxBits()) == TwiResult::Ok;
        return false;
    }
};
