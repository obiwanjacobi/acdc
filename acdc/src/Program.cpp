#define DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
// #include <utils/delay.h>

#include "../lib/DigitalInputPin.h"
#include "../lib/Interupt.h"
#include "../lib/Port.h"
#include "../lib/PwmTimer.h"
#include "../lib/PwmOutputPin.h"
#include "../lib/ServoTimer.h"
#include "../lib/ServoOutputPin.h"
#include "../lib/Twi.h"
#include "../lib/PCA9685.h"
#include "../lib/INA219.h"

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
#include <ReadWithState.h>

const uint8_t MaxItems = 5;
#define TimeRes TimeResolution::Milliseconds
typedef Delays<Time<TimeRes>, MaxItems> Scheduler;

Serial serial;
TimeoutTask<ToggleOutputPinTask<PortPins::B5>, Scheduler, ToMilliseconds(TimeRes, 300)> blinkLedTask;

// ServoTimer1 servoTimer;
// Servo360OutputPin<ServoTimer1, PortPins::B1> pwmServo1Pin(&servoTimer);
// Servo360OutputPin<ServoTimer1, PortPins::B2> pwmServo2Pin(&servoTimer);

uint8_t _pwm = 0;
const char welcomeMsg[] PROGMEM = "AC/DC v1.0";
const char debugMsg[] PROGMEM = "Debug is enabled";

CommandParser<CommandHandler> commandParser;

// ReadWithState<DigitalInputPin<PortPins::D2>, bool> sensor1;
// ReadWithState<DigitalInputPin<PortPins::D4>, bool> sensor2;
// ReadWithState<DigitalInputPin<PortPins::D5>, bool> sensor3;
// ReadWithState<DigitalInputPin<PortPins::D6>, bool> sensor4;

// 0100_0001
typedef INA219<I2cT, 0x41> Ina219T;
int16_t shunt = 0;

class Program
{
public:
    void Run()
    {
        Scheduler::Update();

        // indication that the program is running
        blinkLedTask.Run();

        int16_t val = 0;
        if (Ina219T::TryReadShuntVoltage(&val) && shunt != val && val > 100)
        {
            serial.Transmit.Write("C");
            serial.Transmit.WriteLine(val);
            shunt = val;
        }

        // ReadSensors();
        ReadSerial();
    }

    // void ReadSensors()
    // {
    //     bool value = false;
    //     if (sensor1.TryRead(&value))
    //     {
    //         commandParser.OnDirection(false);
    //         serial.Transmit.Write("O1:");
    //         serial.Transmit.WriteLine(value);
    //     }
    //     if (sensor2.TryRead(&value))
    //     {
    //         serial.Transmit.Write("O2:");
    //         serial.Transmit.WriteLine(value);
    //     }
    //     if (sensor3.TryRead(&value))
    //     {
    //         serial.Transmit.Write("O3:");
    //         serial.Transmit.WriteLine(value);
    //     }
    //     if (sensor4.TryRead(&value))
    //     {
    //         serial.Transmit.Write("O4:");
    //         serial.Transmit.WriteLine(value);
    //     }
    // }

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
            Stop(1);

        // enable global interrupts
        Interupts::Enable();

        if (Twi::Open(I2cFrequency::Normal) != TwiResult::Ok)
            Stop(2);

        if (!PwmModuleT::Open(50) ||
            !PwmModuleT::setOutputMode(PwmModuleT::OutputDriver::PushPull))
            Stop(3);

        if (!Ina219T::Open(7500))
            Stop(4);

        FixedString<20> temp;
        // welcome message
        temp.CopyFromProgMem(welcomeMsg);
        serial.Transmit.WriteLine(temp);

        temp.CopyFromProgMem(debugMsg);
        LogDebug(temp);
    }

    void Stop(uint8_t code)
    {
        // do something with the code
        // while (1)
        // {
        //     Scheduler::Update();
        //     if (stopTask.Blink(code))
        //         break;
        // }

        blinkLedTask.Write(true);
        // full stop
        while (1)
        {
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
