#pragma once
#include <stdint.h>
#include "../lib/atl/IdentifiableObject.h"
#include "../lib/atl/Task.h"

template <class DelaysT, class PwmOutputPinT, const uint16_t Timeout>
class PwmTask
{
public:
    /** Call this method repeatedly from the main loop.
     */
    Task_BeginParams(Run, PwmOutputPinT pwmOutputPin)
    {
        Task_YieldUntil(DelaysT::Wait(getId(), Timeout));
        pwmOutputPin.Write(_pwm);
        _pwm++;
    }
    Task_End;

private:
    uint16_t getId() const
    {
        return (uint16_t)this;
    }
    uint8_t _pwm;
    uint8_t _task;
};
