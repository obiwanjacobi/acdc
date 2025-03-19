#pragma once

template <class PwmPinT, class DirectionPinT>
class MotorController
{
public:
    void PowerOn(bool on)
    {
        _pwmPin.Enable(on);
    }
    void Direction(bool forward)
    {
        _directionPin.Write(forward);
    }
    void Speed(uint8_t speed)
    {
        _pwmPin.Write(speed);
    }

private:
    PwmPinT _pwmPin;
    DirectionPinT _directionPin;
};
