#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <atomic>               // std::atomic
#include <condition_variable>   // std::condition_variable
#include <functional>           // std::function
#include <iostream>             // std::cout
#include <mutex>                // std::mutex, std::unique_lock
#include <thread>               // std::thread
#include <deque>                // std::queue
#include <vector>               // std::vector

template <const uint32_t PRIORITY_LEVEL, const uint32_t INITIAL_THREADS>
class ThreadPool
{
private:
    // State of ThreadPool 
    enum POOL_STATE: unsigned char
    {
        STARTED,
        STOPPED
    };
    POOL_STATE m_State;

    // Task list
    std::mutex m_TaskQueueLock;
    std::condition_variable m_Condition;
    std::vector< std::deque < std::function<void()> > > m_TaskQueue;

    uint32_t m_Priorities;

    // Number of workers
    uint32_t m_Workers;
    std::atomic< uint32_t > m_ActualWorkers;

    // API call synchronization among applications.
    std::mutex m_APILock;
private:
    bool ShouldWakeup()
    {
        for(uint32_t i = 0 ; i < m_TaskQueue.size() ; i++)
        {
            if(m_TaskQueue[i].size() > 0)
            {
                return true;
            }
        }
        return false;
    }    // Create one thread.

    void HireWorker()
    {
        ThreadPool * const self = this;
        std::thread Worker = std::thread([self]()
        {
            self->m_ActualWorkers++;
            for(;;)
            {
                std::function<void()> task = nullptr;
                {
                    std::unique_lock<std::mutex> TaskQueueLock(self->m_TaskQueueLock);
                    while(!self->ShouldWakeup())
                    {
                        self->m_Condition.wait(TaskQueueLock);
                    }
                    for(uint32_t priority = 0 ; priority < self->m_TaskQueue.size() ; priority++)
                    {
                        if(self->m_TaskQueue[priority].size() > 0)
                        {
                            task = std::move(self->m_TaskQueue[priority].front());
                            self->m_TaskQueue[priority].pop_front();
                            break;
                        }
                    }
                }
                if(task)
                {
                    task();
                    std::this_thread::sleep_for(std::chrono::milliseconds(0));
                }
                else
                {
                    // "null task" is used as a fire worker signal.
                    // Any threads getting "null task" will exit the loop.
                    break;
                }
            }
            self->m_ActualWorkers--;
        });
        Worker.detach();
    }

    // Purge one thread
    void FireWorker()
    {
        std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
        do
        {
            try
            {
                m_TaskQueue[0].push_front(nullptr);
                m_Condition.notify_one();
                return;
            }
            catch(const std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
            }
        }
        while (1);
    }
public:
    ThreadPool() : 
    m_State(STOPPED), 
    m_Priorities(PRIORITY_LEVEL>1?PRIORITY_LEVEL:1), 
    m_Workers((INITIAL_THREADS>1?INITIAL_THREADS:1))
    {
        m_ActualWorkers = 0;
        Start();
    }

    ~ThreadPool()
    {
        Stop();
    }

    void Start()
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == POOL_STATE::STARTED)
        {
            return;
        }
        // 1. Create TaskQueue
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            try
            {
                m_TaskQueue.resize(m_Priorities);
            }
            catch(const std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
                m_Priorities = m_TaskQueue.size();
            }

        }

        // 2. Create workers
        for(unsigned int i = 0 ; i < m_Workers ; i++)
        {
            HireWorker();
        }
        while(m_ActualWorkers != m_Workers)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        m_State = POOL_STATE::STARTED;
    }

    void Stop()
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == STOPPED)
        {
            return;
        }
        for(uint32_t i = 0 ; i < m_Workers ; i++)
        {
            FireWorker();
        }
        while(m_ActualWorkers > 0);
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            m_TaskQueue.clear();
        }
        m_State = STOPPED;
    }

    uint32_t SetPriorities(const uint32_t size)
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == STOPPED)
        {
            m_Priorities = size;
            return m_Priorities;
        }
        if(size == 0 || size == m_Priorities)
        {
            return m_Priorities;
        }
        else
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            try
            {
                m_TaskQueue.resize(m_Priorities);
                m_Priorities = size;
            }
            catch(const std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
            }
            return m_Priorities;
        }
    }

    uint32_t SetWorkers(const uint32_t size)
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == STOPPED)
        {
            m_Workers = size;
            return m_Workers;
        }
        if(size == 0 || size == m_Workers)
        {
            return m_Workers;
        }
        if(size > m_Workers)
        {
            const uint32_t hire = size - m_Workers;
            for(uint32_t i = 0 ; i < hire ; i++)
            {
                HireWorker();
            }
            m_Workers += hire;
        }
        else
        {
            const uint32_t fire = m_Workers - size;
            for(uint32_t i = 0 ; i < fire ; i++)
            {
                FireWorker();
            }
            m_Workers -= fire;
        }
        while(m_ActualWorkers != m_Workers)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        return m_Workers;
    }

    bool Enqueue(const std::function<void()> task, const uint32_t priority = 0)
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == STOPPED)
        {
            return false;
        }
        if(task == nullptr)
        {
            return false;
        }
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            if(priority >= m_TaskQueue.size())
            {
                return false;
            }
            try
            {
                m_TaskQueue[priority].push_back(task);
                m_Condition.notify_one();
            }
            catch(const std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
                return false;
            }
        }
        return true;
    }

    uint32_t Tasks()
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        uint32_t ret = 0;
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            for(uint32_t i = 0 ; i < m_TaskQueue.size() ; i++)
            {
                ret += m_TaskQueue[i].size();
            }
        }
        return ret;
    }

    uint32_t Tasks(const uint32_t priority)
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            if(priority >= m_TaskQueue.size())
            {
                return 0;
            }
            return m_TaskQueue[priority].size();
        }
    }
};

#endif
