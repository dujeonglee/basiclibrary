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
    std::thread* m_Thread;
    ThreadPool<1, CONCURRENCY> m_ThreadPool;
    std::atomic<bool> m_Running;
    std::mutex m_RunningLock;

public:
    SingleShotTimer()
    {
        m_Running = false;
        Start();
    }

    ~SingleShotTimer()
    {
        Stop();
    }

    TimerInfo* ScheduleTask(unsigned long milli, std::function <void()> to, std::function <void()> cancel)
    {
        std::lock_guard<std::mutex> lock(m_Lock);
        if(m_Running == false)
        {
            return nullptr;
        }
        TimerInfo* newone = nullptr;
        unsigned long position = m_ActiveTimerInfoList.size();
        // 1. Get a free TimerInfo
        try
        {
            newone = new TimerInfo();
        }
        catch(const std::bad_alloc& ex)
        {
            return nullptr;
        }
        // 2. Setup TimerInfo
        newone->m_Active = true;
        newone->m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(milli);
        newone->m_TimeoutHandler = to;
        newone->m_CancelHandler = cancel;

        // 3. Push TimerInfo into ActiveTimerList
        try
        {
            m_ActiveTimerInfoList.push_back(newone);
        }
        catch(std::bad_alloc& ex)
        {
            std::cout<<ex.what()<<std::endl;
            delete newone;
            return nullptr;
        }

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
        m_ThreadPool.Enqueue([timer](){
            if(timer->m_CancelHandler)
            {
                timer->m_CancelHandler();
            }
            /* timer will be deleted in m_Thread. */
        });
    }

    void Start()
    {
        std::unique_lock<std::mutex> lock(m_RunningLock);
        std::cout<<"TIMERINFOPOOLING OFF"<<std::endl;
        {
            std::unique_lock<std::mutex> lock(m_Lock);
            if(m_Running)
            {
                return;
            }
        }
        try
        {
            m_Thread = new std::thread([this]()
            {
                m_Running = true;
                while(m_Running)
                {
                    TimerInfo* task = nullptr;
                    TimerInfo* temp = nullptr;
                    {
                        std::unique_lock<std::mutex> lock(m_Lock);
                        while(m_ActiveTimerInfoList.size() == 0 && m_Running)
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
                            // Yield CPU Resource to other threads
                            std::this_thread::sleep_for (std::chrono::milliseconds(0));
                            continue;
                        }
                    }
                    if(task)
                    {
                        if(task->m_Active)
                        {
                            m_ThreadPool.Enqueue([this, task](){
                                if(task->m_TimeoutHandler)
                                {
                                    task->m_TimeoutHandler();
                                }
                                {
                                    delete task;
                                }
                            });
                        }
                        else
                        {
                            delete task;
                        }
                    }
                }
            });
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout<<ex.what()<<std::endl;
        }
    }

    void Stop()
    {
        std::unique_lock<std::mutex> lock(m_RunningLock);
        if(!m_Running)
        {
            return;
        }
        m_Running = false;
        m_Condition.notify_one();
        if(m_Thread->joinable())
        {
            m_Thread->join();
        }
        delete m_Thread;
        for(unsigned long i = 0 ; i < m_ActiveTimerInfoList.size() ; i++)
        {
            delete m_ActiveTimerInfoList[i];
        }
        m_ActiveTimerInfoList.clear();
    }
};


#endif
