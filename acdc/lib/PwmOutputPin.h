#pragma once
#include "PwmTimer.h"

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
    }

    void Attach(PwmTimerT *pwmTimer)
    {
        _pwmTimer = pwmTimer;
        _channel = _pwmTimer->PortPinToChannel(PortPinId);
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