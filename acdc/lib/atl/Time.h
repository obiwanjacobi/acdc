#pragma once
#include <stdint.h>
#include "TimeResolution.h"
#include "../TimerCounter.h"

/** The Time class keeps track of time ticks (either milli- or micro-seconds).
 *  \tparam TimeResolution indicates the units of time.
 */
template <const TimeResolution resolution>
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
        return getMilliseconds<resolution>(_ticks);
    }

    /** Returns the time ticks in micro-seconds.
     */
    uint32_t getMicroseconds() const
    {
        return getMicroseconds<resolution>(_ticks);
    }

    /** Returns the TimeResolution template parameter.
     */
    TimeResolution getResolution() const
    {
        return resolution;
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

    _ticks = TimerCounter::getMicroseconds();

    return _ticks - previous;
}

/** Captures the time ticks (specialized).
 *  \return Returns delta-time in milliseconds.
 */
template <>
inline uint32_t Time<TimeResolution::Milliseconds>::Update()
{
    uint32_t previous = _ticks;

    _ticks = TimerCounter::getMilliseconds();

    return _ticks - previous;
}

// ---------------------------------------------------------------------------

/** The TimeEx class adds start-time and delta-time members to Time.
 *  \tparam TimeResolution indicates the time units.
 */
template <TimeResolution resolution = TimeResolution::Milliseconds>
class TimeEx : public Time<resolution>
{
public:
    /** Initializes a new instance.
     */
    TimeEx()
    {
        Reset();
    }

    /** Resets the start time.
     */
    void Reset()
    {
        _start = _previous = Time<resolution>::getTicks();
    }

    /** Updates the time ticks (Time<>) and delta time.
     *  \returns Returns the delta-time.
     */
    uint32_t Update()
    {
        _previous = Time<resolution>::getTicks();
        return Time<resolution>::Update();
    }

    /** Returns the start time in milli-seconds.
     */
    uint32_t getStartMilliseconds() const
    {
        return getMilliseconds<resolution>(_start);
    }

    /** Returns the start time in micro-seconds.
     */
    uint32_t getStartMicroseconds() const
    {
        return getMicroseconds<resolution>(_start);
    }

    /** Returns the delta-time in ticks.
     */
    uint16_t getDeltaTime() const
    {
        return (uint16_t)(Time<resolution>::getTicks() - _previous);
    }

private:
    uint32_t _start;
    uint32_t _previous;
};
