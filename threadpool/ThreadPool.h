#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <atomic>               // std::atomic
#include <condition_variable>   // std::condition_variable
#include <functional>           // std::function
#include <iostream>             // std::cout
#include <mutex>                // std::mutex, std::unique_lock
#include <thread>               // std::thread
#include <queue>                // std::queue
#include <vector>               // std::vector

template <unsigned long PRIORITY_LEVEL, unsigned long INITIAL_THREADS>
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
    std::vector< std::queue < std::function<void()> > > m_TaskQueue;

    // Synchronization of m_TaskQueue
    std::mutex m_TaskQueueLock;
    std::condition_variable m_Condition;

    // Number of workers
    std::atomic< size_t > m_Workers;

    // Number of active workers, i.e., currently serving tasks.
    std::atomic< size_t > m_ActiveWorkers;

    // API call synchronization among applications.
    std::mutex m_APILock;
private:

    // Create one thread.
    void HireWorker()
    {
        ThreadPool * const self = this;
        std::thread Worker = std::thread([self]()
        {
            self->m_Workers++;
            for(;;)
            {
                std::function<void()> task = nullptr;
                {
                    std::unique_lock<std::mutex> TaskQueueLock(self->m_TaskQueueLock);
                    while(!self->ShouldWakeup())
                    {
                        self->m_Condition.wait(TaskQueueLock);
                    }
                    for(unsigned long priority = 0 ; priority < PRIORITY_LEVEL ; priority++)
                    {
                        if(!self->m_TaskQueue[priority].empty())
                        {
                            task = std::move(self->m_TaskQueue[priority].front());
                            self->m_TaskQueue[priority].pop();
                            break;
                        }
                    }
                }
                if(task)
                {
                    self->m_ActiveWorkers++;
                    task();
                    self->m_ActiveWorkers--;
                    std::this_thread::sleep_for(std::chrono::milliseconds(0));
                }
                else
                {
                    // "null task" is used as a fire worker signal.
                    // Any threads getting "null task" will exit the loop.
                    break;
                }
            }
            self->m_Workers--;
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
                m_TaskQueue[0].push(nullptr);
                m_Condition.notify_one();
                return;
            }
            catch(std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
            }
        }
        while (1);
    }
public:
    ThreadPool() : m_State(STOPPED)
    {
        Start();
    }

    ~ThreadPool()
    {
        Stop();
    }

    bool ShouldWakeup()
    {
        for(unsigned long i = 0 ; i < PRIORITY_LEVEL ; i++)
        {
            if(!m_TaskQueue[i].empty())
            {
                return true;
            }
        }
        return false;
    }

    size_t ResizeWorkerQueue(const size_t size)
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(size == 0)
        {
            // Invalid size
            // Return current size
            return m_Workers;
        }
        if(size == m_Workers)
        {
            return m_Workers;
        }
        if(size > m_Workers)
        {
            const size_t hire = size - m_Workers;
            for(uint32_t i = 0 ; i < hire ; i++)
            {
                HireWorker();
            }
        }
        else
        {
            const size_t fire = m_Workers - size;
            for(uint32_t i = 0 ; i < fire ; i++)
            {
                FireWorker();
            }
        }
        while(m_Workers != size)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        return m_Workers;
    }

    bool Enqueue(const std::function<void()> task, const unsigned long priority = 0)
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == STOPPED)
        {
            return false;
        }
        if(priority >= PRIORITY_LEVEL)
        {
            return false;
        }
        if(task == nullptr)
        {
            return false;
        }
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            try
            {
                m_TaskQueue[priority].push(task);
                m_Condition.notify_one();
            }
            catch(std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
                return false;
            }
        }
        return true;
    }

    size_t Tasks()
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        size_t ret = 0;
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            for(size_t i = 0 ; i < m_TaskQueue.size() ; i++)
            {
                ret += m_TaskQueue[i].size();
            }
        }
        return ret;
    }

    size_t Tasks(const unsigned long priority)
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        size_t ret = 0;
        if(priority >= PRIORITY_LEVEL)
        {
            return 0;
        }
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            ret = m_TaskQueue[priority].size();
        }
        return ret;
    }

    size_t ActiveWorkers()
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        return m_ActiveWorkers;
    }

    void Start()
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == POOL_STATE::STARTED)
        {
            return;
        }
        m_State = POOL_STATE::STARTED;
        // 1. Create TaskQueue
        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            try
            {
                m_TaskQueue.resize((PRIORITY_LEVEL>1?PRIORITY_LEVEL:1));
            }
            catch(std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
            }
        }

        // 2. Create workers
        m_Workers = 0;
        for(unsigned int i = 0 ; i < (INITIAL_THREADS>1?INITIAL_THREADS:1) ; i++)
        {
            HireWorker();
        }
        m_ActiveWorkers = 0;
    }

    void Stop()
    {
        std::unique_lock<std::mutex> ApiLock(m_APILock);
        if(m_State == STOPPED)
        {
            return;
        }
        m_State = STOPPED;

        const size_t currentworkers = m_Workers;
        {
            std::queue< std::function< void() > > empty;
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            std::swap(m_TaskQueue[0], empty);
        }
        for(size_t i = 0 ; i < currentworkers ; i++)
        {
            FireWorker();
        }
        while(m_Workers > 0);

        {
            std::unique_lock<std::mutex> TaskQueueLock(m_TaskQueueLock);
            m_TaskQueue.clear();
        }
    }
};

#endif
