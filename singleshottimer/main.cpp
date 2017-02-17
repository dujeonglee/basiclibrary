#include "SingleShotTimer.h"
#include <iostream>
#include <unistd.h>

SingleShotTimer timer;
int main()
{
    while(1)
    {
        timer.ScheduleTask(1000, nullptr, nullptr, "TEST");
        timer.ScheduleTask(1000, nullptr, nullptr, "TEST");
        //std::this_thread::sleep_for (std::chrono::milliseconds(10));
    }
    return 0;
}
