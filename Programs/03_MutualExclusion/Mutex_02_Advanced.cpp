// ===========================================================================
// Mutex_02_Advanced.cpp // Locking Mechanisms
// ===========================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Mutex_And_Locking_Examples {

    // Global, shared mutex to be used by *all* code
    // and threads in all examples below
    static std::mutex g_mutex{};

    // -------------------------------------
    // std::mutex

    static void example_01() {

        // ...

        g_mutex.lock();
        // critical section here
        // ...
        g_mutex.unlock();
    }

    // -------------------------------------
    // std::lock_guard

    static void example_02() {

        // ...

        {
            // 'g_mutex.lock()' is automatically called here
            // at the construction of the 'std::lock_guard'

            std::lock_guard<std::mutex> guard{ g_mutex };

            // critical section here
            // ...

            // 'g_mutex.unlock()' is automatically called here
            //  at the destruction of the 'std::lock_guard' 
        }

        // ...
    }

    // -------------------------------------
    // std::unique_lock

    static void example_03_01() {

        // basic lock_guard similar usage
        {
            // 'g_mutex.lock()' is automatically called here
            // at the construction of the 'std::unique_lock'

            std::unique_lock<std::mutex> guard{ g_mutex };

            // critical section here
            // ...

            // 'g_mutex.unlock()' is automatically called here
            //  at the destruction of the 'std::unique_lock' 
        }
    }

    static void example_03_02() {

        // multiple critical sections
        {
            // 'g_mutex.lock()' is automatically called here
            // at the construction of the 'std::unique_lock'

            std::unique_lock<std::mutex> guard{ g_mutex };
            // first critical section here
            // ...

            guard.unlock();
            // do non-critical stuff here
            // ...

            guard.lock();
            // second critical section here
            // ...

            guard.unlock();
            // do non-critical stuff again here
            // ...
            
            // 'g_mutex.unlock()' is automatically called here
            //  at the destruction of the 'std::unique_lock' 
        }
    }

    static void example_03_03() {

        // choose to NOT automatically lock at construction
        {
            // 'g_mutex.lock()' is NOT automatically called here
            // at the construction of the 'std::unique_lock'

            std::unique_lock<std::mutex> guard{ g_mutex, std::defer_lock };
            // do non-critical stuff here
            // ...

            guard.lock();
            // critical section here
            // ...

            guard.unlock();
            // do non-critical stuff here
            // ...

            // 'g_mutex.unlock()' is automatically called here
            //  at the destruction of the 'std::unique_lock' 
        }
    }

    // -------------------------------------
    // std::condition_variable

    std::condition_variable cv{};

    // data to share
    struct Data
    {
        // The producer will set this to true to indicate the data is new and has
        // not been read by a consumer yet. The consumer will reset it to false
        // once the data has been read.
        bool m_newDataAvailable;
        int m_value1;
        int m_value2;
    };

    // shared data -- to be shared between threads
    Data sharedData{ false, 0, 0 };

    static void example_04_01() {

        // ------------------------------------------
        // Thread 1: Producer
        // Usage of a 'std::condition_variable' to send a notification 
        // from a producer thread to a consumer thread whenever it is time to wake up
        // the consumer to let it work on what the producer has provided
        // via a shared memory object.
        // ------------------------------------------
        {
            std::unique_lock<std::mutex> guard{ g_mutex };

            // critical section here:
            // have unique access via the underlying mutex to
            // atomically *write to* a shared data object
            sharedData.m_newDataAvailable = true;
            sharedData.m_value1 = 123;
            sharedData.m_value2 = 456;

            // Now, immediately unlock the mutex since we are done with the critical
            // section, and notify a consumer thread **which is already waiting on 
            // the condition variable** to get it to wake up and run.
            guard.unlock();

            cv.notify_one(); // wake up just one waiting, consumer thread
        }

        // ------------------------------------------
        // Thread 2: Consumer
        // Wake up and read data from any producer thread which has sent it a 
        // notification via the condition variable.
        // Call 'cv.wait()' WITH the boolean predicate. 
        // ------------------------------------------

        {
            std::unique_lock<std::mutex> guard{ g_mutex };

            // called here at construction 
            cv.wait(guard,
                []() {
                    return sharedData.m_newDataAvailable;
                }
            );

            // 'mutex.lock()' was called prior to 'wait()' returning above too, even
            // though the mutex was necessarily **unlocked**
            //  during the sleeping/waiting period
            sharedData.m_newDataAvailable = false;  // reset our boolean "predicate" variable
            
            Data sharedDataCopy{ sharedData };      // quickly copy out the shared data

            guard.unlock();

            std::cout 
                << "Shared Data Copy: i1 = " << sharedDataCopy.m_value1 
                << ", i2 = " << sharedDataCopy.m_value2 << std::endl;
        }
    }

    // -------------------------------------
    // std::scoped_lock

    std::mutex g_mutex1{};
    std::mutex g_mutex2{};
    std::mutex g_mutex3{};

    static void example_05_01() {

        // basic scoped lock usage on multiple mutexes at once
        {
            // 'lock()' is automatically simultaneously called
            // on all mutexes here at the construction
            // of the 'std::scoped_lock'
            std::scoped_lock guard{ g_mutex1, g_mutex2, g_mutex3 };

            // critical section here
            // ...

            // 'unlock()' is automatically simultaneously called on all mutexes 
            // here at the destruction of the `std::scoped_lock` object
        }
    }

    static void example_05_02() {

        // equivalent to example above, but C++11 compatible
        {
            // 1. explicitly lock all mutexes at once
            std::lock(g_mutex1, g_mutex2, g_mutex3);

            // 2. Now pass their ownership individually to `std::lock_guard` objects
            //    to auto-unlock them at the termination of this scope.
            //    Creating a `std::lock_guard` with the `std::adopt_lock` parameter means,
            //    that the ownership of the mutex is acquired without attempting to lock it!
            std::lock_guard<std::mutex> lock1{ g_mutex1, std::adopt_lock };
            std::lock_guard<std::mutex> lock2{ g_mutex2, std::adopt_lock };
            std::lock_guard<std::mutex> lock3{ g_mutex3, std::adopt_lock };

            // critical section here
            // ...

            // 'unlock()' is automatically simultaneously called on all mutexes 
            // here at the destruction of the `std::scoped_lock` object
        }
    }

    static void example_05_03() {

        // another C++ 11 compatible example
        {
            // explicitly lock all mutexes at once
            std::lock(g_mutex1, g_mutex2, g_mutex3);

            // critical section here
            // ...

            // unlock each mutex individually now
            g_mutex1.unlock();
            g_mutex2.unlock();
            g_mutex3.unlock();
        }
    }
}

void examples()
{
    using namespace Mutex_And_Locking_Examples;

    example_01();

    example_02();

    example_03_01();
    example_03_02();
    example_03_03();

    example_04_01();

    example_05_01();
    example_05_02();
    example_05_03();
}

// ===========================================================================
// End-of-File
// ===========================================================================
