#pragma once
#include <stdint.h>

enum class Direction
{
    Forward,
    Backward
};

template <class PwmPinT, class DirectionPinT, class BreakPinT, class SensePinT>
class MotorController
{
public:
    // only the pwm pin is passed in because it takes a reference to a pwm-timer,
    // which is constructed outside this class.
    MotorController(PwmPinT *pwmPin)
        : _pwmPin(pwmPin)
    {
    }

    void PowerOn(bool on)
    {
        _pwmPin->Write(0);
        _breakPin.Write(!on);
        _directionPin.Write(false);
    }
    void Direction(Direction direction)
    {
        _directionPin.Write(direction == Direction::Forward);
    }
    void Speed(uint8_t speed)
    {
        _breakPin.Write(speed == 0);
        _pwmPin->Write(speed);
    }
    void Stop()
    {
        _pwmPin->Write(0);
        _breakPin.Write(true);
    }

    uint16_t getCurrent()
    {
        return _sensePin.Read();
    }

private:
    PwmPinT *_pwmPin;
    DirectionPinT _directionPin;
    BreakPinT _breakPin;
    SensePinT _sensePin;
};
