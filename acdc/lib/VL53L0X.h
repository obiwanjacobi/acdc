#pragma once
#include <stdint.h>

template <class I2cT, const uint8_t Address>
class VL53L0X : public I2cT
{
    static_assert((Address & 0x80) == 0, "PCA9685 Address highest bit (7) must be cleared. It is not used in I2C.");

public:
    const uint8_t DefaultAddress = 0x29;

    bool Open()
    {
        VL53L0X_DataInit return false;
    }

private:
};