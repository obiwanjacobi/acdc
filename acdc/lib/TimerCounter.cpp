#include "TimerCounter.h"

#ifdef TCCR1A

volatile uint32_t TimerCounter::_milliCount;
volatile uint32_t TimerCounter::_overflowCount;
uint8_t TimerCounter::_fractureCount;

ISR(TIMER0_OVF_vect)
{
    TimerCounter::OnTimerOverflowInterrupt();
}

#endif // TCCR1A