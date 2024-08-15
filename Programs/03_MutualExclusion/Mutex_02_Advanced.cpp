// ===========================================================================
// Mutex_02_Advanced.cpp // Locking Mechanisms
// ===========================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Mutex_And_Locking_Examples {

    // std::mutex object to be used by *all* code below
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
            // 'g_mutex.lock()' is automatically called
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

        // basic std::unique_lock usage

        {
            // 'g_mutex.lock()' is automatically called
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
            // 'g_mutex.lock()' is automatically called
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
   
            // 'g_mutex.unlock()' is automatically called here
            //  at the destruction of the 'std::unique_lock' 
        }
    }

    static void example_03_03() {

        // choose to NOT automatically lock at construction

        {
            // 'g_mutex.lock()' is NOT automatically called
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

            guard.lock();
            // second critical section here
            // ...
             
            // 'g_mutex.unlock()' is automatically called
            //  at the destruction of the 'std::unique_lock' 
            // (guard may be locked or unlocked: no exception is called during destructor invocation !)
        }
    }

    // -------------------------------------
    // std::scoped_lock

    static std::mutex g_mutex1{};
    static std::mutex g_mutex2{};
    static std::mutex g_mutex3{};

    static void example_04_01() {

        // basic scoped lock usage on multiple mutexes at once
        {
            // 'lock()' is automatically simultaneously called
            // on all mutexes here at the construction of the 'std::scoped_lock'
            std::scoped_lock guard{ g_mutex1, g_mutex2, g_mutex3 };

            // critical section here
            // ...

            // 'unlock()' is automatically simultaneously called on all mutexes 
            // here at the destruction of the `std::scoped_lock` object
        }
    }

    static void example_04_02() {

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

    static void example_04_03() {

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

void test_advanced_mutex()
{
    using namespace Mutex_And_Locking_Examples;

    example_01();

    example_02();

    example_03_01();
    example_03_02();
    example_03_03();

    example_04_01();
    example_04_02();
    example_04_03();
}

// ===========================================================================
// End-of-File
// ===========================================================================
