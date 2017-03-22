// condition_variable::wait (with predicate)
#include <iostream>           // std::cout
#include "ThreadPool.h"

int main ()
{
    ThreadPool<3, 3>* pool = new ThreadPool<3, 3>();
    std::thread StopStartThread = std::thread([pool](){
        while(1)
        {
            pool->Stop();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            pool->Start();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    StopStartThread.detach();
    int a = 0;
    int b = 0;
    int c = 0;
    std::thread EnqueueThread = std::thread([pool,&a,&b,&c](){
        while(1)
        {
            pool->Enqueue([&a](){a++;}, 0);
            pool->Enqueue([&b](){b++;}, 1);
            pool->Enqueue([&c](){c++;}, 2);
        }
    });
    EnqueueThread.detach();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    delete pool;
    std::cout<<a<<":"<<b<<":"<<c<<std::endl;
    return 0;
}
