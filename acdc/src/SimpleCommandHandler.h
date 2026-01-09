#pragma once
#include <stdint.h>
#include "../lib/atl/Debug.h"
#include "../lib/atl/AtlMath.h"
#include <BitArray.h>

#include "Block.h"
#include "hardware.h"

extern Serial serial;

BlockControllerT_0 blockController0;
BlockControllerT_1 blockController1;
BlockControllerT_2 blockController2;
BlockControllerT_3 blockController3;

class SimpleCommandHandler
{
public:
    bool Open()
    {
        return blockController0.Open() &&
               blockController1.Open() &&
               blockController2.Open() &&
               blockController3.Open();
    }

    bool TryReadBlocks(uint8_t *outData)
    {
        if (blockController0.TryReadOccupied() || blockController1.TryReadOccupied() || blockController2.TryReadOccupied() || blockController3.TryReadOccupied())
        {
            BitArray<uint8_t> flags;
            flags.Set(0, blockController0.getOccupied());
            flags.Set(1, blockController1.getOccupied());
            flags.Set(2, blockController2.getOccupied());
            flags.Set(3, blockController3.getOccupied());

            *outData = flags;
            return true;
        }

        return false;
    }

    void OnPower(bool on)
    {
        blockController0.setPower(on);
        blockController1.setPower(on);
        blockController2.setPower(on);
        blockController3.setPower(on);
    }
    // speed >= 0 && <= 9
    void OnSpeed(uint8_t speed)
    {
        uint8_t actual = Math::ScaleLinear<uint8_t, uint8_t>(0, 9, 0, 255, speed);

        blockController0.setSpeed(actual);
        blockController1.setSpeed(actual);
        blockController2.setSpeed(actual);
        blockController3.setSpeed(actual);
    }
    void OnDirection(bool forward)
    {
        Direction dir = forward ? Direction::Forward : Direction::Backward;
        blockController0.setDirection(dir);
        blockController1.setDirection(dir);
        blockController2.setDirection(dir);
        blockController3.setDirection(dir);
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
