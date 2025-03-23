#define DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "../lib/DigitalInputPin.h"
#include "../lib/Interupt.h"
#include "../lib/Port.h"
#include "../lib/PwmTimer.h"
#include "../lib/PwmOutputPin.h"
#include "../lib/ServoTimer.h"
#include "../lib/ServoOutputPin.h"
#include "../lib/atl/Debug.h"
#include "../lib/atl/Delays.h"
#include "../lib/atl/FixedString.h"
#include "../lib/atl/Time.h"
#include "../lib/atl/TimeResolution.h"
#include "../lib/atl/TimeoutTask.h"
#include "../lib/atl/ToggleOutputPinTask.h"

#include "Serial.h"
#include "PwmTask.h"
#include "CommandParser.h"
#include "CommandHandler.h"

const uint8_t MaxItems = 5;
#define TimeRes TimeResolution::Milliseconds
typedef Delays<Time<TimeRes>, MaxItems> Scheduler;

Serial serial;
// TimeoutTask<ToggleOutputPinTask<PortPins::B5>, Scheduler, ToMilliseconds(TimeRes, 300)> blinkLedTask;

// ServoTimer1 servoTimer;
// Servo360OutputPin<ServoTimer1, PortPins::B1> pwmServo1Pin(&servoTimer);
// Servo360OutputPin<ServoTimer1, PortPins::B2> pwmServo2Pin(&servoTimer);

DigitalInputPin<PortPins::D2> sensor1;

uint8_t _pwm = 0;
const char welcomeMsg[] PROGMEM = "AC/DC v1.0";
const char debugMsg[] PROGMEM = "Debug is enabled";

CommandParser<CommandHandler> commandParser;

bool sensor1State = false;

class Program
{
public:
    void Run()
    {
        Scheduler::Update();

        // indication that the program is running
        // blinkLedTask.Run();

        bool value = sensor1.Read();
        if (sensor1State != value)
        {
            sensor1State = value;
            commandParser.OnDirection(false);
        }

        ReadSerial();
    }

    void ReadSerial()
    {
        while (serial.Receive.getCount() > 0)
        {
            uint8_t data;
            if (serial.Receive.TryRead(&data))
            {
                // serial echo
                serial.Transmit.Write((const char)data);

                ParseCommand(data);
            }
        }
    }

    bool ParseCommand(char data)
    {
        bool parsed = commandParser.Parse(data);

        if (commandParser.IsError())
        {
            serial.Transmit.WriteLine("?");
            commandParser.Clear();
        }
        else if (commandParser.IsComplete())
        {
            serial.Transmit.WriteLine("ok");
            commandParser.Dispatch();
            commandParser.Clear();
        }

        return parsed;
    }

    void Initialize()
    {
        // Start the timer that powers Time<TimeResolution> / Scheduler
        Scheduler::Start();

        // open serial port (usart)
        if (!serial.Open(BaudRates::Baud115200))
        {
            // blinkLedTask.Write(true);
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
