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
        volatile bool completed = false;
        timer.ScheduleTaskNoExcept(1000, [&completed]()->void{
            std::cout<<"Do something"<<std::endl;
            completed = true;
        });
        while(!completed);
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"How to cancel a task."<<std::endl;
        SingleShotTimer<1, 1> timer;
        volatile bool completed = false;
        uint32_t task1 = timer.ScheduleTaskNoExcept(1000, [&completed]()->void{
            std::cout<<"This task will not be served."<<std::endl;
        });
        timer.ScheduleTaskNoExcept(1000, [&completed]()->void{
            std::cout<<"Only this task is served."<<std::endl;
            completed = true;
        });
        timer.CancelTask(task1);
        while(!completed);
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"How to start periodic task."<<std::endl;
        SingleShotTimer<1, 1> timer;
        const uint32_t delay = 10;
        uint32_t data = 0;
        volatile bool completed = false;
        std::cout<<"Periodic task ends when counting 49."<<std::endl;
        timer.PeriodicTask(delay, [&data, &completed]()->const bool{
            std::cout<<"Count down "<<data++<<"/49"<<std::endl;
            if(data < 50)
            {
                return true; /*Schedule the task after 10ms.*/
            }
            else
            {
                std::cout<<"Periodic task is completed"<<std::endl;
                completed = true;
                return false; /*Stop the task.*/
            }
        });
        while(!completed);
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"How to start advanced periodic task."<<std::endl;
        SingleShotTimer<1, 1> timer;
        uint32_t data = 0;
        volatile bool completed = false;
        std::cout<<"Periodic task with random delay ends when counting 49."<<std::endl;
        timer.PeriodicTaskAdv([&data, &completed]()->const std::tuple<bool, uint32_t, uint32_t>{
            std::cout<<"Count down "<<data++<<"/49"<<std::endl;
            if(data < 50)
            {
                uint32_t delay = rand()%1000;
                std::cout<<"Sleep for "<< delay << std::endl;
                return std::make_tuple(true, delay, 0); /*Schedule the task after delay ms with priority 0.*/
            }
            else
            {
                std::cout<<"Periodic task is completed"<<std::endl;
                completed = true;
                return std::make_tuple(false, 0, 0); /*Stop the task.*/
            }
        });
        while(!completed);
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    return 0;
}
