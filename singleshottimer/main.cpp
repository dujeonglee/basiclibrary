#include "SingleShotTimer.h"
#include <iostream>
#include <unistd.h>

SingleShotTimer timer;
int main()
{
    while(1)
    {
        timer.ScheduleTask(5000, []{std::cout<<"TO 5000\n";}, nullptr);
        timer.ScheduleTask(1000, []{std::cout<<"TO 1000\n";}, nullptr);
        timer.ScheduleTask(3000, []{std::cout<<"TO 3000\n";}, nullptr);
        timer.ScheduleTask(4000, []{std::cout<<"TO 4000\n";}, nullptr);
        timer.ScheduleTask(2000, []{std::cout<<"TO 2000\n";}, nullptr);
        timer.ScheduleTask(2500, []{std::cout<<"TO 2500\n";}, nullptr);
        timer.ScheduleTask(1500, []{std::cout<<"TO 1500\n";}, nullptr);
        timer.ScheduleTask(3500, []{std::cout<<"TO 3500\n";}, nullptr);
        timer.ScheduleTask(4500, []{std::cout<<"TO 4500\n";}, nullptr);
        timer.ScheduleTask(5500, []{std::cout<<"TO 5500\n";}, nullptr);
        std::this_thread::sleep_for (std::chrono::milliseconds(10000));
    }
    return 0;
}
