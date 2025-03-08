#pragma once
#include <stdint.h>
#include "TimeResolution.h"
#include "../TimerCounter.h"

/** The Time class keeps track of time ticks (either milli- or micro-seconds).
 *  Time is a static class and cannot be instantiated.
 *  \tparam TimeResolution indicates the units of time.
 */
template <const TimeResolution ResolutionId>
class Time
{
public:
    /** Captures the time ticks.
     *  \return Returns delta-time in 'resolution'
     */
    static uint32_t Update();

    /** Returns the time ticks in milli-seconds.
     */
    static uint32_t getMilliseconds()
    {
        return ::getMilliseconds<ResolutionId>(_ticks);
    }

    /** Returns the time ticks in micro-seconds.
     */
    static uint32_t getMicroseconds()
    {
        return ::getMicroseconds<ResolutionId>(_ticks);
    }

    /** Returns the TimeResolution template parameter.
     */
    TimeResolution getResolution() const
    {
        return ResolutionId;
    }

    /** Returns the raw time ticks.
     */
    static uint32_t getTicks()
    {
        return _ticks;
    }

private:
    Time() {}
    static uint32_t _ticks;
};

template <const TimeResolution ResolutionId>
uint32_t Time<ResolutionId>::_ticks = 0;

/** Captures the time ticks (specialized).
 *  \return Returns delta-time in microseconds.
 */
template <>
uint32_t Time<TimeResolution::Microseconds>::Update()
{
    uint32_t previous = _ticks;
    _ticks = TimerCounter0::getMicroseconds();
    return _ticks - previous;
}

/** Captures the time ticks (specialized).
 *  \return Returns delta-time in milliseconds.
 */
template <>
uint32_t Time<TimeResolution::Milliseconds>::Update()
{
    uint32_t previous = _ticks;
    _ticks = TimerCounter0::getMilliseconds();
    return _ticks - previous;
}
