// ===========================================================================
// Simple_Condition_Variable_01.cpp // Condition Variable
// ===========================================================================

#include <thread>
#include <mutex>
#include <condition_variable>
#include <syncstream>
#include <iostream>

using namespace std::chrono_literals;

struct Resource {
    bool full = false;
    std::mutex mux;
    // Note that std::condition_variable only works with std::unique_lock<std::mutex>,
    // for other combinations use std::condition_variable_any which may be less efficient.
    std::condition_variable cond;
    void produce() {
        {
            std::unique_lock lock(mux);
            // wait until the condition is true
            // 1. the lock is released
            // 2. when the thread is woken up, the lock is reacquired and the condition checked
            // 3. if the condition is still not true, the lock is rereleased, and we go to step 2.
            // 4. if the condition is true, the wait() call finishes
            cond.wait(lock, [this] { return !full; });
            std::osyncstream(std::cout) << "Filling the resource and notifying the consumer.\n";
            full = true;
            std::this_thread::sleep_for(200ms);
        }
        // wake up one thread waiting on this condition variable
        // note that we already released our lock, otherwise
        // the notified thread would wake up and fail to acquire
        // the lock and suspend itself again
        cond.notify_one();
    }
    void consume() {
        {
            std::unique_lock lock(mux);
            // same as above, but with opposite semantics
            cond.wait(lock, [this] { return full; });
            std::osyncstream(std::cout) << "Consuming the resource and notifying the producer.\n";
            full = false;
            std::this_thread::sleep_for(200ms);
        }
        cond.notify_one();
    }
};

void test_simple_condition_variable_02()
{
    Resource resource;
    auto t1 = std::jthread([&resource](std::stop_token token) {
        while (!token.stop_requested())
            resource.produce();
        });
    auto t2 = std::jthread([&resource](std::stop_token token) {
        while (!token.stop_requested())
            resource.consume();
        });
    std::this_thread::sleep_for(5s);
    t1.request_stop();
    t2.request_stop();
    // Note: using request_stop here is unsafe.

    // If we removed the sleep_for, the t2 thread could 
    // run an entire loop before it notices the stop request 
    // and considering that t1 no longer runs, the blocking
    // condition would never be fulfilled.

    // This can be prevented by using condition_variable_any, 
    // which supports a stop token, or a timeout.
}

// ===========================================================================
// End-of-File
// ===========================================================================



