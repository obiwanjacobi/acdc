#pragma once
#include <stdint.h>
#include "TimeResolution.h"
#include "../TimerCounter.h"

/** The Time class keeps track of time ticks (either milli- or micro-seconds).
 *  \tparam TimeResolution indicates the units of time.
 */
template <const TimeResolution ResolutionId>
class Time
{
public:
    /** Constructs a new instance.
     */
    Time()
        : _ticks(0)
    {
        Update();
    }

    /** Captures the time ticks.
     *  \return Returns delta-time in 'resolution'
     */
    uint32_t Update();

    /** Returns the time ticks in milli-seconds.
     */
    uint32_t getMilliseconds() const
    {
        return getMilliseconds<ResolutionId>(_ticks);
    }

    /** Returns the time ticks in micro-seconds.
     */
    uint32_t getMicroseconds() const
    {
        return getMicroseconds<ResolutionId>(_ticks);
    }

    /** Returns the TimeResolution template parameter.
     */
    TimeResolution getResolution() const
    {
        return ResolutionId;
    }

protected:
    /** Returns the raw time ticks.
     */
    uint32_t getTicks() const
    {
        return _ticks;
    }

private:
    uint32_t _ticks;
};

// Time template specializations

/** Captures the time ticks (specialized).
 *  \return Returns delta-time in microseconds.
 */
template <>
inline uint32_t Time<TimeResolution::Microseconds>::Update()
{
    uint32_t previous = _ticks;
    _ticks = TimerCounter0::getMicroseconds();
    return _ticks - previous;
}

/** Captures the time ticks (specialized).
 *  \return Returns delta-time in milliseconds.
 */
template <>
inline uint32_t Time<TimeResolution::Milliseconds>::Update()
{
    uint32_t previous = _ticks;
    _ticks = TimerCounter0::getMilliseconds();
    return _ticks - previous;
}
