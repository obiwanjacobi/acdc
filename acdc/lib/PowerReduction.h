#pragma once
#include <avr/io.h>

enum class PowerState
{
    Off,
    On
};

class PowerReduction
{
public:
#ifdef PRR0
    static void Timer1(PowerState state)
    {
        Set(PRR0, PRTIM1, state);
    }
#endif // PRR0

#ifdef PRR
    static void Timer0(PowerState state)
    {
        Set(PRR, PRTIM0, state);
    }
    static void Timer1(PowerState state)
    {
        Set(PRR, PRTIM1, state);
    }
    static void Timer2(PowerState state)
    {
        Set(PRR, PRTIM2, state);
    }
#endif // PRR

#ifdef PRR1
    static void Timer3(PowerState state)
    {
        Set(PRR1, PRTIM3, state);
    }

    static void Timer4(PowerState state)
    {
        Set(PRR1, PRTIM4, state);
    }

    static void Timer5(PowerState state)
    {
        Set(PRR1, PRTIM5, state);
    }
#endif // PRR1

private:
    PowerReduction() {};
    static void Set(volatile uint8_t &reg, uint8_t bit, PowerState value)
    {
        if (value == PowerState::Off)
        {
            reg |= (1 << bit);
        }
        else
        {
            reg &= ~(1 << bit);
        }
    }
};
