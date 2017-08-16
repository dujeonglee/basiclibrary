#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_
#include "ThreadPool.h"
#include <chrono>
#include <ctime>
#include <algorithm>
//#define BUSYWAITING

#ifdef __linux__ 
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION >= 40800
typedef std::chrono::steady_clock CLOCK;
#else
typedef std::chrono::monotonic_clock CLOCK;
#endif
#elif _WIN32
typedef std::chrono::steady_clock CLOCK;
#else
typedef std::chrono::steady_clock CLOCK;
#endif

typedef std::chrono::time_point<CLOCK> SSTTime;

class TimerInfo
{
public:
    SSTTime m_TargetTime;
    std::function <void()> m_TimeoutHandler;
    uint32_t m_Priority;
    uint32_t m_TimerID;
    bool m_Active;
};
template <uint32_t PRIORITY, uint32_t CONCURRENCY>
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
    uint32_t m_TimerID;
    #define INVALID_TIMER_ID    ((uint32_t)0)
    #define IMMEDIATE_TIMER_ID  ((uint32_t)1)
    #define MINIMUM_TIMER_ID    ((uint32_t)2)
public:
    SingleShotTimer()
    {
        m_Running = false;
        m_TimerID = MINIMUM_TIMER_ID;
        Start();
    }

    ~SingleShotTimer()
    {
        Stop();
    }

    uint32_t ImmediateTaskNoExcept(std::function <void()> to, uint32_t priority = 0)
    {
        uint32_t ret = INVALID_TIMER_ID;
        while (ret == INVALID_TIMER_ID && m_Running)
        {
            ret = ImmediateTask(to, priority);
        }
        return ret;
    }

    uint32_t ScheduleTaskNoExcept(uint32_t milli, std::function <void()> to, uint32_t priority = 0)
    {
        uint32_t ret = INVALID_TIMER_ID;
        while (ret == INVALID_TIMER_ID && m_Running)
        {
            ret = ScheduleTask(milli, to, priority);
        }
        return ret;
    }
    
    uint32_t ImmediateTask(std::function <void()> to, uint32_t priority = 0)
    {
        return ScheduleTask(0, to, priority);
    }

    uint32_t ScheduleTask(uint32_t milli, std::function <void()> to, uint32_t priority = 0)
    {
        std::lock_guard<std::mutex> APILock(m_APILock);
        if(m_Running == false)
        {
            return INVALID_TIMER_ID;
        }
        if(milli == 0)
        {
            return (m_ThreadPool.Enqueue([to](){to();}, priority) ? 
                IMMEDIATE_TIMER_ID : 
                INVALID_TIMER_ID); /*One cannot cancel immediate task*/
        }
        TimerInfo* newone = nullptr;
        // 1. Get a free TimerInfo
        try
        {
            newone = new TimerInfo();
        }
        catch(const std::bad_alloc& ex)
        {
            return INVALID_TIMER_ID;
        }
        // 2. Setup TimerInfo
        newone->m_TargetTime = CLOCK::now() + std::chrono::milliseconds(milli);
        newone->m_TimeoutHandler = to;
        newone->m_Priority = priority;
        const uint32_t TID = newone->m_TimerID = m_TimerID++;
        if(m_TimerID < MINIMUM_TIMER_ID )
        {
            m_TimerID = MINIMUM_TIMER_ID;
        }
        newone->m_Active = true;

        // 3. Push TimerInfo into ActiveTimerList, which is min heap.
        {
            std::lock_guard<std::mutex> ActiveTimerInfoListLock(m_ActiveTimerInfoListLock);
            try
            {
                m_ActiveTimerInfoList.push_back(newone);
            }
            catch(std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
                delete newone;
                return INVALID_TIMER_ID;
            }
            std::push_heap(m_ActiveTimerInfoList.begin(), m_ActiveTimerInfoList.end(), [](TimerInfo* &a, TimerInfo* &b)->bool{
                return a->m_TargetTime > b->m_TargetTime;
            });
        }
        m_Condition.notify_one();
        return TID;
    }

    void CancelTask(const uint32_t timerid)
    {
        std::lock_guard<std::mutex> APILock(m_APILock);
        if(m_Running == false)
        {
            return;
        }
        if(timerid < MINIMUM_TIMER_ID)
        {
            return;
        }
        std::lock_guard<std::mutex> ActiveTimerInfoListLock(m_ActiveTimerInfoListLock);
        for(uint32_t i = 0 ; i < m_ActiveTimerInfoList.size() ; i++)
        {
            if(m_ActiveTimerInfoList[i]->m_TimerID == timerid)
            {
                m_ActiveTimerInfoList[i]->m_Active = false;
                return;
            }
        }
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
            m_ThreadPool.Start();
            SingleShotTimer* const self = this;
            m_Thread = new std::thread([self]()
            {
                while(self->m_Running)
                {
                    TimerInfo* task = nullptr;
                    {
                        std::unique_lock<std::mutex> ActiveTimerInfoListLock(self->m_ActiveTimerInfoListLock);
                        if(self->m_ActiveTimerInfoList.size() == 0)
                        {
                            while(self->m_ActiveTimerInfoList.size() == 0 && self->m_Running)
                            {
                                self->m_Condition.wait(ActiveTimerInfoListLock);
                            }
                        }
                        if(!self->m_Running)
                        {
                            return;
                        }
#ifdef BUSYWAITING
                        if(self->m_ActiveTimerInfoList[0]->m_TargetTime <= CLOCK::now() && self->m_Running)
                        {
                            std::pop_heap(self->m_ActiveTimerInfoList.begin(), self->m_ActiveTimerInfoList.end(), [](TimerInfo* &a, TimerInfo* &b)->bool{
                                return a->m_TargetTime > b->m_TargetTime;
                            });
                            task = self->m_ActiveTimerInfoList.back();
                            self->m_ActiveTimerInfoList.pop_back();
                        }
                        else
                        {
                            if(self->m_Running == false)
                            {
                                return;
                            }
                            std::this_thread::sleep_for (std::chrono::microseconds(100));
                            continue;
                        }
#else
                        while(self->m_ActiveTimerInfoList[0]->m_TargetTime > CLOCK::now() && self->m_Running)
                        {
                            self->m_Condition.wait_for(ActiveTimerInfoListLock, self->m_ActiveTimerInfoList[0]->m_TargetTime - CLOCK::now());
                        }
                        if(self->m_Running == false)
                        {
                            return;
                        }
                        std::pop_heap(self->m_ActiveTimerInfoList.begin(), self->m_ActiveTimerInfoList.end(), [](TimerInfo* &a, TimerInfo* &b)->bool{
                            return a->m_TargetTime > b->m_TargetTime;
                        });
                        task = self->m_ActiveTimerInfoList.back();
                        self->m_ActiveTimerInfoList.pop_back();
#endif
                    }
                    if(task && task->m_Active)
                    {
                        self->m_ThreadPool.Enqueue([task](){
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
            for(uint32_t i = 0 ; i < m_ActiveTimerInfoList.size() ; i++)
            {
                delete m_ActiveTimerInfoList[i];
            }
            m_ActiveTimerInfoList.clear();
        }
        m_ThreadPool.Stop();
        std::cout<<"SingleShotTimer is stopped"<<std::endl;
    }

    size_t Timers()
    {
        std::unique_lock<std::mutex> ActiveTimerInfoListLock(m_ActiveTimerInfoListLock);
        return m_ActiveTimerInfoList.size();
    }

    bool Running()
    {
        return m_Running;
    }
};
#undef GCC_VERSION
#endif
