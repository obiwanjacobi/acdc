#pragma once
#include <stdint.h>

template <class MotorControllerT, class CurrentSensorT>
class BlockController : public MotorControllerT
{
public:
    bool Open()
    {
        // clang-format off
        return CurrentSensorT::Open(
            0,
            CurrentSensorT::Mode::ShuntAndBusContinuous,
            CurrentSensorT::Sensitivity::mV320,
            CurrentSensorT::AdcMode::Average32,
            CurrentSensorT::AdcMode::Average8,
            CurrentSensorT::BusVoltage::Volt16
        );
        // clang-format on
    }

    bool TryReadOccupied(int16_t *val)
    {
        return CurrentSensorT::TryReadShuntVoltage(val);
    }

    bool IsOccupied(uint16_t threshold = 200, uint16_t delta = 200)
    {
        int16_t bus = 0;
        int16_t shunt = 0;
        if (CurrentSensorT::TryReadBusVoltage(&bus) &&
            CurrentSensorT::TryReadShuntVoltage(&shunt))
        {
            if (Math::Abs(bus) > threshold &&
                Math::Abs(shunt - _lastShuntV) > delta)
            {
                serial.Transmit.Write(CurrentSensorT::getAddress());
                serial.Transmit.Write(" ");
                serial.Transmit.WriteLine(shunt);

                _lastShuntV = Math::Abs(shunt);
            }
        }

        return _lastShuntV > threshold;
    }

private:
    int16_t _lastShuntV;
};