#pragma once
#include <stdint.h>
#include "../lib/atl/Debug.h"
#include "../lib/PwmOutputPin.h"
#include "../lib/DigitalOutputPin.h"
#include "../lib/AnalogInputPin.h"
#include "../lib/Port.h"
#include "../lib/PwmTimer.h"
#include "MotorController.h"

extern Serial serial;

// Arduino motor shield Rev3

// Channel A
// D3 pwm
// B4 dir
// B1 break
// C0 sense

// Channel B
// B3 pwm
// B5 dir
// B0 break
// C1 sense

typedef PwmTimer2 PwmTimerT;
PwmTimerT pwmTimer;
PwmOutputPin<PwmTimerT, PortPins::D3> pwmOutputPin1(&pwmTimer);
PwmOutputPin<PwmTimerT, PortPins::B3> pwmOutputPin2(&pwmTimer);
MotorController<PwmOutputPin<PwmTimerT, PortPins::D3>, DigitalOutputPin<PortPins::B4>, DigitalOutputPin<PortPins::B1>, AnalogInputPin<PortPins::C0>> motorController1(&pwmOutputPin1);
MotorController<PwmOutputPin<PwmTimerT, PortPins::B3>, DigitalOutputPin<PortPins::B5>, DigitalOutputPin<PortPins::B0>, AnalogInputPin<PortPins::C1>> motorController2(&pwmOutputPin2);

class CommandHandler
{
public:
    void OnPower(bool on)
    {
        motorController1.PowerOn(on);
        motorController2.PowerOn(on);
    }
    void OnSpeed(uint8_t speed)
    {
        // const uint8_t minSpeed = 30;
        const uint8_t maxSpeed = 255;

        uint8_t actual = (maxSpeed * speed) / 10;

        motorController1.Speed(actual);
        motorController2.Speed(actual);
    }
    void OnDirection(bool forward)
    {
        motorController1.Direction(forward ? Direction::Forward : Direction::Backward);
        motorController2.Direction(forward ? Direction::Forward : Direction::Backward);
    }
};
