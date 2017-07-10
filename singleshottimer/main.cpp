#include "SingleShotTimer.h"
#include <iostream>
#include <unistd.h>

class Interval
{
private:
    SingleShotTimer<2, 1> m_Timer;
    std::atomic<bool> m_Running;
    unsigned char i;
public:
    void PeriodicTask()
    {
        if(m_Running)
        {
            std::cout<<"Tick: "<<+i++<<std::endl;
            Interval* const self = this;
            m_Timer.ScheduleTask(1000, [self](){
                self->PeriodicTask();
            });
        }
    }
    void Start()
    {
        i = 0;
        m_Running = true;
        PeriodicTask();
    }
    void Stop()
    {
        m_Running = false;
    }
};

int main()
{
    Interval interval;
    interval.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(9500));
    interval.Stop();
    while(1);
    return 0;
}
