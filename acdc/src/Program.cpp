// #define DEBUG
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
#include "../lib/Twi.h"
#include "../lib/PCA9685.h"
#include "../lib/INA219.h"

#include "../lib/atl/Debug.h"
#include "../lib/atl/Delays.h"
#include "../lib/atl/FixedString.h"
#include "../lib/atl/Time.h"
#include "../lib/atl/TimeResolution.h"
#include "../lib/atl/TimeoutTask.h"
#include "../lib/atl/ToggleOutputPinTaskBase.h"
#include <ReadWithState.h>

#include "Serial.h"
#include "PwmTask.h"
#include "CommandParser.h"
#include "CommandHandler.h"
// #include "SimpleCommandParser.h"
// #include "SimpleCommandHandler.h"
#include "BlockDriverTask.h"
#include "hardware.h"

const uint8_t MaxItems = 5;
#define TimeRes TimeResolution::Milliseconds
typedef Delays<Time<TimeRes>, MaxItems> Scheduler;

Serial serial;
TimeoutTask<ToggleOutputPinTaskBase<PortPins::B5>, Scheduler, ToMilliseconds(TimeRes, 300)> blinkLedTask;

// ServoTimer1 servoTimer;
// Servo360OutputPin<ServoTimer1, PortPins::B1> pwmServo1Pin(&servoTimer);
// Servo360OutputPin<ServoTimer1, PortPins::B2> pwmServo2Pin(&servoTimer);

CommandParser<CommandHandler> commandParser;
// SimpleCommandParser<SimpleCommandHandler> commandParser;

BlockControllerTask<Scheduler> blockControllerTask;

// VL53L0XT_0 sensor0;
// VL53L0XT_1 sensor1;

class Program
{
public:
    void Run()
    {
        Scheduler::Update();

        // indication that the program is running
        blinkLedTask.Run();

        // blockControllerTask.Run(blockController0, blockController1, blockController2, blockController3);

        ReadSerial();
        ReadSensors();

        // if (sensor0.isMeasurementReady() && sensor1.isMeasurementReady())
        // {
        //     uint16_t range0, range1;

        //     sensor0.ReadRange_mm(&range0);
        //     sensor1.ReadRange_mm(&range1);

        //     serial.Transmit.Write("1: ");
        //     serial.Transmit.Write(range0);
        //     serial.Transmit.Write(" - 2: ");
        //     serial.Transmit.Write(range1);
        //     serial.Transmit.WriteLine();
        // }

        // tredding water
        Scheduler::SpinWait(Scheduler::ForMilliseconds(20));
    }

    void ReadSerial()
    {
        while (serial.Receive.getCount() > 0)
        {
            uint8_t data;
            if (serial.Receive.TryRead(&data))
            {
                ParseCommand(data);
            }
        }
    }

    bool ParseCommand(char data)
    {
        bool parsed = commandParser.Parse(data);
        if (parsed)
            serial.Transmit.Write(data);

        if (commandParser.IsError())
        {
            serial.Transmit.WriteLine("?");
            commandParser.Clear();
        }
        else if (commandParser.IsComplete())
        {
            // if (commandParser.Dispatch())
            //      serial.Transmit.WriteLine("ok");
            //  else
            //      serial.Transmit.WriteLine("err");
            if (!commandParser.Dispatch())
                serial.Transmit.WriteLine("?");
            commandParser.Clear();
        }

        return parsed;
    }

    void ReadSensors()
    {
        // uint8_t blockFlags = 0;
        // if (commandParser.TryReadBlocks(&blockFlags))
        // {
        //     serial.Transmit.WriteLine(blockFlags);
        // }

        BlockOccupationEvent *blockEvent = nullptr;
        if (commandParser.TryCreateBlockOccupationEvent(&blockEvent))
        {
            FixedArray<uint8_t, 10> array;
            CommandBuffer buffer(array.getBuffer(), array.getCapacity());
            uint8_t l = blockEvent->Serialize(buffer);
            // shrink buffer to what has actually being written
            CommandBuffer data(buffer, l);
            serial.Transmit.WriteBuffer(data);

            // uint8_t flags = blockEvent->OccupationFlags;
            // serial.Transmit.WriteLine(flags);
        }
    }

    void Initialize()
    {
        // make sure light is off
        blinkLedTask.Write(false);

        // Start the timer that powers Time<TimeResolution> / Scheduler
        Scheduler::Start();

        // open serial port (usart)
        if (!serial.Open(BaudRates::Baud115200))
            Stop(1);

        // enable global interrupts
        Interupts::Enable();

        if (Twi::Open(I2cFrequency::Fast) != TwiResult::Ok)
            Stop(2);

        if (!PwmModuleT::Open(70) ||
            !PwmModuleT::setOutputMode(PwmModuleT::OutputDriver::PushPull))
            Stop(3);

        if (!commandParser.Open())
            Stop(4);

        // if (!sensor0.Open())
        //     Stop(5);
        // sensor0.StartContinuous();

        // if (!sensor1.Open())
        //     Stop(6);
        // sensor1.StartContinuous();
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

        if (code > 1)
        {
            serial.Transmit.Write("Stop: ");
            serial.Transmit.WriteLine(code);
        }

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
    serial.Transmit.Write(" [");
    serial.Transmit.Write(componentId);
    serial.Transmit.Write("] ");

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
    // if (level > DebugLevel::Warning)
    //     return false;

    // if (componentId == 1)
    //     return false;

    if (componentId == Twi::TwiDebugComponentId)
        return false;

    return true;
}

#endif // DEBUG
