#pragma once
#include <stdint.h>
#include "../lib/atl/Collection.h"
#include "../lib/atl/FixedArray.h"
#include "../lib/atl/Task.h"
#include "OptoBlockController.h"
#include "CurrentBlockController.h"
#include "hardware.h"
#include "Serial.h"

extern Serial serial;

template <class BlockControllerT>
class Block : public BlockControllerT
{
public:
    bool TryReadOccupied()
    {
        // int16_t val = 0;
        // if (BlockT::TryReadOccupied(&val))
        // {
        // occupiedValues.Add(Math::Abs(val));

        // if (occupiedValues.getCount() == occupiedValues.getCapacity())
        // {
        //     int16_t value = GetOccupiedValue(200);
        //     occupiedValues.Clear();

        //     if (_occupied != value)
        //     {
        //         _occupied = value;
        //         return true;
        //     }
        // }
        // }

        // bool on = BlockControllerT::IsOccupied(150, 75);
        bool on = BlockControllerT::IsOccupied();
        if (_occupied != on)
        {
            _occupied = on;
            return true;
        }

        return false;
    }

    bool getOccupied() const
    {
        return _occupied;
    }

    // void Dump()
    // {
    //     for (int16_t i = 0; i < occupiedValues.getCount(); i++)
    //     {
    //         if (i > 0)
    //             serial.Transmit.Write(", ");
    //         serial.Transmit.Write(occupiedValues[i]);
    //     }

    //     serial.Transmit.WriteLine();
    // }

private:
    bool _occupied;

#define ValueCount 6

    // Collection<FixedArray<int16_t, ValueCount>> occupiedValues;

    // bool GetOccupiedValue(int16_t threshold) const
    // {
    //     int16_t count = occupiedValues.getCount();
    //     int16_t overThresholdCount = 0;

    //     for (int16_t i = 0; i < count; i++)
    //     {
    //         if (occupiedValues[i] > threshold)
    //             overThresholdCount++;
    //     }

    //     return overThresholdCount >= count / 2;
    // }
};

typedef Block<CurrentBlockController<MotorControllerT_0, Ina219T_0>> BlockControllerT_0;
typedef Block<CurrentBlockController<MotorControllerT_1, Ina219T_1>> BlockControllerT_1;
typedef Block<CurrentBlockController<MotorControllerT_2, Ina219T_2>> BlockControllerT_2;
typedef Block<CurrentBlockController<MotorControllerT_3, Ina219T_3>> BlockControllerT_3;

// typedef Block<OptoBlockController<MotorControllerT_0, PortPins::C0>> BlockControllerT_0;
// typedef Block<OptoBlockController<MotorControllerT_1, PortPins::C1>> BlockControllerT_1;
// typedef Block<OptoBlockController<MotorControllerT_2, PortPins::C2>> BlockControllerT_2;
// typedef Block<OptoBlockController<MotorControllerT_3, PortPins::C3>> BlockControllerT_3;
