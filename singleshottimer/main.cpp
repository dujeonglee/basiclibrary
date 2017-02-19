#include "SingleShotTimer.h"
#include <iostream>
#include <unistd.h>

SingleShotTimer<10> timer;
int main()
{
    while(1)
    {
        timer.ScheduleTask(5, []{std::cout<<"TO 5000\n";}, nullptr);
        timer.ScheduleTask(1, []{std::cout<<"TO 1000\n";}, nullptr);
        timer.ScheduleTask(3, []{std::cout<<"TO 3000\n";}, nullptr);
        timer.ScheduleTask(4, []{std::cout<<"TO 4000\n";}, nullptr);
        timer.ScheduleTask(2, []{std::cout<<"TO 2000\n";}, nullptr);
        timer.ScheduleTask(2, []{std::cout<<"TO 2500\n";}, nullptr);
        timer.ScheduleTask(1, []{std::cout<<"TO 1500\n";}, nullptr);
        timer.ScheduleTask(3, []{std::cout<<"TO 3500\n";}, nullptr);
        timer.ScheduleTask(4, []{std::cout<<"TO 4500\n";}, nullptr);
        timer.ScheduleTask(5, []{std::cout<<"TO 5500\n";}, nullptr);
        std::this_thread::sleep_for (std::chrono::milliseconds(1));
    }
    return 0;
}