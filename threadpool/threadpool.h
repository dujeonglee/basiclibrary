#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <atomic>		// std::atomic
#include <condition_variable>	// std::condition_variable
#include <functional>		// std::function
#include <iostream>		// std::cout
#include <mutex>		// std::mutex, std::unique_lock
#include <thread>		// std::thread
#include <queue>                // std::queue


class ThreadPool
{
    class WorkerThread
    {
        enum STATE : unsigned char{
            RUNNING = 0,
            DESTROY,
        };
    private:
        ThreadPool* _pool;
        std::thread* _thread;
        std::atomic<STATE> _state;
        bool _should_wakeup()
        {
            return  ((!_pool->_task_queue.empty()) || (_state != STATE::RUNNING));
        }

    public:
        WorkerThread(ThreadPool* pool) : _pool(pool), _state(STATE::RUNNING)
        {
            _thread = new std::thread([this](){
                std::cout<<std::this_thread::get_id()<<"Thread Create\n";
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
                        if(!_pool->_task_queue.empty())
                        {
                            task = std::move(_pool->_task_queue.front());
                            _pool->_task_queue.pop();
                        }
                    }
                    if(task != nullptr)
                        task();
                }
            });
        }

        ~WorkerThread(){
            _state = STATE::DESTROY;
            _pool->_condition.notify_all();
            _thread->join();
        }
    };
private:
    std::queue < std::unique_ptr< WorkerThread > > _worker_queue;
    std::mutex _worker_queue_lock;

    std::queue < std::function<void()> > _task_queue;
    std::mutex _task_queue_lock;
    std::condition_variable _condition;
public:
    ThreadPool()
    {
        std::unique_lock<std::mutex> lock(_worker_queue_lock);
        for(unsigned i = 0 ; i < std::thread::hardware_concurrency() ; i++)
        {
            _worker_queue.emplace(new WorkerThread(this));
        }
        std::cout<<_worker_queue.size()<<" threads\n";
    }

    ThreadPool(size_t size)
    {
        std::unique_lock<std::mutex> lock(_worker_queue_lock);
        for(size_t i = 0 ; i < size ; i++)
        {
            _worker_queue.emplace(new WorkerThread(this));
        }
        std::cout<<_worker_queue.size()<<" threads\n";
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
                _worker_queue.pop();
            }
        }
        else if(size > _worker_queue.size())
        {
            while(size > _worker_queue.size())
            {
                _worker_queue.emplace(new WorkerThread(this));
            }
        }
    }

    void destroy()
    {
        std::unique_lock<std::mutex> lock(_worker_queue_lock);
        while(!_worker_queue.empty())
        {
            _worker_queue.pop();
        }
    }

    void enqueue(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(_task_queue_lock);
            _task_queue.push(task);
        }
        _condition.notify_one();
    }
};

#endif
