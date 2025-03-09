#pragma once
#include "PowerReduction.h"
#include <avr/io.h>
#include "Port.h"

#ifdef OCR2A

/*
 * Pwm timer for port-pins B3 and D3.
 */
class PwmTimer2
{
public:
    enum class Channel : uint8_t
    {
        None = 0,
        A = 0xB3,
        B = 0xB4,
    };

    PwmTimer2()
    {
        init();
        PowerReduction::Timer2(PowerState::On);
    }

    void Enable(Channel channel, bool enable = true)
    {
        if (channel == Channel::None)
            return;

        // TODO: optimize this code
        if (enable)
        {
            if (channel == Channel::A && !(TCCR2A & (1 << COM2A1)))
            {
                TCCR2A |= (1 << COM2A1); // Non-inverting mode
            }
            else if (channel == Channel::B && !(TCCR2A & (1 << COM2B1)))
            {
                TCCR2A |= (1 << COM2B1); // Non-inverting mode
            }
        }
        else
        {
            if (channel == Channel::A)
            {
                TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0)); // Disconnect OC2A
                // Set the pin low manually
                PORTB &= ~(1 << PORTB3);
            }
            else if (channel == Channel::B)
            {
                TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0)); // Disconnect OC2B
                // Set the pin low manually
                PORTD &= ~(1 << PORTD3);
            }
        }
    }

    void SetOutputCompareValue(Channel channel, uint8_t value)
    {
        if (channel == Channel::None)
            return;

        // special case for turning off the PWM (value = 0)
        if (value == 0)
        {
            Enable(channel, false);
        }
        else
        {
            Enable(channel, true);
            _SFR_MEM8(channel) = value;
        }
    }

    // clang-format off
    constexpr Channel PortPinToChannel(PortPins portPin) const
    {
        return (portPin == PortPins::B3) 
            ? Channel::A : (portPin == PortPins::D3)
                ? Channel::B : Channel::None;
    }

    constexpr PortPins ChannelToPortPin(Channel channel) const
    {
        return (channel == Channel::A) 
            ? PortPins::B3 : (channel == Channel::B) 
                ? PortPins::D3 : PortPins::None;
    }
    // clang-format on

private:
    void init()
    {
        // Set Fast PWM mode with ICR3 as top
        TCCR2A = (1 << WGM20) | (1 << WGM21);
        TCCR2B = (1 << CS22); // Prescaler 64
        // TCCR2B = (1 << CS21); // Prescaler 8

        // Set non-inverting mode
        TCCR2A |= (1 << COM2A1) | (1 << COM2B1);

        // Set initial duty cycle to 0
        OCR2A = 0;
        OCR2B = 0;
    }
};

#endif // OCR2A

#ifdef OCR3A

/*
 * Pwm timer for port-pins E3, E4, and E5.
 */
class PwmTimer3
{
public:
    enum class Channel : uint8_t
    {
        None = 0,
        A = 0x98,
        B = 0x9A,
        C = 0x9C
    };

    PwmTimer3()
    {
        init();
        PowerReduction::Timer3(PowerState::On);
    }

    void SetOutputCompareValue(Channel channel, uint8_t value)
    {
        if (channel != Channel::None)
            _SFR_MEM16(channel) = (uint16_t)value;
    }

    Channel PortPinToChannel(PortPins portPin)
    {
        switch (portPin)
        {
        case PortPins::E3:
            return Channel::A;
        case PortPins::E4:
            return Channel::B;
        case PortPins::E5:
            return Channel::C;
        default:
            return Channel::None;
        }
    }

    PortPins ChannelToPortPin(Channel channel)
    {
        switch (channel)
        {
        case Channel::A:
            return PortPins::E3;
        case Channel::B:
            return PortPins::E4;
        case Channel::C:
            return PortPins::E5;
        default:
            return PortPins::None;
        }
    }

private:
    void init()
    {
        // Set Fast PWM mode with ICR3 as top
        TCCR3A = (1 << WGM31);
        TCCR3B = (1 << WGM33) | (1 << WGM32) |
                 (1 << CS31) | (1 << CS30); // Prescaler 64

        // Set non-inverting mode
        TCCR3A |= (1 << COM3A1) | (1 << COM3B1) | (1 << COM3C1);

        // Set ICR3 to define TOP value for PWM frequency
        ICR3 = 0xFFFF;

        // Set initial duty cycle to 0
        OCR3A = 0;
        OCR3B = 0;
        OCR3C = 0;
    }
};

#endif // OCR3C