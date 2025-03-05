#pragma once
#include "Port.h"

/*
    Initializes a PortPin to output.
 */
template <const PortPins PortPinId>
class DigitalOutputPin
{
public:
    /*
        The ctor sets the Pin on the Port to Output.
     */
    DigitalOutputPin()
    {
        PortPin<portPin>::SetDirection(Output);
    }

    /*
        The ctor sets the Pin on the Port to Output and writes the initialValue.
     */
    DigitalOutputPin(bool initialValue)
    {
        PortPin<portPin>::SetDirection(Output);
        PortPin<portPin>::Write(initialValue);
    }

    /*
        Writes the value to the Port/Pin.
     */
    void Write(bool value) const
    {
        PortPin<portPin>::Write(value);
    }

    /*
        Returns the value of the Port/Pin.
     */
    bool getValue() const
    {
        return PortPin<portPin>::Read();
    }

    /*
        Returns the Port based on the template parameter.
     */
    Ports getPort() const
    {
        return TO_PORT(PortPinId);
    }

    /*
        Returns the Pin based on the template parameter.
     */
    Pins getPin() const
    {
        return TO_PIN(PortPinId);
    }

    /*
        Returns the portPin template parameter.
     */
    PortPins getPortPin() const
    {
        return PortPinId;
    }
};
