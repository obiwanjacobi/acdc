#define DEBUG
#include "../lib/atl/Debug.h"
#include "../lib/Port.h"
#include "../lib/PwmTimer.h"
#include "../lib/PwmOutputPin.h"
#include "../lib/atl/Delays.h"
#include "../lib/atl/Time.h"
#include "../lib/atl/TimeResolution.h"
#include "../lib/atl/TimeoutTask.h"

#include "PwmTask.h"

PwmTimer2 pwmTimer2;
PwmOutputPin<PwmTimer2, PortPins::D3> pwmOutputPin(&pwmTimer2);

const uint8_t MaxItems = 5;
typedef Delays<Time<TimeResolution::Milliseconds>, MaxItems> Scheduler;

PwmTask<Scheduler, PwmOutputPin<PwmTimer2, PortPins::D3>, 10> pwmTask;

class Program
{
public:
    void Initialize()
    {
        // Start the timer that powers Time<TimeResolution>
        TimerCounter0::Start();
    }

    void Run()
    {
        Scheduler::Update();

        pwmTask.Run(pwmOutputPin);
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
