#define DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../lib/Interupt.h"
#include "../lib/Port.h"
#include "../lib/PwmTimer.h"
#include "../lib/PwmOutputPin.h"
#include "../lib/atl/Debug.h"
#include "../lib/atl/Delays.h"
#include "../lib/atl/Time.h"
#include "../lib/atl/TimeResolution.h"
#include "../lib/atl/TimeoutTask.h"
#include "../lib/atl/ToggleOutputPinTask.h"

#include "Serial.h"
#include "PwmTask.h"

PwmTimer2 pwmTimer2;
PwmOutputPin<PwmTimer2, PortPins::D3> pwmOutputPin(&pwmTimer2);

const uint8_t MaxItems = 5;
typedef Delays<Time<TimeResolution::Milliseconds>, MaxItems> Scheduler;

PwmTask<Scheduler, PwmOutputPin<PwmTimer2, PortPins::D3>, 10> pwmTask;
Serial serial;

// DigitalOutputPin<PortPins::B5> led(false);
TimeoutTask<ToggleOutputPinTask<PortPins::B5>, Scheduler, 300> blinkLedTask;

class Program
{
public:
    void Initialize()
    {
        // Start the timer that powers Time<TimeResolution>
        TimerCounter0::Start();

        UsartConfig config;
        config.InitAsync((uint32_t)BaudRates::Baud9600);
        serial.OpenAsync(config);
        serial.Transmit.setEnable();
        serial.Transmit.setEnableAcceptDataInterrupt(true);
        //  serial.Receive.setEnable();
        //  serial.Receive.setEnableIsCompleteInterrupt(true);

        Interupts::Enable();

        serial.Transmit.WriteLine("AC/DC v1.0");
    }

    void Run()
    {
        Scheduler::Update();

        blinkLedTask.Run();
    }
};

Program program;

int main()
{
    program.Initialize();

    while (1)
    {
        program.Run();
    }

    return 0;
}

ISR(USART_RX_vect)
{
    serial.Receive.OnIsCompleteInterrupt();
}

ISR(USART_UDRE_vect)
{
    serial.Transmit.OnAcceptDataInterrupt();
}

void AtlDebugWrite(const char *message)
{
    serial.Transmit.Write("TRACE: ");
    serial.Transmit.WriteLine(message);
}

// bool AtlDebugLevel(uint8_t componentId, DebugLevel level)
// {
//     return true;
// }
