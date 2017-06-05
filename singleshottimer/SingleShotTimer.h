#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_
#include "ThreadPool.h"
#include <chrono>
#include <ctime>
#include <algorithm>
//#define BUSYWAITING

#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#if GCC_VERSION >= 40800
typedef std::chrono::steady_clock CLOCK;
#else
typedef std::chrono::monotonic_clock CLOCK;
#endif

typedef std::chrono::time_point<CLOCK> SSTTime;

class TimerInfo
{
public:
    SSTTime m_TargetTime;
    std::function <void()> m_TimeoutHandler;
    unsigned long m_Priority;
    void PrintTime()
    {
        std::time_t ttp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + (m_TargetTime - CLOCK::now()));
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
        newone->m_TargetTime = CLOCK::now() + std::chrono::milliseconds(milli);
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
        std::push_heap(m_ActiveTimerInfoList.begin(), m_ActiveTimerInfoList.end(), [](TimerInfo* &a, TimerInfo* &b)->bool{
            return a->m_TargetTime > b->m_TargetTime;
        });
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
                    if(task)
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
#undef GCC_VERSION
#endif
