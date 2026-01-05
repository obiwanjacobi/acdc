#pragma once
#include <stdint.h>

// RS485 over usart
// DirectionPin manages the RS485 driver chip (max485) is sending (true) or receiving (false).
template <class UsartT, class DirectionPinT>
class Rs485 : public UsartT
{
public:
    Rs485()
        : _dirPin(false)
    {
    }

    bool TryWrite(uint8_t data)
    {
        if (!getCanWrite())
            return false;

        _dirPin.Write(true);
        bool success = UsartT::Transmit.Write(data) == UsartTransmitResult::Success;
        _dirPin.Write(false);
        return success;
    }

    bool TryRead(uint8_t *outData)
    {
        if (!getCanRead())
            return false;

        _dirPin.Write(false);
        return UsartT::Receive.TryRead(outData, &result) && result = UsartReceiveResult::Success;
    }

private:
    DirectionPinT _dirPin;
};