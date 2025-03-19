#pragma once
#include <stdint.h>

/** The Delays class is used count-down timer values typically used in Tasks.
 *  Delays is a static class and cannot be instantiated.
 *  \tparam TimeT is the Time class to use - indicating time in Milliseconds or Microseconds.
 *  TimeT implements: `uint16_t Update()` - returns the delta-Time.
 *  \tparam MaxItems is the maximum number of delays that can be tracked.
 *  \tparam TimeUnitT is the data type of the delay time.
 *
 *  For each item (times MaxItems) the size of uint16_t and TimeUnitT is pre-allocated (default 4 bytes per item).
 */
template <class TimeT, const uint8_t MaxItems, typename TimeUnitT = uint32_t>
class Delays : public TimeT
{
public:
    /** Calls Time::Update and returns the delta time.
     *  \return Returns the delta time in units indicated by how Time was constructed.
     */
    static uint32_t Update()
    {
        _delta = TimeT::Update();
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

    /** Returns the delay time for the specified id.
     *  \param id is the identification of the delay.
     *  \return Returns the delay time in units the TimeT was constructed with.
     */
    static TimeUnitT getDelayValue(uint16_t id)
    {
        for (int i = 0; i < MaxItems; i++)
        {
            if (_ids[i] == id)
            {
                return _delays[i];
            }
        }

        return InvalidId;
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
     *  You must call Update to update to a new delta time which is used to count down the delays.
     *  When the id is not listed it is added with the specified time.
     *  If it is listed it's delay is counted down.
     *  \param id is the identification of the delay - cannot be zero (InvalidId).
     *  \param time is the delay time in units the TimeT was constructed with.
     *  \return Returns true to indicate the delay has reached zero.
     */
    static bool Wait(uint16_t id, TimeUnitT time)
    {
        if (id == InvalidId)
            return false;

        int emptyIndex = -1;

        for (int i = 0; i < MaxItems; i++)
        {
            if (emptyIndex == -1 && _ids[i] == InvalidId)
            {
                emptyIndex = i;
                continue;
            }

            if (_ids[i] == id)
            {
                if (_delta >= _delays[i])
                {
                    _ids[i] = InvalidId;
                    return true;
                }

                _delays[i] -= _delta;
                return false;
            }
        }

        if (emptyIndex != -1)
        {
            _ids[emptyIndex] = id;
            _delays[emptyIndex] = time;
        }

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
                _ids[i] = InvalidId;
            }
        }
    }

    /** The invalid id value.
     */
    static uint16_t InvalidId;

    static TimeUnitT getDelta()
    {
        return _delta;
    }

private:
    static TimeUnitT _delta;
    static uint16_t _ids[MaxItems];
    static TimeUnitT _delays[MaxItems];

    Delays() {}
};

template <class TimeT, const uint8_t MaxItems, typename TimeUnitT>
uint16_t Delays<TimeT, MaxItems, TimeUnitT>::InvalidId = 0;

template <class TimeT, const uint8_t MaxItems, typename TimeUnitT>
TimeUnitT Delays<TimeT, MaxItems, TimeUnitT>::_delta;

template <class TimeT, const uint8_t MaxItems, typename TimeUnitT>
uint16_t Delays<TimeT, MaxItems, TimeUnitT>::_ids[] = {};

template <class TimeT, const uint8_t MaxItems, typename TimeUnitT>
TimeUnitT Delays<TimeT, MaxItems, TimeUnitT>::_delays[] = {};
