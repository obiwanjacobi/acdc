#pragma once
#include <stdint.h>

#include "MotorController.h"
#include "Serial.h"
#include "../lib/Port.h"

extern Serial serial;

template <class MotorControllerT, PortPins TriggerPin>
class OptoBlockController : public MotorControllerT
{
    typedef MotorControllerT BaseT;

public:
    bool Open()
    {
        // nothing to do here
        return true;
    }

    bool IsOccupied()
    {
        return !_sensor.Read();
    }

private:
    DigitalInputPin<TriggerPin> _sensor;
};