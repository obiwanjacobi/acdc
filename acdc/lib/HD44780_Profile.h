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
    static void WaitPowerUp()
    {
        Time<TimeResolution::Milliseconds>::Wait(50);
    }

    /** Performs a delay that is part of the full initialization.
     */
    static void WaitInitializeLong()
    {
        Time<TimeResolution::Microseconds>::Wait(4100);
    }

    /** Performs a delay that is part of the full initialization.
     */
    static void WaitInitialize()
    {
        Time<TimeResolution::Microseconds>::Wait(100);
    }

    /** Performs a delay that is part of the full initialization.
     */
    static void WaitInitializeShort()
    {
        Time<TimeResolution::Microseconds>::Wait(40);
    }

    /** Performs a delay the display needs to process a command.
     */
    static void WaitForCommand()
    {
        Time<TimeResolution::Microseconds>::Wait(40);
    }

    /** Performs a delay the display needs to process a command.
     */
    static void WaitForCommandLong()
    {
        Time<TimeResolution::Microseconds>::Wait(1640);
    }

    /** Performs a delay the display needs to read the enable line.
     */
    static void WaitPulseEnable()
    {
        Time<TimeResolution::Microseconds>::Wait(37);
    }

private:
    HD44780_Profile() {}
};

/** The HD44780_ProfileTweak implements waiting times tweaked to a  minimum.
 *  Might not work with all displays.
 *  This is a static class and cannot be instantiated.
 */
class HD44780_ProfileTweaked
{
public:
    /** Performs a delay the display needs to power up.
     */
    static void WaitPowerUp()
    {
        Time<TimeResolution::Milliseconds>::Wait(2);
    }

    /** Performs a delay that is part of the full initialization.
     */
    static void WaitInitializeLong()
    {
        Time<TimeResolution::Microseconds>::Wait(500);
    }

    /** Performs a delay that is part of the full initialization.
     */
    static void WaitInitialize()
    {
        Time<TimeResolution::Microseconds>::Wait(10);
    }

    /** Performs a delay that is part of the full initialization.
     */
    static void WaitInitializeShort()
    {
        Time<TimeResolution::Microseconds>::Wait(5);
    }

    /** Performs a delay the display needs to process a command.
     */
    static void WaitForCommand()
    {
        Time<TimeResolution::Microseconds>::Wait(5);
    }

    /** Performs a delay the display needs to process a command.
     */
    static void WaitForCommandLong()
    {
        Time<TimeResolution::Microseconds>::Wait(400);
    }

    /** Performs a delay the display needs to read the enable line.
     */
    static void WaitPulseEnable()
    {
        Time<TimeResolution::Microseconds>::Wait(1);
    }

private:
    HD44780_ProfileTweaked() {}
};
