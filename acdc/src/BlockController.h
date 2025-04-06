#pragma once
#include <stdint.h>

template <class MotorControllerT, class CurrentSensorT>
class BlockController : public MotorControllerT
{
public:
    bool Open()
    {
        return CurrentSensorT::Open(7500);
    }

    bool TryReadOccupied(int16_t *val)
    {
        return CurrentSensorT::TryReadShuntVoltage(val);
    }

    bool IsOccupied(int16_t threshold = 500)
    {
        int16_t val = 0;
        if (TryReadOccupied(&val) && _lastShuntV != val)
        {
            _lastShuntV = val;
        }

        return Math::Abs(_lastShuntV) > threshold;
    }

private:
    int16_t _lastShuntV;
};