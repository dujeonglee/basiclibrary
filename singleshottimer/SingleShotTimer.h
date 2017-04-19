#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_
#include "ThreadPool.h"
#include <chrono>
#include <ctime>

typedef std::chrono::time_point<std::chrono::steady_clock> SSTTime;

class TimerInfo
{
public:
    SSTTime m_TargetTime;
    std::function <void()> m_TimeoutHandler;
    unsigned long m_Priority;
    void PrintTime()
    {
        std::time_t ttp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + (m_TargetTime - std::chrono::steady_clock::now()));
        std::cout << "time: " << std::ctime(&ttp);
    }
};
template <unsigned long PRIORITY, unsigned long CONCURRENCY>
class SingleShotTimer
{
private:
    std::mutex m_APILock;

    std::mutex m_ActiveTimerInfoListLock;
    std::condition_variable m_Condition;
    std::vector<TimerInfo*> m_ActiveTimerInfoList;
    std::thread* m_Thread;
    ThreadPool<PRIORITY, CONCURRENCY> m_ThreadPool;
    std::atomic<bool> m_Running;

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

    bool ScheduleTask(unsigned long milli, std::function <void()> to, unsigned long priority = 0)
    {
        std::lock_guard<std::mutex> APILock(m_APILock);
        if(m_Running == false)
        {
            return false;
        }
        std::lock_guard<std::mutex> ActiveTimerInfoListLock(m_ActiveTimerInfoListLock);
        TimerInfo* newone = nullptr;
        unsigned long position = m_ActiveTimerInfoList.size();
        // 1. Get a free TimerInfo
        try
        {
            newone = new TimerInfo();
        }
        catch(const std::bad_alloc& ex)
        {
            return false;
        }
        // 2. Setup TimerInfo
        newone->m_TargetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(milli);
        newone->m_TimeoutHandler = to;
        newone->m_Priority = priority;

        // 3. Push TimerInfo into ActiveTimerList, which is min heap.
        try
        {
            m_ActiveTimerInfoList.push_back(newone);
        }
        catch(std::bad_alloc& ex)
        {
            std::cout<<ex.what()<<std::endl;
            delete newone;
            return false;
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
        return true;
    }

    void Start()
    {
        std::unique_lock<std::mutex> APILock(m_APILock);
        if(m_Running)
        {
            return;
        }
        m_Running = true;
        try
        {
            m_Thread = new std::thread([this]()
            {
                while(m_Running)
                {
                    TimerInfo* task = nullptr;
                    {
                        std::unique_lock<std::mutex> ActiveTimerInfoListLock(m_ActiveTimerInfoListLock);
#ifdef BUSYWAITING
                        while(m_ActiveTimerInfoList.size() == 0 && m_Running)
                        {
                            m_Condition.wait(ActiveTimerInfoListLock);
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
                                unsigned long parent = 0;
                                while(parent < TIMERS)
                                {
                                    const unsigned long leftchild = parent*2+1;
                                    const unsigned long rightchild = parent*2+2;
                                    unsigned long target = parent;
                                    target = (leftchild < TIMERS && m_ActiveTimerInfoList[leftchild]->m_TargetTime < m_ActiveTimerInfoList[target]->m_TargetTime?
                                                  leftchild:
                                                  target);
                                    target = (rightchild < TIMERS && m_ActiveTimerInfoList[rightchild]->m_TargetTime < m_ActiveTimerInfoList[target]->m_TargetTime?
                                                  rightchild:
                                                  target);
                                    if(parent == target)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        TimerInfo* task = m_ActiveTimerInfoList[target];
                                        m_ActiveTimerInfoList[target] = m_ActiveTimerInfoList[parent];
                                        m_ActiveTimerInfoList[parent] = task;
                                        parent = target;
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::this_thread::sleep_for (std::chrono::seconds(0));
                            continue;
                        }
#else
                        if(m_ActiveTimerInfoList.size() == 0)
                        {
                            while(m_ActiveTimerInfoList.size() == 0 && m_Running)
                            {
                                m_Condition.wait(ActiveTimerInfoListLock);
                            }
                            if(!m_Running)
                            {
                                return;
                            }
                        }
                        while(m_ActiveTimerInfoList[0]->m_TargetTime > std::chrono::steady_clock::now() && m_Running)
                        {
                            m_Condition.wait_for(ActiveTimerInfoListLock, m_ActiveTimerInfoList[0]->m_TargetTime - std::chrono::steady_clock::now());
                        }
                        if(m_Running == false)
                        {
                            return;
                        }
                        task = m_ActiveTimerInfoList[0];
                        m_ActiveTimerInfoList[0] = m_ActiveTimerInfoList.back();
                        m_ActiveTimerInfoList.pop_back();
                        const unsigned long TIMERS = m_ActiveTimerInfoList.size();
                        // re-arrang heap
                        {
                            unsigned long parent = 0;
                            while(parent < TIMERS)
                            {
                                const unsigned long leftchild = parent*2+1;
                                const unsigned long rightchild = parent*2+2;
                                unsigned long target = parent;
                                target = (leftchild < TIMERS && m_ActiveTimerInfoList[leftchild]->m_TargetTime < m_ActiveTimerInfoList[target]->m_TargetTime?
                                              leftchild:
                                              target);
                                target = (rightchild < TIMERS && m_ActiveTimerInfoList[rightchild]->m_TargetTime < m_ActiveTimerInfoList[target]->m_TargetTime?
                                              rightchild:
                                              target);
                                if(parent == target)
                                {
                                    break;
                                }
                                else
                                {
                                    TimerInfo* task = m_ActiveTimerInfoList[target];
                                    m_ActiveTimerInfoList[target] = m_ActiveTimerInfoList[parent];
                                    m_ActiveTimerInfoList[parent] = task;
                                    parent = target;
                                }
                            }
                        }
#endif
                    }
                    if(task)
                    {
                        m_ThreadPool.Enqueue([this, task](){
                            if(task->m_TimeoutHandler)
                            {
                                task->m_TimeoutHandler();
                            }
                            delete task;
                        }, task->m_Priority);
                    }
                }
            });
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout<<ex.what()<<std::endl;
        }
        std::cout<<"SingleShotTimer is started"<<std::endl;
    }

    void Stop()
    {
        std::unique_lock<std::mutex> APIlock(m_APILock);
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
        {
            std::unique_lock<std::mutex> ActiveTimerInfoListLock(m_ActiveTimerInfoListLock);
            for(unsigned long i = 0 ; i < m_ActiveTimerInfoList.size() ; i++)
            {
                delete m_ActiveTimerInfoList[i];
            }
            m_ActiveTimerInfoList.clear();
        }
        std::cout<<"SingleShotTimer is stopped"<<std::endl;
    }
    
    size_t Timers()
    {
        std::unique_lock<std::mutex> ActiveTimerInfoListLock(m_ActiveTimerInfoListLock);
        return m_ActiveTimerInfoList.size();
    }
};

#endif
