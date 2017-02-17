#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_
#include "ThreadPool.h"
#include <chrono>

typedef std::chrono::time_point<std::chrono::steady_clock> SSTTime;

struct TimerInfo
{
    std::atomic<bool> m_Active;
    std::chrono::time_point<std::chrono::steady_clock> m_TargetTime;
    std::function <void()> m_TimeoutHandler;
    std::function <void()> m_CancelHandler;
    std::string m_Label;
};

class SingleShotTimer
{
private:
    std::mutex m_Lock;
    std::condition_variable m_Condition;
    unsigned long m_MaxTimers;
    std::vector<TimerInfo*> m_MinHeap;
    unsigned long m_Timers;
    std::thread m_Thread;
    ThreadPool<1, 3> m_ThreadPool;
    bool m_Running;

public:
    SingleShotTimer(unsigned long timers = 32):m_MaxTimers(timers)
    {
        try
        {
            m_MinHeap.resize(m_MaxTimers, nullptr);
        }
        catch(const std::bad_alloc& ex)
        {
            m_MaxTimers = 0;
            return;
        }
        m_Timers = 0;
        m_Running = true;
        m_Thread = std::thread([this]()
        {
            while(m_Running)
            {
                TimerInfo* task = nullptr;
                TimerInfo* temp = nullptr;
                {
                    std::unique_lock<std::mutex> lock(m_Lock);
                    while(m_Timers == 0)
                    {
                        m_Condition.wait(lock);
                    }
                    if(m_MinHeap[0]->m_TargetTime <= std::chrono::steady_clock::now())
                    {
                        task = m_MinHeap[0];
                        m_MinHeap[0] = m_MinHeap[--m_Timers];
                        // re-arrang heap
                        {
                            unsigned long position = 0;
                            while(position < m_Timers)
                            {
                                if(position*2+2 < m_Timers)
                                {
                                    temp = ( m_MinHeap[position]->m_TargetTime < m_MinHeap[position*2+1]->m_TargetTime?
                                                 ( m_MinHeap[position]->m_TargetTime < m_MinHeap[position*2+2]->m_TargetTime ? m_MinHeap[position] : m_MinHeap[position*2+2]) :
                                                 ( m_MinHeap[position*2+1]->m_TargetTime < m_MinHeap[position*2+2]->m_TargetTime ? m_MinHeap[position*2+1] : m_MinHeap[position*2+2]));
                                    if(temp == m_MinHeap[position])
                                    {
                                        break;
                                    }
                                    else if(temp == m_MinHeap[position*2+1])
                                    {
                                        m_MinHeap[position*2+1] = m_MinHeap[position];
                                        m_MinHeap[position] = temp;
                                        position = position*2+1;
                                    }
                                    else
                                    {
                                        m_MinHeap[position*2+2] = m_MinHeap[position];
                                        m_MinHeap[position] = temp;
                                        position = position*2+2;
                                    }
                                }
                                else if(position*2+1 < m_Timers)
                                {
                                    temp = ( m_MinHeap[position]->m_TargetTime < m_MinHeap[position*2+1]->m_TargetTime? m_MinHeap[position] : m_MinHeap[position*2+1]);
                                    if(temp == m_MinHeap[position])
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        m_MinHeap[position*2+1] = m_MinHeap[position];
                                        m_MinHeap[position] = temp;
                                        position = position*2+1;
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        std::this_thread::sleep_for (std::chrono::milliseconds(1));
                    }
                }
                if(task && task->m_Active)
                {
                    m_ThreadPool.enqueue([task](){
                        if(task->m_TimeoutHandler)
                        {
                            task->m_TimeoutHandler();
                        }
                        delete task;
                    });
                }
            }
            for(unsigned long i = 0 ; i < m_Timers ; i++)
            {
                delete m_MinHeap[i];
            }
            m_MinHeap.clear();
        });
    }

    ~SingleShotTimer()
    {
        m_Running = false;
        m_Thread.join();
    }

    TimerInfo* ScheduleTask(unsigned long milli, std::function <void()> to, std::function <void()> cancel, const std::string& label = "NoLabel")
    {
        TimerInfo* newone = nullptr;
        unsigned long position = m_Timers;
        if((m_MaxTimers == 0) || (m_Timers >= m_MaxTimers) || m_Running == false)
        {
            return nullptr;
        }
        try
        {
            newone = new TimerInfo();
            newone->m_Active = true;
            newone->m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(milli);
            newone->m_TimeoutHandler = to;
            newone->m_CancelHandler = cancel;
            newone->m_Label = label;
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout<<ex.what()<<std::endl;
            return nullptr;
        }
        {
            std::unique_lock<std::mutex> lock(m_Lock);
            m_MinHeap[position] = newone;
            while(position)
            {
                if(m_MinHeap[position]->m_TargetTime < m_MinHeap[position/2]->m_TargetTime)
                {
                    m_MinHeap[position] = m_MinHeap[position/2];
                    m_MinHeap[position/2] = newone;
                    position = position/2;
                }
                else
                {
                    break;
                }
            }
            m_Timers++;
           m_Condition.notify_one();
        }
        std::cout<<m_Timers<<std::endl;
        return newone;
    }

    void Cancel(TimerInfo* timer)
    {
        timer->m_Active = false;
        m_ThreadPool.enqueue([timer](){
            if(timer->m_CancelHandler)
            {
                timer->m_CancelHandler();
            }
        });
    }
};


#endif
