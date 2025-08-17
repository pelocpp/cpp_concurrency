// ===========================================================================
// LockFreeProgramming02.cpp // Lock-Free Programming
// ===========================================================================

#include <atomic>
#include <print>

//#include <print>
//#include <thread>

namespace LockFreeProgramming {

    static void test_lock_free_programming_03()
    {
        std::atomic<int> value{ 123 };

        int expectedValue = 123;

        std::println("Previous expected value: {}", expectedValue);

        bool b = value.compare_exchange_weak(expectedValue, 456);

        std::println("Return Value:            {}", b);
        std::println("Current expected Value:  {}", expectedValue);
        std::println("Current Value:           {}", value.load());
    }

    //std::atomic<size_t> count;
    //… on the threads …
    //    size_t c = count.load(std::memory_order_relaxed);
    //while (!count.compare_exchange_strong(c, c + 1,
    //    std::memory_order_relaxed, std::memory_order_relaxed)) {
    //}

    void increment(std::atomic<size_t> count)
    {
        size_t value = count.load(std::memory_order_relaxed);

        while (!count.compare_exchange_weak(
            value,
            value + 1,
            std::memory_order_relaxed, 
            std::memory_order_relaxed)) 
        {
        }
    }




    static void test_lock_free_programming_04()
    {
        std::atomic<int> value{ 123 };

        int expectedValue = 124;

        std::println("Previous expected value: {}", expectedValue);

        bool b = value.compare_exchange_weak(expectedValue, 456);

        std::println("Return Value:            {}", b);
        std::println("Current expected Value:  {}", expectedValue);
        std::println("Current Value:           {}", value.load());
        std::println();
    }
}

void test_lock_free_programming() {

    using namespace LockFreeProgramming;

    test_lock_free_programming_01();
    test_lock_free_programming_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
