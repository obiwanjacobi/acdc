#pragma once
#include "PwmTimer.h"

template <class PwmTimer, const PortPins PortPinId>
class PwmOutputPin
{
public:
    PwmOutputPin(PwmTimer &pwmTimer)
        : _pwmTimer(pwmTimer)
    {
        _channel = _pwmTimer.PortPinToChannel(PortPinId);
        PortPin<PortPinId>::SetDirection(PinIO::Output);
    }

    void Write(uint8_t dutyCycle)
    {
        _pwmTimer.SetOutputCompareValue(_channel, dutyCycle);
    }

    PortPins getPortPin() const
    {
        return PortPinId;
    }

private:
    PwmTimer &_pwmTimer;
    typename PwmTimer::Channel _channel;
};