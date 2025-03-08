#pragma once
#include "PwmTimer.h"

// NOTE: Writing dutycycle 0 to the PWM pin does not completely turn off the pin.

template <class PwmTimerT, const PortPins PortPinId>
class PwmOutputPin
{
public:
    PwmOutputPin()
        : _pwmTimer(nullptr)
    {
        PortPin<PortPinId>::SetDirection(PinIO::Output);
    }
    PwmOutputPin(PwmTimerT *pwmTimer)
        : _pwmTimer(pwmTimer)
    {
        _channel = _pwmTimer->PortPinToChannel(PortPinId);
        PortPin<PortPinId>::SetDirection(PinIO::Output);
        _pwmTimer->SetOutputCompareValue(_channel, 0);
    }

    void Attach(PwmTimerT *pwmTimer)
    {
        _pwmTimer = pwmTimer;
        _channel = _pwmTimer->PortPinToChannel(PortPinId);
        _pwmTimer->SetOutputCompareValue(_channel, 0);
    }

    void Write(uint8_t dutyCycle)
    {
        if (_pwmTimer)
            _pwmTimer->SetOutputCompareValue(_channel, dutyCycle);
    }

    PortPins getPortPin() const
    {
        return PortPinId;
    }

private:
    PwmTimerT *_pwmTimer;
    typename PwmTimerT::Channel _channel;
};