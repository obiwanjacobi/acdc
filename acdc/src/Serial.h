#pragma once
#include "../lib/Usart.h"
#include "../lib/UsartInputStream.h"
#include "../lib/UsartOutputStream.h"
#include "../lib/atl/RingBuffer.h"
#include "../lib/atl/TextWriter.h"

enum class BaudRates : uint32_t
{
    Baud9600 = 9600,
    Baud19200 = 19200,
    Baud38400 = 38400,
    Baud57600 = 57600,
    Baud115200 = 115200
};

const UsartIds usartId = UsartIds::Usart0;
const uint8_t CharacterBufferSize = 6;
class Serial : public Usart<
                   usartId,
                   TextWriter<UsartOutputStream<UsartTransmit<usartId>, RingBufferFast<uint8_t, CharacterBufferSize>>>,
                   UsartInputStream<UsartReceive<usartId>, RingBufferFast<uint8_t, CharacterBufferSize>>>
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
