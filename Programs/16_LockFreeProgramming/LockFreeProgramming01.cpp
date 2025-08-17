// ===========================================================================
// LockFreeProgramming01.cpp // Lock-Free Programming
// ===========================================================================

#include <atomic>
#include <print>

//#include <print>
//#include <thread>

namespace LockFreeProgramming {

    static void test_lock_free_programming_01()
    {
        std::atomic<int> value{ 123 };

        int expectedValue = 123;

        std::println("Previous expected value: {}", expectedValue);

        bool b = value.compare_exchange_weak(expectedValue, 456);

        std::println("Return Value:            {}", b);
        std::println("Current expected Value:  {}", expectedValue);
        std::println("Current Value:           {}", value.load());
    }

    static void test_lock_free_programming_02()
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
