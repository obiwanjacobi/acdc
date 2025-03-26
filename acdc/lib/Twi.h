#pragma once
#include <stdint.h>
#include "atl/Bit.h"
#include "Port.h"
#include "PowerReduction.h"

enum class I2cFrequency : uint16_t
{
    Slow = 25,
    Normal = 100,
    Fast = 400
};

enum class TwiResult : uint8_t
{
    Ok,
    InvalidParameter,
    StartFailed,
    AddressFailed,
    DataFailed
};

#define PromoteFailure(result)   \
    if (TwiT::HasFailed(result)) \
        return result;

class Twi
{
public:
    static TwiResult Open(I2cFrequency frequency, bool enablePullups = true)
    {
        return Open(((uint32_t)frequency) * 1000, enablePullups);
    }
    static TwiResult Open(uint32_t frequency, bool enablePullups = true)
    {
        if (frequency == 0)
        {
            Close();
            return TwiResult::InvalidParameter;
        }

        PortPin<PortPins::C4>::EnablePullup(enablePullups);
        PortPin<PortPins::C5>::EnablePullup(enablePullups);

        PowerReduction::Twi(PowerState::On);

        // Calculate bit rate register value for desired frequency
        // TWBR = ((CPU_FREQ / SCL_FREQ) - 16) / 2
        uint8_t prescaler = 0; // No prescaling (TWSR bits 0-1 = 00)
        uint8_t bitRateReg = ((F_CPU / frequency) - 16) / (2 << prescaler);

        // Set bit rate
        TWBR = bitRateReg;

        // Clear prescaler bits and set to 0
        TWSR &= ~((1 << TWPS0) | (1 << TWPS1));

        Enable(true);

        return TwiResult::Ok;
    }

    static void Close()
    {
        Enable(false);
        PowerReduction::Twi(PowerState::Off);
    }

    static void Enable(bool enable = true)
    {
        BitFlag::Set(TWCR, TWEN, enable);
    }

    // Send START condition
    static TwiResult Start(uint8_t address, bool read)
    {
        if (!IsValidAddress(address))
            return TwiResult::InvalidParameter;

        // Send START condition
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
        WaitForComplete();

        // Check status
        if ((TWSR & 0xF8) != 0x08)
            return TwiResult::StartFailed;

        // Send address and R/W bit
        Send((address << 1) | (read ? 1 : 0));

        // Check if address was acknowledged
        if ((TWSR & 0xF8) != (read ? 0x40 : 0x18))
            return TwiResult::AddressFailed;

        return TwiResult::Ok;
    }

    // Send STOP condition
    static void Stop()
    {
        // Transmit STOP condition
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

        // Wait until STOP condition is executed and bus released
        while (TWCR & (1 << TWSTO))
            ;
    }

    // Send data byte
    static TwiResult Write(uint8_t data)
    {
        Send(data);

        // Check if data was acknowledged
        if ((TWSR & 0xF8) != 0x28)
            return TwiResult::DataFailed;

        return TwiResult::Ok;
    }

    // Read data byte with ACK (more bytes to follow)
    static uint8_t ReadAck()
    {
        // Signal acknowledgment after reception
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
        WaitForComplete();

        // Return received data
        return TWDR;
    }

    // Read data byte with NACK (last byte)
    static uint8_t ReadNack()
    {
        // Signal no acknowledgment after reception
        TWCR = (1 << TWINT) | (1 << TWEN);
        WaitForComplete();

        // Return received data
        return TWDR;
    }

    static bool IsValidAddress(uint8_t address)
    {
        return address > 0 && (address & 0x80) == 0;
    }

    static bool HasFailed(TwiResult result)
    {
        return result != TwiResult::Ok;
    }

private:
    Twi() {}
    static void Send(uint8_t data)
    {
        TWDR = data;
        TWCR = (1 << TWINT) | (1 << TWEN);
        WaitForComplete();
    }
    static void WaitForComplete()
    {
        while (!(TWCR & (1 << TWINT)))
            ;
    }
};

template <class TwiT = Twi>
class TwiTransmit : public TwiT
{
public:
    static TwiResult WriteRegister(uint8_t address, uint8_t reg, uint8_t data)
    {
        TwiResult result = TwiT::Start(address, false);
        PromoteFailure(result);

        result = TwiT::Write(reg);
        PromoteFailure(result);

        result = TwiT::Write(data);
        PromoteFailure(result);

        TwiT::Stop();

        return TwiResult::Ok;
    }

private:
    TwiTransmit() {}
};

template <class TwiT = Twi>
class TwiReceive : public TwiT
{

public:
    static TwiResult TryReadRegister(uint8_t address, uint8_t reg, uint8_t *outData)
    {
        TwiResult result = TwiT::Start(address, false);
        PromoteFailure(result);

        result = TwiT::Write(reg);
        PromoteFailure(result);

        result = TwiT::Start(address, true);
        PromoteFailure(result);

        *outData = TwiT::ReadNack();

        TwiT::Stop();

        return TwiResult::Ok;
    }

private:
    TwiReceive() {}
};
