// ===========================================================================
// Examples_LockingStrategies.cpp // `std::defer_lock` vs `std::adopt_lock`
// ===========================================================================

#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>

// https://medium.com/@back_to_basics/c-11-locking-strategy-adopt-lock-and-defer-lock-eeedf76a2689

namespace Locking_Strategies
{
    std::mutex g_mutex1{};
    std::mutex g_mutex2{};

    static void example01() {

        // calling thread locks the mutexes
        std::lock(g_mutex1, g_mutex2);

        std::lock_guard<std::mutex> lock1{ g_mutex1, std::adopt_lock };
        std::lock_guard<std::mutex> lock2{ g_mutex2, std::adopt_lock };
        
        // access shared data protected by g_mutex1 and g_mutex2

        // release of mutexes
    }

    static void example02() {

        std::unique_lock<std::mutex> lock1{ g_mutex1, std::defer_lock };
        std::unique_lock<std::mutex> lock2{ g_mutex2, std::defer_lock };

        // mutexes are locked now
        std::lock(lock1, lock2);

        // access shared data protected by the g_mutex1 and g_mutex2

        // release of mutexes
    }
}

void example_locking_strategies()
{
    using namespace Locking_Strategies;

    example01();
    example02();

}

// ===========================================================================
// End-of-File
// ===========================================================================
