#include "SingleShotTimer.h"
#include <iostream>


int main()
{
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    { 
        std::cout<<"How to start and stop timer."<<std::endl;
        SingleShotTimer<1/*Priority levels*/, 1/*Number of threads*/> timer;// Timer is automatically started when instanciation.
        timer.Stop();/*If one does not call "Stop", Dtor will call "Stop".*/
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"How to schedule a task."<<std::endl;
        SingleShotTimer<1, 1> timer;
        timer.ScheduleTaskNoExcept(1000, []()->void{
            std::cout<<"Do something"<<std::endl;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"How to cancel a task."<<std::endl;
        SingleShotTimer<1, 1> timer;
        uint32_t task1 = timer.ScheduleTaskNoExcept(1000, []()->void{
            std::cout<<"This task will not be served."<<std::endl;
        });
        timer.ScheduleTaskNoExcept(1000, []()->void{
            std::cout<<"Only this task is served."<<std::endl;
        });
        timer.CancelTask(task1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"How to start periodic task."<<std::endl;
        SingleShotTimer<1, 1> timer;
        uint32_t data = 0;
        std::cout<<"Periodic task ends when counting 49.";
        timer.PeriodicTask(10, [&data]()->bool{
            std::cout<<"Count down "<<data++<<"/49"<<std::endl;
            if(data < 50)
            {
                return true; /*Schedule the task after 10ms.*/
            }
            else
            {
                std::cout<<"Periodic task is completed"<<std::endl;
                return false; /*Stop the task.*/
            }
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    return 0;
}
