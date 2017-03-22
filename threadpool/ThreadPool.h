#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <atomic>		// std::atomic
#include <condition_variable>	// std::condition_variable
#include <functional>		// std::function
#include <iostream>		// std::cout
#include <mutex>		// std::mutex, std::unique_lock
#include <thread>		// std::thread
#include <queue>                // std::queue
#include <vector>                // std::vector

template <unsigned long PRIORITY_LEVEL, unsigned long INITIAL_THREADS>
class ThreadPool
{
    class WorkerThread
    {
    public:
        enum STATE : unsigned char{
            RUNNING = 0,
            DESTROY
        };
    private:
        ThreadPool* m_Pool;
        std::thread* m_Thread;
        std::atomic<STATE> m_State;
        bool ShouldWakeup()
        {
            for(unsigned long i = 0 ; i < PRIORITY_LEVEL ; i++)
            {
                if(!m_Pool->m_TaskQueue[i].empty())
                {
                    return true;
                }
            }
            if(m_State != STATE::RUNNING)
            {
                return true;
            }
            return false;
        }

    public:
        WorkerThread(ThreadPool* pool) : m_Pool(pool), m_State(STATE::RUNNING)
        {
            try
            {
                m_Thread = new std::thread([this](){
                    for(;;)
                    {
                        std::function<void()> task = nullptr;
                        {
                            std::unique_lock<std::mutex> lock(m_Pool->m_TaskQueueLock);
                            while(!ShouldWakeup())
                                m_Pool->m_Condition.wait(lock);
                            if(this->m_State == STATE::DESTROY)
                            {
                                return;
                            }
                            for(unsigned long priority = 0 ; priority < PRIORITY_LEVEL ; priority++)
                            {
                                if(!m_Pool->m_TaskQueue[priority].empty())
                                {
                                    task = std::move(m_Pool->m_TaskQueue[priority].front());
                                    m_Pool->m_TaskQueue[priority].pop();
                                    break;
                                }
                            }
                        }
                        if(task != nullptr)
                        {
                            m_Pool->m_ActiveWorkers++;
                            task();
                            m_Pool->m_ActiveWorkers--;
                            std::this_thread::sleep_for(std::chrono::milliseconds(0));
                        }
                    }
                });
            }
            catch(const std::bad_alloc& ex)
            {
                throw ex;
            }
        }

        ~WorkerThread()
        {
            m_Pool->m_Condition.notify_all();
            m_Thread->join();
            delete m_Thread;
        }

        STATE state()
        {
            return m_State.load();
        }

        void state(STATE s)
        {
            m_State = s;
        }
    };
private:
    enum POOL_STATE: unsigned char
    {
        STARTED,
        STOPPED
    };
    POOL_STATE m_State;
    std::mutex m_StateLock;

    std::queue < std::unique_ptr< WorkerThread > > m_WorkerQueue;
    std::mutex m_WorkerQueueLock;

    std::vector< std::queue < std::function<void()> > > m_TaskQueue;
    std::mutex m_TaskQueueLock;
    std::atomic< size_t > m_ActiveWorkers;
    std::condition_variable m_Condition;

public:
    ThreadPool()
    {
        m_State = STOPPED;
        Start();
    }

    ~ThreadPool()
    {
        Stop();
    }

    size_t ResizeWorkerQueue(size_t size)
    {
        if(size == 0)
        {
            return 0;
        }
        std::unique_lock<std::mutex> lock(m_WorkerQueueLock);
        if(size < m_WorkerQueue.size())
        {
            while(size < m_WorkerQueue.size())
            {
                m_WorkerQueue.front()->state(WorkerThread::DESTROY);
                m_WorkerQueue.pop();
            }
        }
        else if(size > m_WorkerQueue.size())
        {
            while(size > m_WorkerQueue.size())
            {
                try
                {
                    m_WorkerQueue.emplace(std::unique_ptr< WorkerThread >(new WorkerThread(this)));
                }
                catch(const std::bad_alloc &ex)
                {
                    std::cout<<ex.what()<<std::endl;
                    break;
                }
            }
        }
        return m_WorkerQueue.size();
    }

    bool Enqueue(std::function<void()> task, const unsigned long priority = 0)
    {
        if(m_State == STOPPED)
        {
            return false;
        }
        {
            std::unique_lock<std::mutex> lock(m_TaskQueueLock);
            if(priority >= PRIORITY_LEVEL)
            {
                return false;
            }
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
        size_t ret = 0;
        std::unique_lock<std::mutex> lock(m_TaskQueueLock);
        for(size_t i = 0 ; i < m_TaskQueue.size() ; i++)
        {
            ret += m_TaskQueue.size();
        }
        return ret;
    }

    size_t Tasks(const unsigned long priority)
    {
        if(priority < PRIORITY_LEVEL)
        {
            std::unique_lock<std::mutex> lock(m_TaskQueueLock);
            return m_TaskQueue[priority].size();
        }
        return 0;
    }

    size_t ActiveWorkers()
    {
        return m_ActiveWorkers;
    }

    void Start()
    {
        std::unique_lock<std::mutex> lock(m_StateLock);
        if(m_State == POOL_STATE::STARTED)
        {
            return;
        }
        m_State = POOL_STATE::STARTED;
        {
            std::unique_lock<std::mutex> lock(m_TaskQueueLock);
            try
            {
                m_TaskQueue.resize((PRIORITY_LEVEL>1?PRIORITY_LEVEL:1));
            }
            catch(std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
            }
        }
        {
            std::unique_lock<std::mutex> lock(m_WorkerQueueLock);
            for(unsigned int i = 0 ; i < (INITIAL_THREADS>1?INITIAL_THREADS:1) ; i++)
            {
                try
                {
                    m_WorkerQueue.emplace(std::unique_ptr< WorkerThread >(new WorkerThread(this)));
                }
                catch(const std::bad_alloc &ex)
                {
                    std::cout<<ex.what()<<std::endl;
                    break;
                }
            }
        }
        std::cout<<"ThreadPool is started. ";
        std::cout<<m_WorkerQueue.size()<<" threads and ";
        std::cout<<m_TaskQueue.size()<<" priority levels\n";
        m_ActiveWorkers = 0;
    }

    void Stop()
    {
        std::unique_lock<std::mutex> lock(m_StateLock);
        if(m_State == POOL_STATE::STOPPED)
        {
            return;
        }
        m_State = POOL_STATE::STOPPED;
        {
            std::unique_lock<std::mutex> lock(m_WorkerQueueLock);
            while(!m_WorkerQueue.empty())
            {
                m_WorkerQueue.front()->state(WorkerThread::DESTROY);
                m_WorkerQueue.pop();
            }
        }
        m_TaskQueue.clear();
        std::cout<<"ThreadPool is stopped. ";
        std::cout<<m_WorkerQueue.size()<<" threads and ";
        std::cout<<Tasks()<<" tasks.\n";
    }
};

#endif
