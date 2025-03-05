#pragma once
#include <stdint.h>
#include <avr/io.h>

enum class Ports : uint8_t
{
    None = 0,
#ifdef PORTA
    A = 0x08,
#endif // PORTA
#ifdef PORTB
    B = 0x10,
#endif // PORTB
#ifdef PORTC
    C = 0x18,
#endif // PORTC
#ifdef PORTD
    D = 0x20,
#endif // PORTC
#ifdef PORTE
    E = 0x28,
#endif // PORTE
#ifdef PORTF
    F = 0x30,
#endif // PORTF
#ifdef PORTG
    G = 0x38,
#endif // PORTG
#ifdef PORTH
    H = 0x40,
#endif // PORTH
#ifdef PORTJ
    J = 0x48,
#endif // PORTJ
#ifdef PORTK
    K = 0x50,
#endif // PORTK
#ifdef PORTL
    L = 0x58,
#endif // PORTL
};

enum PinIO : uint8_t
{
    Input,
    Output
};

enum class Pins : uint8_t
{
    Pin0 = 0,
    Pin1 = 1,
    Pin2 = 2,
    Pin3 = 3,
    Pin4 = 4,
    Pin5 = 5,
    Pin6 = 6,
    Pin7 = 7
};

// clang-format off
enum class PortPins : uint8_t
{
    None = 0,
#ifdef PORTA
    A0 = 0x08, A1, A2, A3, A4, A5, A6, A7,
#endif // PORTA
#ifdef PORTB
    B0 = 0x10, B1, B2, B3, B4, B5, B6, B7,
#endif // PORTB
#ifdef PORTC
    C0 = 0x18, C1, C2, C3, C4, C5, C6, C7,
#endif // PORTC
#ifdef PORTD
    D0 = 0x20, D1, D2, D3, D4, D5, D6, D7,
#endif // PORTD
#ifdef PORTE
    E0 = 0x28, E1, E2, E3, E4, E5, E6, E7,
#endif // PORTE
#ifdef PORTF
    F0 = 0x30, F1, F2, F3, F4, F5, F6, F7,
#endif // PORTF
#ifdef PORTG
    G0 = 0x38, G1, G2, G3, G4, G5, G6, G7,
#endif // PORTG
#ifdef PORTH
    H0 = 0x40, H1, H2, H3, H4, H5, H6, H7,
#endif // PORTH
#ifdef PORTJ
    J0 = 0x48, J1, J2, J3, J4, J5, J6, J7,
#endif // PORTJ
#ifdef PORTK
    K0 = 0x50, K1, K2, K3, K4, K5, K6, K7,
#endif // PORTK
#ifdef PORTL
    L0 = 0x58, L1, L2, L3, L4, L5, L6, L7,
#endif // PORTL
};
// clang-format on

#define TO_PORT(portPin) ((Ports)((uint8_t)portPin & 0xF8))
#define TO_PIN(portPin) ((Pins)((uint8_t)portPin & 0x07))
#define TO_PORTPIN(port, pin) ((PortPins)((uint8_t)port | (uint8_t)pin))

// ----------------------------------------------------------------------------

template <const Ports PortId>
class PortReg
{
    static_assert(PortId != Ports::None, "PortReg<PortId> is not set (Ports::None).");

    // get from port id to register memory address
    static const uint8_t baseRegister = (((uint8_t)PortId >> 3) - 1) * 3;

public:
    static volatile uint8_t &In()
    {
        return _SFR_IO8(baseRegister);
    }

    static volatile uint8_t &Dir()
    {
        return _SFR_IO8(baseRegister + 1);
    }

    static volatile uint8_t &Out()
    {
        return _SFR_IO8(baseRegister + 2);
    }

private:
    PortReg() {}
};

// ----------------------------------------------------------------------------

/*
 *
 */
template <const Ports PortId>
class Port
{
    static_assert(PortId != Ports::None, "Port<PortId> is not set (Ports::None).");

public:
    // Enables the internal pull-up for inputs.
    // Returns false when pin is not an input.
    static bool EnablePullup(Pins pin, bool enable = true)
    {
        uint8_t mask = PinToMask(pin);

        if ((PortReg<PortId>::Dir() & mask) == Input)
        {
            PortReg<PortId>::Out() |= mask;
            return true;
        }

        return false;
    }

    static void EnableAllPullups(bool enable = true)
    {
        if (enable)
            PortReg<PortId>::Out() = ~PortReg<PortId>::Dir();
        else
            PortReg<PortId>::Out() = PortReg<PortId>::Dir();
    }

    static void SetDirection(Pins pin, PinIO io)
    {
        uint8_t mask = PinToMask(pin);

        if (io)
            PortReg<PortId>::Dir() |= mask;
        else
            PortReg<PortId>::Dir() &= ~mask;
    }

    static void SetDirection(PinIO io7, PinIO io6, PinIO io5, PinIO io4, PinIO io3, PinIO io2, PinIO io1, PinIO io0)
    {
        PortReg<PortId>::Dir() = io7 << 7 | io6 << 6 | io5 << 5 | io4 << 4 | io3 << 3 | io2 << 2 | io1 << 1 | io0;
    }

    static void SetDirection(uint8_t allPinsIO)
    {
        PortReg<PortId>::Dir() = allPinsIO;
    }

    static void Write(Pins pin, bool value)
    {
        uint8_t mask = PinToMask(pin);

        if (value)
            PortReg<PortId>::Out() |= mask;
        else
            PortReg<PortId>::Out() &= ~mask;
    }

    static bool Read(Pins pin)
    {
        return (PortReg<PortId>::In() & PinToMask(pin)) > 0;
    }

    Ports getPort() const
    {
        return PortId;
    }

private:
    Port() {}
    static uint8_t PinToMask(Pins pin)
    {
        return 1 << (uint8_t)pin;
    }
};

// ----------------------------------------------------------------------------

template <const PortPins PortPinId>
class PortPin
{
    const static Ports PortId = TO_PORT(PortPinId);
    const static Pins PinId = TO_PIN(PortPinId);

public:
    // Enables the internal pull-up for inputs.
    // Returns false when pin is not an input.
    static bool EnablePullup(bool enable = true)
    {
        uint8_t mask = PinToMask();

        if ((PortReg<PortId>::Dir() & mask) == Input)
        {
            PortReg<PortId>::Out() |= mask;
            return true;
        }

        return false;
    }

    static void SetDirection(PinIO io)
    {
        uint8_t mask = PinToMask();

        if (io)
            PortReg<PortId>::Dir() |= mask;
        else
            PortReg<PortId>::Dir() &= ~mask;
    }

    static void Write(bool value)
    {
        uint8_t mask = PinToMask();

        if (value)
            PortReg<PortId>::Out() |= mask;
        else
            PortReg<PortId>::Out() &= ~mask;
    }

    static bool Read()
    {
        return (PortReg<PortId>::In() & PinToMask()) > 0;
    }

    Ports getPort() const
    {
        return PortId;
    }
    Pins getPin() const
    {
        return PinId;
    }
    PortPins getPortPin() const
    {
        return PortPinId;
    }

private:
    static uint8_t PinToMask()
    {
        return 1 << (uint8_t)PinId;
    }
};
