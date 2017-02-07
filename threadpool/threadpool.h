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
            DESTROY,
            COMPLETE_ALL_TASK_AND_DESTROY
        };
    private:
        ThreadPool* _pool;
        std::thread* _thread;
        std::atomic<STATE> _state;
        bool _should_wakeup()
        {
            for(unsigned long i = 0 ; i < PRIORITY_LEVEL ; i++)
            {
                if(!_pool->_task_queue[i].empty())
                {
                    return true;
                }
            }
            if(_state != STATE::RUNNING)
            {
                return true;
            }
            return false;
        }

    public:
        WorkerThread(ThreadPool* pool) : _pool(pool), _state(STATE::RUNNING)
        {
            _thread = new std::thread([this](){
                while(PRIORITY_LEVEL != _pool->_task_queue.size());
                for(;;)
                {
                    std::function<void()> task = nullptr;
                    {
                        std::unique_lock<std::mutex> lock(_pool->_task_queue_lock);
                        while(!_should_wakeup())
                            _pool->_condition.wait(lock);
                        if(this->_state == STATE::DESTROY)
                        {
                            return;
                        }
                        if((this->_state == STATE::COMPLETE_ALL_TASK_AND_DESTROY) && (_pool->_total_tasks == 0))
                        {
                            return;
                        }
                        for(unsigned long priority = 0 ; priority < PRIORITY_LEVEL ; priority++)
                        {
                            if(!_pool->_task_queue[priority].empty())
                            {
                                task = std::move(_pool->_task_queue[priority].front());
                                _pool->_task_queue[priority].pop();
                                _pool->_total_tasks--;
                                break;
                            }
                        }
                    }
                    if(task != nullptr)
                        task();
                }
            });
        }

        ~WorkerThread()
        {
            _pool->_condition.notify_all();
            _thread->join();
        }

        STATE state()
        {
            return _state.load();
        }

        void state(STATE s)
        {
            _state = s;
        }
    };
private:
    std::queue < std::unique_ptr< WorkerThread > > _worker_queue;
    std::mutex _worker_queue_lock;

    std::vector< std::queue < std::function<void()> > > _task_queue;
    unsigned long _total_tasks;
    std::mutex _task_queue_lock;
    std::condition_variable _condition;
public:
    ThreadPool()
    {
        std::unique_lock<std::mutex> lock(_worker_queue_lock);
        for(unsigned int i = 0 ; i < (INITIAL_THREADS>1?INITIAL_THREADS:1) ; i++)
        {
            try
            {
                _worker_queue.emplace(new WorkerThread(this));
            }
            catch(const std::bad_alloc &ex)
            {
                std::cout<<ex.what()<<std::endl;
                break;
            }
        }
        try
        {
            _task_queue.resize((PRIORITY_LEVEL>1?PRIORITY_LEVEL:1));
        }
        catch(std::bad_alloc& ex)
        {
            std::cout<<ex.what()<<std::endl;
        }
        std::cout<<_worker_queue.size()<<" threads with ";
        std::cout<<_task_queue.size()<<" priorities\n";
        _total_tasks = 0;
    }

    ~ThreadPool()
    {
        destroy();
    }

    void resize(size_t size)
    {
        if(size == 0)
        {
            return;
        }
        std::unique_lock<std::mutex> lock(_worker_queue_lock);
        if(size < _worker_queue.size())
        {
            while(size < _worker_queue.size())
            {
                _worker_queue.front()->state(WorkerThread::DESTROY);
                _worker_queue.pop();
            }
        }
        else if(size > _worker_queue.size())
        {
            while(size > _worker_queue.size())
            {
                try
                {
                    _worker_queue.emplace(new WorkerThread(this));
                }
                catch(const std::bad_alloc &ex)
                {
                    std::cout<<ex.what()<<std::endl;
                    break;
                }
            }
        }
    }

    void destroy()
    {
        std::unique_lock<std::mutex> lock(_worker_queue_lock);
        while(!_worker_queue.empty())
        {
            _worker_queue.front()->state(WorkerThread::COMPLETE_ALL_TASK_AND_DESTROY);
            _worker_queue.pop();
        }
    }

    void enqueue(std::function<void()> task, const unsigned long priority = 0)
    {
        {
            std::unique_lock<std::mutex> lock(_worker_queue_lock);
            if((_worker_queue.empty() == true) || (_worker_queue.front()->state() != WorkerThread::RUNNING))
            {
                return;
            }
        }
        {
            std::unique_lock<std::mutex> lock(_task_queue_lock);
            try
            {
                _task_queue[priority].push(task);
                _total_tasks++;
                _condition.notify_one();
            }
            catch(std::bad_alloc& ex)
            {
                std::cout<<ex.what()<<std::endl;
            }
        }
    }

    size_t tasks()
    {
        std::unique_lock<std::mutex> lock(_task_queue_lock);
        return _total_tasks;
    }

    size_t tasks(const unsigned long priority)
    {
        if(priority < PRIORITY_LEVEL)
        {
            std::unique_lock<std::mutex> lock(_task_queue_lock);
            return _task_queue[priority].size();
        }
    }
};

#endif