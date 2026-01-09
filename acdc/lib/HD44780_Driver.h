#pragma once
#include "HD44780_Profile.h"
#include "atl/Debug.h"

/** This class implements driver logic for the HD44780 LCD display.
 *  It assumes a write-only (5-R/W connected to GND) 4-bit mode (soft-mode).
 *  Note that in 4-bit mode only the high data lines (4-7) of the display are used.
 *  See also: http://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller
 *  \tparam DriverIoT performs the actual driver IO (for instance HD44780_DriverPins).
 *  \tparam TimingProfileT is used to perform synchronous delays.
 *  It can be used to fine-tune the synchronous delays. See also HD44780_Profile.
 */
template <typename DriverIoT, typename TimingProfileT = HD44780_Profile>
class HD44780_Driver : public DriverIoT
{
    typedef DriverIoT BaseT;

public:
    static const uint8_t DebugComponentId = 21;

    /** Constructs a new instance and sets the Enable pin HIGH.
     */
    HD44780_Driver()
    {
        BaseT::SetEnable(true);
        BaseT::WriteIO();
    }

    /** Writes a command to the LCD.
     *  Pulls the RS LOW to indicate a command.
     *  \param cmd is the command to write.
     */
    bool WriteCommand(uint8_t cmd)
    {
        Trace("WriteCommand");
        BaseT::SetRs(false);
        return Write8(cmd);
    }

    /** Writes data to the LCD.
     *  Pulls the RS HIGH to indicate data.
     *  \param data is the data to write.
     */
    bool WriteData(uint8_t data)
    {
        Trace("WriteData");
        BaseT::SetRs(true);
        return Write8(data);
    }

protected:
    /** Writes 8-bits to the display - MSB first.
     *  \param data is all 8 bits.
     */
    bool Write8(uint8_t data)
    {
        if (!Write4(data >> 4))
            return false;
        return Write4(data);
    }

    /** Writes 4-bits to the display.
     *  Pulses Enable to let the display read the data.
     *  \param data is all 4 bits in the lower nibble.
     */
    bool Write4(uint8_t data)
    {
        BaseT::Set04(data & 0x01);
        BaseT::Set15(data & 0x02);
        BaseT::Set26(data & 0x04);
        BaseT::Set37(data & 0x08);
        if (!BaseT::WriteIO())
            return false;

        return PulseEnable();
    }

private:
    bool PulseEnable()
    {
        Trace("PulseEnable");
        BaseT::SetEnable(false);
        if (!BaseT::WriteIO())
            return false;

        TimingProfileT::WaitPulseEnable();

        BaseT::SetEnable(true);
        return BaseT::WriteIO();
    }

    static void Trace(const char *msg)
    {
        LogTrace<DebugComponentId>(msg);
    }
};