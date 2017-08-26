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
    {
        std::cout<<"Three threads"<<std::endl;
        ThreadPool<1/*Number of priority levels*/,3/*Number of threads*/> threadpool;
        for(unsigned int i = 0 ; i < 10 ; i++)
        {
            threadpool.Enqueue([i, &threadpool](){
                std::cout<<"Task "<<i<<std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if(i == 9)
		{
                    //threadpool.Stop(); // This leads to blocking process.
                    threadpool.StopAsync(); // Instead, one must use StopAsync inside the task.
		}
            });
        }
        while(threadpool.Tasks());
    }
    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    return 0;
}
