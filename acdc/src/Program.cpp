#define DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "../lib/Interupt.h"
#include "../lib/Port.h"
#include "../lib/PwmTimer.h"
#include "../lib/PwmOutputPin.h"
#include "../lib/atl/Debug.h"
#include "../lib/atl/Delays.h"
#include "../lib/atl/FixedString.h"
#include "../lib/atl/Time.h"
#include "../lib/atl/TimeResolution.h"
#include "../lib/atl/TimeoutTask.h"
#include "../lib/atl/ToggleOutputPinTask.h"

#include "Serial.h"
#include "PwmTask.h"
#include "Commands.h"
#include "CommandHandler.h"

PwmTimer2 pwmTimer2;
PwmOutputPin<PwmTimer2, PortPins::D3> pwmOutputPin(&pwmTimer2);

const uint8_t MaxItems = 5;
typedef Delays<Time<TimeResolution::Milliseconds>, MaxItems> Scheduler;

PwmTask<Scheduler, PwmOutputPin<PwmTimer2, PortPins::D3>, 10> pwmTask;
Serial serial;

TimeoutTask<ToggleOutputPinTask<PortPins::B5>, Scheduler, 300> blinkLedTask;

uint8_t _pwm = 0;
const char welcomeMsg[] PROGMEM = "AC/DC v1.0";
const char debugMsg[] PROGMEM = "Debug is enabled";

CommandParser<RingBufferFast<char, 16>> commandParser;
CommandDispatcher<CommandHandler<SerialWriter>> commandDispatcher;

class Program
{
public:
    void Initialize()
    {
        // Start the timer that powers Time<TimeResolution>
        TimerCounter0::Start();
        // open serial port (usart)
        if (!serial.Open(BaudRates::Baud115200))
        {
            blinkLedTask.Write(true);
            while (1)
            {
                // stop here
            }
        }

        // finally enable global interrupts
        Interupts::Enable();

        FixedString<20> temp;
        // welcome message
        temp.CopyFromProgMem(welcomeMsg);
        serial.Transmit.WriteLine(temp);

        temp.CopyFromProgMem(debugMsg);
        LogDebug(temp);

        commandDispatcher.SetTextWriter(&serial.Transmit);
    }

    void Run()
    {
        Scheduler::Update();

        // indication that the program is running
        blinkLedTask.Run();

        while (serial.Receive.getCount() > 0)
        {
            uint8_t data;
            if (serial.Receive.TryRead(&data))
            {
                // serial echo
                serial.Transmit.Write((const char)data);

                bool parsed = commandParser.Parse(data);
                // serial.Transmit.WriteLine(parsed ? " ok" : " nok");
                if (parsed)
                {
                    if (commandParser.IsError())
                    {
                        serial.Transmit.Write("<X>");
                        commandParser.Clear();
                    }
                    else if (commandParser.IsComplete())
                    {
                        if (commandParser.Dispatch(commandDispatcher))
                        {
                            serial.Transmit.Write("<OK>");
                        }
                        else
                        {
                            serial.Transmit.Write("<NOK>");
                        }
                        commandParser.Clear();
                    }
                }

                switch (data)
                {
                case 'u':
                    _pwm += 10;
                    pwmOutputPin.Write(_pwm);
                    break;
                case 'd':
                    _pwm -= 10;
                    pwmOutputPin.Write(_pwm);
                    break;
                }
            }
        }
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

#ifdef DEBUG

void AtlDebugWrite(uint8_t componentId, DebugLevel level, const char *message)
{
    serial.Transmit.Write(Scheduler::getTicks());
    if (componentId != 0)
    {
        serial.Transmit.Write(" [");
        serial.Transmit.Write(componentId);
        serial.Transmit.Write("] ");
    }
    else
        serial.Transmit.Write(" - ");

    switch (level)
    {
    case DebugLevel::Critical:
        serial.Transmit.Write("CRITICAL: ");
        break;
    case DebugLevel::Error:
        serial.Transmit.Write("ERROR: ");
        break;
    case DebugLevel::Warning:
        serial.Transmit.Write("WARNING: ");
        break;
    case DebugLevel::Info:
        serial.Transmit.Write("INFO: ");
        break;
    case DebugLevel::Trace:
        serial.Transmit.Write("TRACE: ");
        break;
    case DebugLevel::Debug:
        serial.Transmit.Write("DEBUG: ");
        break;
    default:
        break;
    }
    serial.Transmit.WriteLine(message);
}

bool AtlDebugFilter(uint8_t componentId, DebugLevel level)
{
    if (componentId == 1)
        return false;

    return true;
}

#endif // DEBUG
