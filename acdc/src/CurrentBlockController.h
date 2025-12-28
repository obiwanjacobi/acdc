#pragma once
#include <stdint.h>

#include "MotorController.h"

extern Serial serial;

template <class MotorControllerT, class CurrentSensorT>
class CurrentBlockController : public MotorControllerT
{
public:
    bool Open()
    {
        // clang-format off
        return CurrentSensorT::Open(
            0,
            CurrentSensorT::Mode::ShuntAndBusContinuous,
            CurrentSensorT::Sensitivity::mV320,
            CurrentSensorT::AdcMode::Average8,
            CurrentSensorT::AdcMode::Average2,
            CurrentSensorT::BusVoltage::Volt16
        );
        // clang-format on
    }

    // bool TryReadOccupied(int16_t *val)
    // {
    //     return CurrentSensorT::TryReadShuntVoltage(val);
    // }

    bool IsOccupied(uint16_t threshold = 200, uint16_t delta = 100)
    {
        // int16_t bus = 0;
        int16_t shunt = 0;
        // Direction dir = MotorControllerT::getDirection();
        if ( // CurrentSensorT::TryReadBusVoltage(&bus) &&
            CurrentSensorT::TryReadShuntVoltage(&shunt))
        {
            int16_t shuntDelta = shunt - _lastShuntV;

            if ( // bus > threshold &&
                 //  ((dir == Direction::Forward && shunt < 0) ||
                 //   (dir == Direction::Backward && shunt > 0)) &&
                Math::Abs(shuntDelta) > (int16_t)delta)
            {
                _lastShuntV = shunt;

                serial.Transmit.Write(Math::Abs(_lastShuntV) > (int16_t)threshold ? "Y" : "N");
                serial.Transmit.Write(" ");
                serial.Transmit.Write(CurrentSensorT::getAddress());
                serial.Transmit.Write(" ");
                serial.Transmit.Write(_lastShuntV);

                // serial.Transmit.Write(" (");
                // serial.Transmit.Write(bus);
                // serial.Transmit.Write(") ");
                // serial.Transmit.Write(shuntDelta);

                serial.Transmit.WriteLine();
            }
        }

        return Math::Abs(_lastShuntV) > (int16_t)threshold;
    }

private:
    int16_t _lastShuntV;
};