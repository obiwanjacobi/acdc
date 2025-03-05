#include "../lib/Port.h"
#include "../lib/PwmTimer.h"
#include "../lib/PwmOutputPin.h"
#include "../lib/atl/Delays.h"
#include "../lib/atl/Time.h"
#include "../lib/atl/TimeResolution.h"
#include "../lib/atl/TimeoutTask.h"

#include "PwmTask.h"

PwmTimer2 pwmTimer2;
PwmOutputPin<PwmTimer2, PortPins::D3> pwmOutputPin(pwmTimer2);

// maintain max 10 delays in millisecond precision
const uint8_t MaxItems = 10;
typedef Delays<Time<TimeResolution::Milliseconds>, MaxItems> Scheduler;

PwmTask<Scheduler, PwmOutputPin<PwmTimer2, PortPins::D3>, 10> pwmTask;

class Program
{
public:
    void Initialize()
    {
        TimerCounter::Start();
    }

    void Run()
    {
        Scheduler::Update();

        pwmTask.Execute(pwmOutputPin);
    }
};

Program program;

int main()
{
    program.Initialize();

    // main loop
    while (1)
    {
        program.Run();
    }

    return 0;
}

#include "../lib/TimerCounter.cpp"
