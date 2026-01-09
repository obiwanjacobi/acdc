#pragma once

#include "atl/Time.h"

/** The HD44780_Profile class contains static methods that perform synchronous delays used for timing the digital signals to the LCD display.
 *  It is a static class and cannot be instantiated.
 */
class HD44780_Profile
{
public:
    /** Performs a delay the display needs to power up.
     */
    inline static void WaitPowerUp()
    {
        Time<TimeResolution::Milliseconds>::Wait(50);
    }

    /** Performs a delay that is part of the full initialization.
     */
    inline static void WaitInitializeLong()
    {
        Time<TimeResolution::Microseconds>::Wait(4100);
    }

    /** Performs a delay that is part of the full initialization.
     */
    inline static void WaitInitialize()
    {
        Time<TimeResolution::Microseconds>::Wait(100);
    }

    /** Performs a delay that is part of the full initialization.
     */
    inline static void WaitInitializeShort()
    {
        Time<TimeResolution::Microseconds>::Wait(40);
    }

    /** Performs a delay the display needs to process a command.
     */
    inline static void WaitForCommand()
    {
        Time<TimeResolution::Microseconds>::Wait(40);
    }

    /** Performs a delay the display needs to process a command.
     */
    inline static void WaitForCommandLong()
    {
        Time<TimeResolution::Microseconds>::Wait(1640);
    }

    /** Performs a delay the display needs to read the enable line.
     */
    inline static void WaitPulseEnable()
    {
        Time<TimeResolution::Microseconds>::Wait(37);
    }

private:
    HD44780_Profile() {}
};

/** The HD44780_ProfileTweak implements waiting times tweaked to a  minimum.
 *  It is a static class and cannot be instantiated.
 */
class HD44780_ProfileTweaked
{
public:
    /** Performs a delay the display needs to power up.
     */
    inline static void WaitPowerUp()
    {
        Time<TimeResolution::Milliseconds>::Wait(10);
    }

    /** Performs a delay that is part of the full initialization.
     */
    inline static void WaitInitializeLong()
    {
        Time<TimeResolution::Microseconds>::Wait(1000);
    }

    /** Performs a delay that is part of the full initialization.
     */
    inline static void WaitInitialize()
    {
        Time<TimeResolution::Microseconds>::Wait(30);
    }

    /** Performs a delay that is part of the full initialization.
     */
    inline static void WaitInitializeShort()
    {
        Time<TimeResolution::Microseconds>::Wait(20);
    }

    /** Performs a delay the display needs to process a command.
     */
    inline static void WaitForCommand()
    {
        Time<TimeResolution::Microseconds>::Wait(15);
    }

    /** Performs a delay the display needs to process a command.
     */
    inline static void WaitForCommandLong()
    {
        Time<TimeResolution::Microseconds>::Wait(800);
    }

    /** Performs a delay the display needs to read the enable line.
     */
    inline static void WaitPulseEnable()
    {
        Time<TimeResolution::Microseconds>::Wait(8);
    }

private:
    HD44780_ProfileTweaked() {}
};
