#pragma once
#include "IdentifiableObject.h"
#include "../DigitalOutputPin.h"

template <const PortPins PortPinId>
class ToggleOutputPinTaskBase : public IdentifiableObject<DigitalOutputPin<PortPinId>>
{
    typedef IdentifiableObject<DigitalOutputPin<PortPinId>> BaseT;

protected:
    // called by the TimeoutTask
    void OnTimeout()
    {
        BaseT::Toggle();
    }
};