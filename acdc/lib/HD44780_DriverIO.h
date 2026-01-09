#pragma once
#include <stdint.h>

/** This class performs HD44780 Driver IO through indiviual output pins.
 *  \tparam RegSelPinT is a DigitalOutputPin that is connected to RS of the display.
 *  \tparam EnablePinT is a DigitalOutputPin that is connected to Enable of the display.
 *  \tparam Data04PinT is a DigitalOutputPin that is connected to D4 of the display.
 *  \tparam Data15PinT is a DigitalOutputPin that is connected to D5 of the display.
 *  \tparam Data26PinT is a DigitalOutputPin that is connected to D6 of the display.
 *  \tparam Data37PinT is a DigitalOutputPin that is connected to D7 of the display.
 */
template <typename RegSelPinT, typename EnablePinT, typename Data04PinT,
          typename Data15PinT, typename Data26PinT, typename Data37PinT>
class HD44780_DriverPins
{
public:
    const uint8_t DebugComponentId = 21;

    HD44780_DriverPins() {}

protected:
    void SetEnable(bool value)
    {
        _enable.Write(value);
    }
    void SetRs(bool value)
    {
        _rs.Write(value);
    }
    void Set04(bool value)
    {
        _data04.Write(value);
    }
    void Set15(bool value)
    {
        _data15.Write(value);
    }
    void Set26(bool value)
    {
        _data26.Write(value);
    }
    void Set37(bool value)
    {
        _data37.Write(value);
    }
    // for other cases where combined bits are written together
    bool WriteIO() { return true; }

private:
    RegSelPinT _rs;
    EnablePinT _enable;
    Data04PinT _data04;
    Data15PinT _data15;
    Data26PinT _data26;
    Data37PinT _data37;
};

//-----------------------------------------------------------------------------

/** This class implements driver IO logic for the HD44780 LCD display controlled over a serial connection.
 *  It assumes the serial connection can be shared with other peripherals/devices.
 *  It uses a BitArray to store all the bits that are clocked out over the serial connection.
 *  This BitArray is shared amongst all 'devices' that share the serial connection (which can be chained to 'any' length).
 *  It is important to call the `setDataRegister()` method (and check the return value)
 *      BEFORE calling Initialize on the controller.
 *
 *  \tparam BaseT is a Serial-Output type that implements `void Write(const BitArray<T>&)`.
 *  \tparam RegSetIndex is the index in BitArrayT of the RS line to the display.
 *  \tparam EnableIndex is the index in BitArrayT of the enable line to the display.
 *  \tparam Data04Index is the index in BitArrayT of the D4 line to the display.
 *  \tparam Data15Index is the index in BitArrayT of the D5 line to the display.
 *  \tparam Data26Index is the index in BitArrayT of the D6 line to the display.
 *  \tparam Data37Index is the index in BitArrayT of the D7 line to the display.
 *  \tparam BitArrayT is the type that holds the serial bits at the specific indexes.
 *      It can be used to fine-tune the synchronous delays. See also HD44780_Profile.
 */
template <typename BaseT, const uint8_t RegSelIndex, const uint8_t EnableIndex,
          const uint8_t Data04Index, const uint8_t Data15Index, const uint8_t Data26Index, const uint8_t Data37Index,
          typename BitArrayT>
class HD44780_DriverBitArray : public BaseT
{
public:
    const uint8_t DebugComponentId = 21;

    HD44780_DriverBitArray()
        : _dataReg(nullptr)
    {
    }

    /** Stores the pointer to the dataReg and initializes the lines to the display (all low except enable).
     *  \param dataReg is a pointer to the shared BitArray. Must not be nullptr.
     */
    bool setDataRegister(BitArrayT *dataReg)
    {
        if (dataReg == nullptr)
            return false;

        _dataReg = dataReg;

        WriteBit(RegSelIndex, false);
        WriteBit(EnableIndex, true);
        WriteBit(Data04Index, false);
        WriteBit(Data15Index, false);
        WriteBit(Data26Index, false);
        WriteBit(Data37Index, false);

        return WriteIO();
    }

protected:
    void SetEnable(bool value)
    {
        WriteBit(EnableIndex, value);
    }
    void SetRs(bool value)
    {
        WriteBit(RegSelIndex, value);
    }
    void Set04(bool value)
    {
        WriteBit(Data04Index, value);
    }
    void Set15(bool value)
    {
        WriteBit(Data15Index, value);
    }
    void Set26(bool value)
    {
        WriteBit(Data26Index, value);
    }
    void Set37(bool value)
    {
        WriteBit(Data37Index, value);
    }
    bool WriteIO()
    {
        if (_dataReg == nullptr)
            return false;

        return BaseT::Write(*_dataReg);
    }

private:
    BitArrayT *_dataReg;

    bool WriteBit(uint8_t bitIndex, bool value)
    {
        if (_dataReg == nullptr)
            return false;

        return _dataReg->Set(bitIndex, value);
    }
};
