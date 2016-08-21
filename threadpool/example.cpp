// condition_variable::wait (with predicate)
#include <iostream>           // std::cout
#include "threadpool.h"

int main ()
{
    ThreadPool pool(4);
    //pool.enqueue([](){std::cout<<"hey1\n";std::this_thread::sleep_for( std::chrono::seconds(3) );});
    //pool.enqueue([](){std::cout<<"hey2\n";std::this_thread::sleep_for( std::chrono::seconds(3) );});
    //pool.enqueue([](){std::cout<<"hey3\n";std::this_thread::sleep_for( std::chrono::seconds(3) );});
    //pool.enqueue([](){std::cout<<"hey4\n";std::this_thread::sleep_for( std::chrono::seconds(3) );});
    return 0;
}
