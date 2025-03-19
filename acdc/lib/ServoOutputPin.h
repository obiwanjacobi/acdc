#pragma once
#include <stdint.h>
#include "Port.h"

template <class ServoTimerT, const PortPins PortPinId>
class Servo180OutputPin
{
public:
    Servo180OutputPin(ServoTimerT *pwmTimer)
        : _servoTimer(pwmTimer)
    {
        PortPin<PortPinId>::SetDirection(Output);
        SetAngle(0);
    }

    // Sets angle from 0-180 degrees
    void SetAngle(uint8_t angle)
    {
        typename ServoTimerT::Channel channel = _servoTimer->PortPinToChannel(PortPinId);
        _servoTimer->SetAngle(channel, angle);
    }

private:
    ServoTimerT *_servoTimer;
};

// ----------------------------------------------------------------------------

template <class ServoTimerT, const PortPins PortPinId>
class Servo360OutputPin
{
public:
    Servo360OutputPin(ServoTimerT *pwmTimer)
        : _servoTimer(pwmTimer)
    {
        PortPin<PortPinId>::SetDirection(Output);
        SetSpeed(0);
    }

    // speed range: -100 to +100
    // 0 = stop, negative = one direction, positive = other direction
    void SetSpeed(int8_t speed)
    {
        typename ServoTimerT::Channel channel = _servoTimer->PortPinToChannel(PortPinId);
        _servoTimer->SetSpeed(channel, speed);
    }

private:
    ServoTimerT *_servoTimer;
};