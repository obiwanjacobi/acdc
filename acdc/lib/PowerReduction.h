#pragma once
#include <avr/io.h>
#include "Bit.h"

enum class PowerState
{
    Off,
    On
};

class PowerReduction
{
public:
#ifdef PRR
#ifdef PRUSART0
    static void Usart0(PowerState state)
    {
        Set(PRR, PRUSART0, state);
    }
#endif
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
    static void Twi(PowerState state)
    {
        Set(PRR, PRTWI, state);
    }
#endif // PRR

#ifdef PRR0
#ifdef PRUSART0
    static void Usart0(PowerState state)
    {
        Set(PRR0, PRUSART0, state);
    }
#ifdef PRUSART1
    template <>
    static void Usart1(PowerState state)
    {
        Set(PRR0, PRUSART1, state);
    }
#endif
#endif

    static void Timer1(PowerState state)
    {
        Set(PRR0, PRTIM1, state);
    }
#endif // PRR0

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
        BitFlag::Set(reg, bit, value == PowerState::Off);
    }
};
