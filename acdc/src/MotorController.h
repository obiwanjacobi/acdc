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

    Direction getDirection() const
    {
        return _dir;
    }

    void setSpeed(uint8_t speed)
    {
        if (_dir == Direction::Forward)
            BaseT::CounterClockwise(speed);
        else
            BaseT::Clockwise(speed);
    }

private:
    Direction _dir;
};