# basic data structures and libraries
Implementation of basic data structures and libraries for own study.

## Header only data structures
  - AVL Tree: Insert, find, remove functions are provided.
## Libraries
  - SingleShotTimer: C++11 based timer library. One can use this library to implement asynchronous software architecture.
```
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
```
  - ThreadPool: C++ based thread pool library.
```
#include "ThreadPool.h"
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"One thread"<<std::endl;
        ThreadPool<1/*Number of priority levels*/,1/*Number of threads*/> threadpool;
        for(unsigned int i = 0 ; i < 10 ; i++)
        {
            threadpool.Enqueue([i](){
                std::cout<<"Task "<<i<<std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            });
        }
        while(threadpool.Tasks());
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"Two threads"<<std::endl;
        ThreadPool<1/*Number of priority levels*/,2/*Number of threads*/> threadpool;
        for(unsigned int i = 0 ; i < 10 ; i++)
        {
            threadpool.Enqueue([i](){
                std::cout<<"Task "<<i<<std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            });
        }
        while(threadpool.Tasks());
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    {
        std::cout<<"Three threads"<<std::endl;
        ThreadPool<1/*Number of priority levels*/,3/*Number of threads*/> threadpool;
        for(unsigned int i = 0 ; i < 10 ; i++)
        {
            threadpool.Enqueue([i](){
                std::cout<<"Task "<<i<<std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            });
        }
        while(threadpool.Tasks());
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
	return 0;
}
```
