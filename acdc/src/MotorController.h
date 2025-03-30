#pragma once
#include <stdint.h>

enum class Direction
{
    Forward,
    Backward
};

template <class BaseT>
class MotorController : public BaseT
{
public:
    MotorController()
        : _dir(Direction::Forward)
    {
    }

    void setPower(bool on)
    {
        if (!on)
            BaseT::Stop();
    }

    void setDirection(Direction direction)
    {
        _dir = direction;
    }

    void setSpeed(uint8_t speed)
    {
        if (_dir == Direction::Forward)
            BaseT::CounterClockwise(speed);
        else
            BaseT::Clockwise(speed);
    }

    void Stop()
    {
        BaseT::Stop();
    }

    uint16_t getCurrent()
    {
        return 0;
    }

private:
    Direction _dir;
};