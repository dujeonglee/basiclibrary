#include <iostream>
#include "ThreadPool.h"

int main(int argc, char *argv[])
{
    ThreadPool<1,1> pool;
    std::cout<<"Sole worker"<<std::endl;
    for(uint32_t i = 0 ; i < 10 ; i++)
    {
        pool.Enqueue([i](){
            std::cout<<"Task: "<<i<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        });
    }
    while(pool.Tasks());

    std::cout<<"Two workers"<<std::endl;
    pool.ResizeWorkerQueue(2);
    for(uint32_t i = 0 ; i < 10 ; i++)
    {
        pool.Enqueue([i](){
            std::cout<<"Task: "<<i<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        });
    }
    while(pool.Tasks());

    std::cout<<"Three workers"<<std::endl;
    pool.ResizeWorkerQueue(3);
    for(uint32_t i = 0 ; i < 10 ; i++)
    {
        pool.Enqueue([i](){
            std::cout<<"Task: "<<i<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        });
    }
    while(pool.Tasks());

    std::cout<<"Sole worker"<<std::endl;
    pool.ResizeWorkerQueue(1);
    for(uint32_t i = 0 ; i < 10 ; i++)
    {
        pool.Enqueue([i](){
            std::cout<<"Task: "<<i<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        });
    }
    while(pool.Tasks());
    return 0;
}
