#pragma once
#include <stdint.h>
#include "../lib/atl/IdentifiableObject.h"
#include "../lib/atl/Task.h"

template <class DelaysT, class PwmOutputPinT, const uint32_t Timeout>
class PwmTask
{
public:
    /** Call this method repeatedly from the main loop.
     */
    Task_BeginParams(Run, PwmOutputPinT *pwmOutputPin)
    {
        Task_YieldUntil(DelaysT::Delay(getId(), Timeout));
        pwmOutputPin->Write(_pwm);
        _pwm += 10;
    }
    Task_End;

    uint16_t getId() const
    {
        return (uint16_t)this;
    }

private:
    uint8_t _pwm;
    uint8_t _task;
};
