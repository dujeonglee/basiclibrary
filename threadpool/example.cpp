#include <unistd.h>
#include <iostream>
#include <cstdint>
#include <string>
#include "ThreadPool.h"

#define MAXTASKS  (10)

#define YIELD_CPU(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
// Interesting example of serialized data access to shared memory space using ThreadPool with a single worker.
class VeryImportantData
{
    enum ACCESS_PRIORITY: uint8_t
    {
        READ = 0, /*Urgent*/
        WRITE,    /*Less urgent*/
        NUM_OF_PRIORITY
    };
private:
    uint32_t m_ImportantData;
    ThreadPool<ACCESS_PRIORITY::NUM_OF_PRIORITY,1> m_AccessRequests;

public:
    void SetImportantData(const uint32_t value)
    {
        VeryImportantData* const self = this;
        while(m_AccessRequests.Tasks() >= MAXTASKS){YIELD_CPU(0);}
        m_AccessRequests.Enqueue([self, value](){
            self->m_ImportantData = value;
        }, ACCESS_PRIORITY::WRITE);
    }

    uint32_t GetImportantData()
    {
        VeryImportantData* const self = this;
        volatile bool Ready = false;
        uint32_t ret;

        while(m_AccessRequests.Tasks() >= MAXTASKS){YIELD_CPU(0);}

        m_AccessRequests.Enqueue([self, &Ready,&ret](){
            ret = self->m_ImportantData;
            Ready = true;
        }, ACCESS_PRIORITY::READ);

        while(!Ready){YIELD_CPU(0);}

        return ret;
    }
};

int main(int argc, char *argv[])
{
    VeryImportantData important;
    VeryImportantData* const p_important = &important;
    std::thread worker1;
    std::thread worker2;
    std::thread worker3;
    std::thread worker4;

    std::atomic<uint32_t> data;
    data = 0;
    worker1 = std::thread([&data, p_important](){
        while(1)
        {
            p_important->SetImportantData(data++);
        }
    });
    worker1.detach();
    worker2 = std::thread([p_important](){
        while(1)
        {
            std::cout<<p_important->GetImportantData()<<std::endl;
        }
    });
    worker2.detach();
    worker3 = std::thread([&data, p_important](){
        while(1)
        {
            p_important->SetImportantData(data++);
        }
    });
    worker3.detach();
    worker4 = std::thread([p_important](){
        while(1)
        {
            std::cout<<p_important->GetImportantData()<<std::endl;
        }
    });
    worker4.detach();
	while(1)
	{
		sleep(10);
	}
	return 0;
}
