#pragma once
#include <stdint.h>
#include "../lib/atl/Debug.h"
#include "../lib/atl/Math.h"
#include "hardware.h"

class CommandHandler
{
public:
    void OnPower(bool on)
    {
        // motorController1.setPower(on);
        // motorController2.setPower(on);
        // motorController3.setPower(on);
        // motorController4.setPower(on);
    }
    // speed >= 0 && <= 9
    void OnSpeed(uint8_t speed)
    {
        uint8_t actual = Math::ScaleLinear<uint8_t, uint8_t>(0, 9, 0, 255, speed);

        // motorController1.setSpeed(actual);
        // motorController2.setSpeed(actual);
        // motorController3.setSpeed(actual);
        // motorController4.setSpeed(actual);
    }
    void OnDirection(bool forward)
    {
        Direction dir = forward ? Direction::Forward : Direction::Backward;
        // motorController1.setDirection(dir);
        // motorController2.setDirection(dir);
        // motorController3.setDirection(dir);
        // motorController4.setDirection(dir);
    }
};

#ifdef ARDUINO_MOTOR_SHIELD_REV3
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
#endif
