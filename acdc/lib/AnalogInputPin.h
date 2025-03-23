#pragma once
#include <stdint.h>
#include <avr/io.h>
// #include <util/delay.h>
#include "Port.h"

// TODO: The ADC can be used for other puproses than just reading analog values.
// This code should be refactored into a dedicated ADC class to be more generic.

template <const PortPins PortPinId>
class AnalogInputPin
{
    static_assert(PortPinId >= PortPins::C0 && PortPinId <= PortPins::C5, "AnalogInputPin must be an analog pin (C0-C5)");

public:
    // ADC reference voltage options
    enum class AdcReference : uint8_t
    {
        External = 0,   // AREF pin
        VCC = 1,        // AVCC with external capacitor at AREF pin
        Internal1V1 = 3 // Internal 1.1V reference
    };

    AnalogInputPin(AdcReference reference = AdcReference::VCC)
    {
        // Enable ADC
        ADCSRA |= (1 << ADEN);

        // Set reference voltage
        ADMUX = (ADMUX & 0x3F) | (static_cast<uint8_t>(reference) << 6);

        // Default to right-adjust result (10-bit in ADCL/ADCH)
        ADMUX &= ~(1 << ADLAR);

        // Set ADC prescaler to 128 (125kHz @ 16MHz)
        // This gives good accuracy with reasonable conversion time
        ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    }

    uint16_t Read()
    {
        // Select the correct ADC channel based on the pin
        uint8_t channel = getAdcChannel();

        // Set the ADC channel (low 3 bits of ADMUX)
        ADMUX = (ADMUX & 0xF8) | (channel & 0x07);

        // Add small delay after changing channel
        //_delay_us(10);

        // Start conversion
        ADCSRA |= (1 << ADSC);

        // Wait for conversion to complete
        while (ADCSRA & (1 << ADSC))
            ;

        // Return result
        return ADC;
    }

    /*
        Returns the Port based on the template parameter.
     */
    uint8_t getPort() const
    {
        return TO_PORT(PortPinId);
    }

    /*
        Returns the Pin based on the template parameter.
     */
    uint8_t getPin() const
    {
        return TO_PIN(PortPinId);
    }

    /*
        Returns the PortPinId template parameter.
     */
    uint8_t getPortPin() const
    {
        return PortPinId;
    }

private:
    constexpr static uint8_t getAdcChannel()
    {
        // Map C0-C5 pins to appropriate ADC channels
        return (uint8_t)PortPinId - (uint8_t)PortPins::C0;
    }
};

// -----------------------------------------------------------------------------
// Other ADC usage examples

/*
uint16_t ReadVcc()
{
    // Set ADC to measure 1.1V reference against AVcc
    ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1);
    _delay_ms(2); // Wait for Vref to settle

    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC))
        ; // Wait for completion

    uint16_t result = ADC;
    // Calculate Vcc in millivolts: Vcc = 1.1 * 1024 * 1000 / ADC
    return (1125300L / result);
}

int16_t ReadTemperature()
{
    // Set ADC to measure temperature sensor
    ADMUX = (1 << REFS0) | (1 << REFS1) | (1 << MUX3);
    _delay_ms(2); // Wait for ADC to settle

    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC))
        ; // Wait for completion

    // Simple temperature calculation (needs calibration for accuracy)
    return (ADC - 324) / 1.22; // Rough approximation in °C
}

uint8_t GetRandomByte()
{
    // Sample unconnected ADC input for noise
    ADMUX = (1 << REFS0);
    uint8_t result = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC))
            ;
        result = (result << 1) | (ADC & 0x01);
    }

    return result;
}

void SetupAnalogComparator()
{
    // Route ADC multiplexer to analog comparator
    ADCSRB |= (1 << ACME);

    // Select ADC input 1 as negative input to comparator
    ADMUX = 1;

    // Disable digital input buffer on AIN0 pin to save power
    DIDR1 |= (1 << AIN0D);

    // Enable analog comparator interrupt on rising edge
    ACSR = (1 << ACIE) | (1 << ACIS1) | (1 << ACIS0);
}

void StartFreeRunningADC()
{
    // Set auto-trigger source to free running mode
    ADCSRA |= (1 << ADATE);
    ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0));

    // Enable ADC interrupt
    ADCSRA |= (1 << ADIE);

    // Start first conversion
    ADCSRA |= (1 << ADSC);
}
    */