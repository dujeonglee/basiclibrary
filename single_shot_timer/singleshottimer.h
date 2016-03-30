#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_
#include <thread>
#include <mutex>

typedef void (*handler_func)();

class singleshottimer{
private:
    std::timed_mutex _lock;
    unsigned int _timeout;
    handler_func _cancel_handler;
    handler_func _timeout_handler;
    std::thread _thread;

public:
    explicit singleshottimer(){};
    ~singleshottimer(){};
    void start(unsigned int timeout_milles, handler_func cancel_handler, handler_func timeout_handler){
        _timeout = timeout_milles;
        _cancel_handler = cancel_handler;
        _timeout_handler = timeout_handler;
        _thread = std::thread([&](){
            this->_lock.lock();
            if(this->_lock.try_lock_for(std::chrono::milliseconds(this->_timeout)) == true){
                this->_cancel_handler();
            }else{
                this->_lock.unlock();
                this->_timeout_handler();
            }
            return;
        });
    }

    void cancel(){
        _lock.unlock();
    }
};

#endif
