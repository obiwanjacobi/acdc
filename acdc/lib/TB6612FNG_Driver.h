#pragma once
#include <stdint.h>

/** This driver class writes to one port of a Toshiba TB6612FNG - a dual 1A motor driver/controller.
 *  Use two instances of this class to drive both channels of the chip.
 *  The class implements the IO routines for direct connection onto an Arduino.
 *  \tparam In1_BoardPinNumber is the BoardPinNumber of an output that is connected to input 1 of the chip.
 *  \tparam In2_BoardPinNumber is the BoardPinNumber of an output that is connected to input 2 of the chip.
 *  \tparam Pwm_BoardPinNumber is the BoardPinNumber of an output for the PWM input that controls speed.
 *  The Pwm_BoardPinNumber must be a PWM (~) pin.
 */
template <class PwmOutputPinT, class In1OutputPinT, class In2OutputPinT>
class TB6612FNG_Driver
{
public:
    /** Sends the signals to the defined outputs.
     *  \param in1 is the state for in1.
     *  \param in2 is the state for in2.
     *  \param pwm is the value for the PWM.
     */
    void
    Send(bool in1, bool in2, uint8_t pwm)
    {
        _in1.Write(in1);
        _in2.Write(in2);
        _pwm.Write(pwm);
    }

private:
    In1OutputPinT _in1;
    In2OutputPinT _in2;
    PwmOutputPinT _pwm;
};