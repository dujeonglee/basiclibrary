#include "SingleShotTimer.h"
#include <iostream>
#include <unistd.h>

int main()
{
    SingleShotTimer<1> timer;

    std::thread StartStop = std::thread([&timer](){
        while(1)
        {
            std::cout<<"Stop\n";
            timer.Stop();
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::cout<<"Start\n";
            timer.Start();
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });
    StartStop.detach();

    while(1)
    {
        timer.ScheduleTask(5, []{std::cout<<"TO 9\n";});
        timer.ScheduleTask(1, []{std::cout<<"TO 1\n";});
        timer.ScheduleTask(3, []{std::cout<<"TO 5\n";});
        timer.ScheduleTask(4, []{std::cout<<"TO 7\n";});
        timer.ScheduleTask(2, []{std::cout<<"TO 3\n";});
        timer.ScheduleTask(2, []{std::cout<<"TO 4\n";});
        timer.ScheduleTask(1, []{std::cout<<"TO 2\n";});
        timer.ScheduleTask(3, []{std::cout<<"TO 6\n";});
        timer.ScheduleTask(4, []{std::cout<<"TO 8\n";});
        timer.ScheduleTask(5, []{std::cout<<"TO 10\n";});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
