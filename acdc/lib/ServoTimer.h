#pragma once
#include <stdint.h>
#include <avr/io.h>
#include "LockScope.h"
#include "PowerReduction.h"
#include "Port.h"

#ifdef OCR0A

/*
 * Servo timer for port-pins D6 (OC0A) and D5 (OC0B).
 * Configured for standard RC servo control (50Hz PWM with 1-2ms pulses).
 *
 * For SG90 servos:
 * - Value 10-12 = 0° position (1ms pulse)
 * - Value 18-20 = 90° position (1.5ms pulse)
 * - Value 25-30 = 180° position (2ms pulse)
 */
class ServoTimer0
{
public:
    enum class Channel : uint8_t
    {
        None = 0,
        A = 0x47, // Address of OCR0A
        B = 0x48, // Address of OCR0B
    };

    ServoTimer0()
    {
        init();
        PowerReduction::Timer0(PowerState::On);
    }

    void SetOutputCompareValue(Channel channel, uint8_t value)
    {
        if (channel == Channel::None)
            return;

        // Clamp value to valid servo range (approx 1-2ms pulse)
        if (value < 10)
            value = 10; // Min pulse (~1ms)
        if (value > 30)
            value = 30; // Max pulse (~2ms)

        Enable(channel, true);
        _SFR_MEM8(channel) = value;
    }

    void Enable(Channel channel, bool enable = true)
    {
        if (channel == Channel::None)
            return;

        if (enable)
        {
            if (channel == Channel::A && !(TCCR0A & (1 << COM0A1)))
            {
                TCCR0A |= (1 << COM0A1); // Non-inverting mode
            }
            else if (channel == Channel::B && !(TCCR0A & (1 << COM0B1)))
            {
                TCCR0A |= (1 << COM0B1); // Non-inverting mode
            }
        }
        else
        {
            if (channel == Channel::A)
            {
                TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0)); // Disconnect OC0A
                // Set the pin low manually
                PORTD &= ~(1 << PORTD6);
            }
            else if (channel == Channel::B)
            {
                TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0)); // Disconnect OC0B
                // Set the pin low manually
                PORTD &= ~(1 << PORTD5);
            }
        }
    }

    // clang-format off
    constexpr Channel PortPinToChannel(PortPins portPin) const
    {
        return (portPin == PortPins::D6) 
            ? Channel::A : (portPin == PortPins::D5)
                ? Channel::B : Channel::None;
    }

    constexpr PortPins ChannelToPortPin(Channel channel) const
    {
        return (channel == Channel::A) 
            ? PortPins::D6 : (channel == Channel::B) 
                ? PortPins::D5 : PortPins::None;
    }
    // clang-format on

private:
    void init()
    {
        // Use Phase Correct PWM mode (WGM00=1, WGM01=0) for better servo performance
        TCCR0A = (1 << WGM00);

        // Set largest prescaler (1024) to get close to 50Hz for servos
        // 16MHz/1024/256 = ~61Hz
        TCCR0B = (1 << CS02) | (1 << CS00);

        OCR0A = 0;
        OCR0B = 0;
    }
};

#endif // OCR0A

// ----------------------------------------------------------------------------

#ifdef OCR1A

/*
 * Servo timer using Timer1 (16-bit) for port-pins B1/9 (OC1A) and B2/10 (OC1B).
 * Configured for precise 50Hz PWM with 1-2ms pulses for standard servo control.
 *
 * For BOTH standard and continuous rotation servos:
 * - Value ~1000  = 0° position / full speed one direction (1ms pulse)
 * - Value ~3000  = 90° position / stop (1.5ms pulse)
 * - Value ~5000  = 180° position / full speed other direction (2ms pulse)
 */
class ServoTimer1
{
#define Neutral 3000
public:
    enum class Channel : uint16_t
    {
        None = 0,
        A = 0x88, // Address of OCR1A
        B = 0x8A, // Address of OCR1B
    };

    ServoTimer1()
    {
        init();
        PowerReduction::Timer1(PowerState::On);
    }

    void SetOutputCompareValue(Channel channel, uint16_t value)
    {
        if (channel == Channel::None)
            return;

        // Enable(channel, true);

        _SFR_MEM16(channel) = value;
    }

    // Maps an angle (0-180) to appropriate pulse width
    void SetAngle(Channel channel, uint8_t angle)
    {
        if (channel == Channel::None)
            return;

        if (angle > 180)
            angle = 180;

        // Map 0-180 degrees to 1000-2000 microseconds
        uint16_t pulse = 1000 + (uint16_t)(angle * 4000) / 180;

        SetOutputCompareValue(channel, pulse);
    }

    // Maps speed (-100 to +100) to appropriate pulse width
    void SetSpeed(Channel channel, int8_t speed)
    {
        if (channel == Channel::None)
            return;

        if (speed < -100)
            speed = -100;
        if (speed > 100)
            speed = 100;

        // Direct linear mapping from [-100,+100] to [1000,5000]
        // 3000 is the neutral point (Neutral)
        uint16_t pulse = Neutral + (speed * 20);

        Enable(channel, true);
        SetOutputCompareValue(channel, pulse);
    }

    void Enable(Channel channel, bool enable = true)
    {
        if (channel == Channel::None)
            return;

        if (enable)
        {
            if (channel == Channel::A && !(TCCR1A & (1 << COM1A1)))
            {
                TCCR1A |= (1 << COM1A1); // Non-inverting mode
            }
            else if (channel == Channel::B && !(TCCR1A & (1 << COM1B1)))
            {
                TCCR1A |= (1 << COM1B1); // Non-inverting mode
            }
        }
        else
        {
            if (channel == Channel::A)
            {
                TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0)); // Disconnect OC1A
                // Set the pin low manually
                PORTB &= ~(1 << PORTB1);
            }
            else if (channel == Channel::B)
            {
                TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0)); // Disconnect OC1B
                // Set the pin low manually
                PORTB &= ~(1 << PORTB2);
            }
        }
    }

    // clang-format off
    constexpr Channel PortPinToChannel(PortPins portPin) const
    {
        return (portPin == PortPins::B1) 
            ? Channel::A : (portPin == PortPins::B2)
                ? Channel::B : Channel::None;
    }

    constexpr PortPins ChannelToPortPin(Channel channel) const
    {
        return (channel == Channel::A) 
            ? PortPins::B1 : (channel == Channel::B) 
                ? PortPins::B2 : PortPins::None;
    }
    // clang-format on

private:
    void init()
    {
        // Configure Timer1 for servo control

        // Fast PWM mode with ICR1 as TOP (mode 14)
        // WGM13=1, WGM12=1, WGM11=1, WGM10=0
        TCCR1A = (1 << WGM11);
        TCCR1B = (1 << WGM13) | (1 << WGM12);

        // Set prescaler to 8 (CS11=1)
        TCCR1B |= (1 << CS11);

        // Set TOP value for exactly 50Hz
        // 16MHz / 8 prescaler / 50Hz = 40,000 counts
        ICR1 = 40000;

        // Initialize to neutral position (1.5ms pulse)
        OCR1A = Neutral; // ~1.5ms pulse (90° position / stop)
        OCR1B = Neutral; // ~1.5ms pulse (90° position / stop)
    }
};

#endif // OCR1A