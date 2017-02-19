#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_
#include "ThreadPool.h"
#include <chrono>

typedef std::chrono::time_point<std::chrono::steady_clock> SSTTime;

class TimerInfo
{
public:
    std::atomic<bool> m_Active;
    SSTTime m_TargetTime;
    std::function <void()> m_TimeoutHandler;
    std::function <void()> m_CancelHandler;
};
template <unsigned long CONCURRENCY>
class SingleShotTimer
{
private:
    std::mutex m_Lock;
    std::condition_variable m_Condition;
    std::vector<TimerInfo*> m_ActiveTimerInfoList;
    std::vector<TimerInfo*> m_TimerInfoPool;
    std::thread m_Thread;
    ThreadPool<1, CONCURRENCY> m_ThreadPool;
    std::atomic<bool> m_Running;

public:
    SingleShotTimer()
    {
        m_Running = true;
        m_Thread = std::thread([this]()
        {
            while(m_Running)
            {
                TimerInfo* task = nullptr;
                TimerInfo* temp = nullptr;
                {
                    std::unique_lock<std::mutex> lock(m_Lock);
                    while(m_ActiveTimerInfoList.size() == 0)
                    {
                        m_Condition.wait(lock);
                    }
                    if(m_Running == false)
                    {
                        return;
                    }
                    if(m_ActiveTimerInfoList[0]->m_TargetTime <= std::chrono::steady_clock::now())
                    {
                        task = m_ActiveTimerInfoList[0];
                        m_ActiveTimerInfoList[0] = m_ActiveTimerInfoList.back();
                        m_ActiveTimerInfoList.pop_back();
                        const unsigned long TIMERS = m_ActiveTimerInfoList.size();
                        // re-arrang heap
                        {
                            unsigned long position = 0;
                            while(position < TIMERS)
                            {
                                if(position*2+2 < TIMERS)
                                {
                                    temp = ( m_ActiveTimerInfoList[position]->m_TargetTime < m_ActiveTimerInfoList[position*2+1]->m_TargetTime?
                                                 ( m_ActiveTimerInfoList[position]->m_TargetTime < m_ActiveTimerInfoList[position*2+2]->m_TargetTime ? m_ActiveTimerInfoList[position] : m_ActiveTimerInfoList[position*2+2]) :
                                                 ( m_ActiveTimerInfoList[position*2+1]->m_TargetTime < m_ActiveTimerInfoList[position*2+2]->m_TargetTime ? m_ActiveTimerInfoList[position*2+1] : m_ActiveTimerInfoList[position*2+2]));
                                    if(temp == m_ActiveTimerInfoList[position])
                                    {
                                        break;
                                    }
                                    else if(temp == m_ActiveTimerInfoList[position*2+1])
                                    {
                                        m_ActiveTimerInfoList[position*2+1] = m_ActiveTimerInfoList[position];
                                        m_ActiveTimerInfoList[position] = temp;
                                        position = position*2+1;
                                    }
                                    else
                                    {
                                        m_ActiveTimerInfoList[position*2+2] = m_ActiveTimerInfoList[position];
                                        m_ActiveTimerInfoList[position] = temp;
                                        position = position*2+2;
                                    }
                                }
                                else if(position*2+1 < TIMERS)
                                {
                                    temp = ( m_ActiveTimerInfoList[position]->m_TargetTime < m_ActiveTimerInfoList[position*2+1]->m_TargetTime? m_ActiveTimerInfoList[position] : m_ActiveTimerInfoList[position*2+1]);
                                    if(temp == m_ActiveTimerInfoList[position])
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        m_ActiveTimerInfoList[position*2+1] = m_ActiveTimerInfoList[position];
                                        m_ActiveTimerInfoList[position] = temp;
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
                        continue;
                    }
                }
                if(task && task->m_Active)
                {
                    m_ThreadPool.enqueue([this, task](){
                        if(task->m_TimeoutHandler)
                        {
                            task->m_TimeoutHandler();
                        }
                        {
                            std::lock_guard<std::mutex> lock(m_Lock);
                            m_TimerInfoPool.push_back(task);
                        }
                    });
                }
            }
        });
    }

    ~SingleShotTimer()
    {
        m_Running = false;
        m_Condition.notify_one();
        m_Thread.join();
        {
            std::lock_guard<std::mutex> lock(m_Lock);
            for(unsigned long i = 0 ; i < m_ActiveTimerInfoList.size() ; i++)
            {
                delete m_ActiveTimerInfoList[i];
            }
            m_ActiveTimerInfoList.clear();
        }

        while(m_ThreadPool.tasks());
        for(unsigned long i = 0 ; i < m_TimerInfoPool.size() ; i++)
        {
            delete m_TimerInfoPool[i];
        }
        m_TimerInfoPool.clear();
    }

    TimerInfo* ScheduleTask(unsigned long milli, std::function <void()> to, std::function <void()> cancel)
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        TimerInfo* newone = nullptr;
        unsigned long position = m_ActiveTimerInfoList.size();
        if(m_Running == false)
        {
            return nullptr;
        }
        // 1. Get a free TimerInfo
        if(m_TimerInfoPool.size() == 0)
        {
            try
            {
                newone = new TimerInfo();
            }
            catch(const std::bad_alloc& ex)
            {
                return nullptr;
            }
            try
            {
                m_TimerInfoPool.push_back(newone);
            }
            catch(const std::bad_alloc& ex)
            {
                delete newone;
                return nullptr;
            }
        }
        newone = m_TimerInfoPool.back();
        m_TimerInfoPool.pop_back();

        // 2. Setup TimerInfo
        newone->m_Active = true;
        newone->m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(milli);
        newone->m_TimeoutHandler = to;
        newone->m_CancelHandler = cancel;

        // 3. Push TimerInfo into ActiveTimerList
        m_ActiveTimerInfoList.push_back(newone);
        while(position)
        {
            if(m_ActiveTimerInfoList[position]->m_TargetTime < m_ActiveTimerInfoList[position/2]->m_TargetTime)
            {
                m_ActiveTimerInfoList[position] = m_ActiveTimerInfoList[position/2];
                m_ActiveTimerInfoList[position/2] = newone;
                position = position/2;
            }
            else
            {
                break;
            }
        }
        m_Condition.notify_one();
        return newone;
    }

    void Cancel(TimerInfo* timer)
    {
        std::lock_guard<std::mutex> lock(m_Lock);
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
