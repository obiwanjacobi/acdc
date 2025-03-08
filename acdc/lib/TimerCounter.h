#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include "atl/LockScope.h"
#include "TimeResolution.h"
#include "PowerReduction.h"

#ifdef TCNT0

// This class is a quick implementation taken from Arduino. Uses Timer0.
class TimerCounter0
{
// FROM ARDUINO
// Copyright (c) 2005-2006 David A. Mellis

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define Microseconds_Per_Timer0_Overflow ForClockCycles<TimeResolution::Microseconds>(64 * 256)

// the whole number of milliseconds per timer0 overflow
#define Millis_Inc (Microseconds_Per_Timer0_Overflow / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define Fract_Inc ((Microseconds_Per_Timer0_Overflow % 1000) >> 3)
#define Fract_Max (1000 >> 3)

public:
    static void Start()
    {
        PowerReduction::Timer0(PowerState::On);

        // set normal timer mode
        TCCR0A = 0;
        // clear counter register
        TCNT0 = 0;
        // prescaler to 64
        TCCR0B = (1 << CS00) | (1 << CS01);
        // clear any pending overflow interrupts
        TIFR0 = (1 << TOV0);
        // enable overflow interrupt
        TIMSK0 = (1 << TOIE0);
    }

    template <TimeResolution TimeResolution>
    static uint32_t getTime();

    static uint32_t getMilliseconds()
    {
        LockScope lock;
        return _milliCount;
    }

    static uint32_t getMicroseconds()
    {
        uint32_t micros;
        uint8_t count;

        {
            LockScope lock;

            micros = _overflowCount;
            count = TCNT0;

            if ((TIFR0 & (1 << TOV0)) && (count < 255))
                micros++;
        }

        return ((micros << 8) + count) * (64 / (F_CPU / 1000000L));
    }

    // Call from ISR(TIMER0_OVF_vect)
    static void OnTimerOverflowInterrupt()
    {
        // copy these to local variables so they can be stored in registers
        // (volatile variables must be read from memory on every access)
        uint32_t millis = _milliCount;
        uint32_t fract = _fractureCount;

        millis += Millis_Inc;
        fract += Fract_Inc;
        if (fract >= Fract_Max)
        {
            fract -= Fract_Max;
            millis += 1;
        }

        _milliCount = millis;
        _fractureCount = (uint8_t)fract;
        _overflowCount++;
    }

private:
    TimerCounter0() {}
    static volatile uint32_t _milliCount;
    static volatile uint32_t _overflowCount;
    static uint8_t _fractureCount; // not volatile, only accessed from within ISR
};

template <>
uint32_t TimerCounter0::getTime<TimeResolution::Microseconds>()
{
    return getMicroseconds();
}

template <>
uint32_t TimerCounter0::getTime<TimeResolution::Milliseconds>()
{
    return getMilliseconds();
}

volatile uint32_t TimerCounter0::_milliCount;
volatile uint32_t TimerCounter0::_overflowCount;
uint8_t TimerCounter0::_fractureCount;

ISR(TIMER0_OVF_vect)
{
    TimerCounter0::OnTimerOverflowInterrupt();
}

#endif // TCNT0
