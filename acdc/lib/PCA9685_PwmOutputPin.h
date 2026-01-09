#pragma once
#include "../lib/atl/AtlMath.h"
#include "Port.h"
#include "PCA9685.h"

template <class PCA9685T, const PCA9685_Pins PwmPinId>
class PCA9685_PwmOutputPin
{
public:
    void Write(uint8_t dutyCycle)
    {
        uint16_t value = Math::ScaleLinear<uint8_t, uint16_t>(0, 255, 0, 4095, dutyCycle);
        PCA9685T::Write(PwmPinId, value);
    }
};