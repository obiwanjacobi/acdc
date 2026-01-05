#pragma once
#include "../lib/Usart.h"
#include "../lib/UsartInputStream.h"
#include "../lib/UsartOutputStream.h"
#include "../lib/atl/RingBuffer.h"
#include "../lib/atl/TextWriter.h"
#include "../lib/atl/Slice.h"

enum class BaudRates : uint32_t
{
    Baud9600 = 9600,
    Baud19200 = 19200,
    Baud38400 = 38400,
    Baud57600 = 57600,
    Baud115200 = 115200
};

template <class BaseT>
class DataWriter : public BaseT
{
public:
    void WriteData(uint8_t data)
    {
        BaseT::Write(data);
    }
    void WriteBuffer(Slice<uint8_t> &buffer)
    {
        for (uint8_t i = 0; i < buffer.getLength(); i++)
        {
            BaseT::Write(buffer[i]);
        }
    }
};

const UsartIds usartId = UsartIds::Usart0;
const uint8_t CharacterBufferSize = 21;

typedef TextWriter<DataWriter<UsartOutputStream<UsartTransmit<usartId>, RingBuffer<uint8_t, CharacterBufferSize>>>> SerialWriter;
typedef UsartInputStream<UsartReceive<usartId>, RingBuffer<uint8_t, CharacterBufferSize>> SerialReader;

class Serial : public Usart<usartId, SerialWriter, SerialReader>
{
public:
    bool Open(BaudRates baudRate, bool enableInterrupts = true)
    {
        UsartConfig config;
        if (config.InitAsync((uint32_t)baudRate) &&
            OpenAsync(config))
        {
            Transmit.setEnable();
            Transmit.setEnableAcceptDataInterrupt(enableInterrupts);
            Receive.setEnable();
            Receive.setEnableIsCompleteInterrupt(enableInterrupts);
            return true;
        }
        return false;
    }
};
