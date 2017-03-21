// condition_variable::wait (with predicate)
#include <iostream>           // std::cout
#include "ThreadPool.h"

int main ()
{
    ThreadPool<2, 2> pool;
    std::thread* prio0;
    std::thread* prio1;
    prio0 = new std::thread([&](){
        while(1)
        {
            pool.Enqueue([&](){std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!PRI0\n";std::this_thread::sleep_for(std::chrono::seconds(1));}, 0);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
    prio0->detach();
    prio1 = new std::thread([&](){
        while(1)
        {
            if(pool.ActiveWorkers() == 0)
                pool.Enqueue([&](){/*std::cout<<"PRI1\n";*/}, 1);
        }
    });
    prio1->detach();
    while(1)
    {
        std::cout<<"Active "<<pool.ActiveWorkers()<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
