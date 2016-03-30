#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_
#include <thread>
#include <mutex>

typedef void (*timeout_handler)();

class singleshottimer{
private:
    std::timed_mutex _lock;
    unsigned int _timeout;
    timeout_handler _handler;
    std::thread _thread;

public:
    explicit singleshottimer(){};
    ~singleshottimer(){};
    void start(unsigned int timeout_milles, timeout_handler handler){
        _timeout = timeout_milles;
        _handler = handler;
        _thread = std::thread([&](){
            this->_lock.lock();
            if(this->_lock.try_lock_for(std::chrono::milliseconds(this->_timeout)) == false){
                this->_lock.unlock();
                this->_handler();
            }
            return;
        });
    }

    void stop(){
        _lock.unlock();
    }
};

#endif
