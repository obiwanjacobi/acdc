#pragma once
#include <stdint.h>

/** The Delays class is used count-down timer values typically used in Tasks.
 *  Delays is a static class and cannot be instantiated.
 *  \tparam TimeT is the Time class to use - indicating time in Milliseconds or Microseconds.
 *  TimeT implements: `uint16_t Update()` - returns the delta-Time.
 *  \tparam MaxItems is the maximum number of delays that can be tracked.
 */
template <class TimeT, const uint8_t MaxItems>
class Delays
{
public:
    /** Calls Time::Update and returns the delta time.
     *  \return Returns the delta time in units indicated by how Time was constructed.
     */
    static uint32_t Update()
    {
        _delta = _time.Update();
        return _delta;
    }

    /** Indicates if the id is listed.
     *  \param id is the identification of the delay.
     *  \return Returns true if the id is listed.
     */
    static bool IsWaiting(uint16_t id)
    {
        for (int i = 0; i < MaxItems; i++)
        {
            if (_ids[i] == id)
            {
                return true;
            }
        }

        return false;
    }

    /** Zero's out the delay time but keeps the id in the list.
     *  The next call to Wait will report done.
     *  \param id is the identification of the delay.
     */
    static void Abort(uint16_t id)
    {
        for (int i = 0; i < MaxItems; i++)
        {
            if (_ids[i] == id)
            {
                _delays[i] = 0;
            }
        }
    }

    /** Can be called repeatedly and will count-down the specified time.
     *  You must call Update to update to a new delta time.
     *  When the id is not listed it is added with the specified time.
     *  If it is listed it's delay is counted down.
     *  \param id is the identification of the delay.
     *  \param time is the delay time in units the TimeT was constructed with.
     *  \return Returns true to indicate the delay has reached zero.
     */
    static bool Wait(uint16_t id, uint32_t time)
    {
        int emptyIndex = -1;

        for (int i = 0; i < MaxItems; i++)
        {
            if (emptyIndex == -1 && _ids[i] == 0)
            {
                emptyIndex = i;
                continue;
            }

            if (_ids[i] == id)
            {
                if (_delta >= _delays[i])
                {
                    _ids[i] = 0;
                    return true;
                }

                _delays[i] -= _delta;
                return false;
            }
        }

        if (emptyIndex == -1)
        {
            for (int i = 0; i < MaxItems; i++)
            {
                if (_ids[i] == 0)
                {
                    emptyIndex = i;

                    break;
                }
            }
        }

        _ids[emptyIndex] = id;
        _delays[emptyIndex] = time;

        return false;
    }

    /** Removes the id from the listing.
     *  \param id is the identification of the delay.
     */
    static void Clear(uint16_t id)
    {
        for (int i = 0; i < MaxItems; i++)
        {
            if (_ids[i] == id)
            {
                _ids[i] = 0;
            }
        }
    }

    /** Returns the Time reference used by this class.
     *  \return Returns a reference to Time.
     */
    static TimeT &getTime()
    {
        return _time;
    }

    /** Retrieves the detla-time after `Update()` was called.
     *  \return Returns the delta time in units the Time was constructed with.
     */
    static uint16_t getLastDeltaTime()
    {
        return _delta;
    }

private:
    static TimeT _time;
    static uint32_t _delta;
    static uint16_t _ids[MaxItems];
    static uint32_t _delays[MaxItems];

    Delays() {}
};

template <class TimeT, const uint8_t MaxItems>
TimeT Delays<TimeT, MaxItems>::_time;

template <class TimeT, const uint8_t MaxItems>
uint32_t Delays<TimeT, MaxItems>::_delta = 0;

template <class TimeT, const uint8_t MaxItems>
uint16_t Delays<TimeT, MaxItems>::_ids[] = {};

template <class TimeT, const uint8_t MaxItems>
uint32_t Delays<TimeT, MaxItems>::_delays[] = {};