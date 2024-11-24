// ===========================================================================
// Spin Locks // SpinLock.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
// #include <print>
#include <mutex>

namespace SpinLocks {

    class Spinlock
    {
    private:
        std::atomic_flag m_atomic_flag;    // = ATOMIC_FLAG_INIT;

    public:
        Spinlock() : m_atomic_flag{} {}    // ATOMIC_FLAG_INIT

        void lock()
        {
            while (m_atomic_flag.test_and_set(std::memory_order_acquire))
            {
            }
        }

        void unlock()
        {
            m_atomic_flag.clear(std::memory_order_release);
        }
    };
}

namespace TestSpinLocksCommon
{
    const size_t MaxIterations = 1000000;
    const size_t NumWorkers = 10;

    using ValueType = unsigned long;

    volatile ValueType value{};

    std::atomic<ValueType> valueAtomic;

    std::mutex mutex;
}

namespace TestUsingSpinLocks
{
    using namespace TestSpinLocksCommon;
    using namespace SpinLocks;

    Spinlock spinlock;

    static void task(size_t iterations)
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "Task ", tid, " started ...");

        for (size_t i{}; i != iterations; ++i)
        {
            spinlock.lock();
            value++;
            spinlock.unlock();
        }
    }
}

namespace TestUsingAtomics
{
    using namespace TestSpinLocksCommon;

    static void atomicTask(size_t iterations)
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "Task ", tid, " started ...");

        for (size_t i{}; i != iterations; ++i)
        {
            valueAtomic++;
        }
    }
}

namespace TestUsingMutex
{
    using namespace TestSpinLocksCommon;

    static void mutexTask(size_t iterations)
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "Task ", tid, " started ...");

        for (size_t i{}; i != iterations; ++i)
        {
            mutex.lock();
            value++;
            mutex.unlock();
        }
    }
}

void test_using_spinlocks() {

    using namespace SpinLocks;
    using namespace TestUsingSpinLocks;

    std::vector<std::thread> threads;

    Logger::log(std::cout, "SpinLock test started");
    Logger::log(std::cout, "Iterations: ", MaxIterations);
    
    value = 0;
    
    {
        ScopedTimer watch{};

        for (size_t i{}; i != NumWorkers; ++i) {
            std::thread thread{ task, MaxIterations };
            threads.push_back(std::move(thread));
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    Logger::log(std::cout, "Threads done.");

    if (value == MaxIterations * NumWorkers)
    {
        Logger::log(std::cout, "Test passed successfully!");
    }
    else
    {
        Logger::log(std::cout, "Test FAILED!");
    }
}

void test_using_atomics() {

    using namespace TestSpinLocksCommon;
    using namespace TestUsingAtomics;

    std::vector<std::thread> threads;

    Logger::log(std::cout, "std::atomic<> test started");
    Logger::log(std::cout, "Iterations: ", MaxIterations);

    valueAtomic = 0;

    {
        ScopedTimer watch{};

        for (size_t i{}; i != NumWorkers; ++i) {
            std::thread thread{ atomicTask, MaxIterations };
            threads.push_back(std::move(thread));
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    Logger::log(std::cout, "Threads done.");

    ValueType result = valueAtomic;

    if (result == MaxIterations * NumWorkers)
    {
        Logger::log(std::cout, "Test passed successfully!");
    }
    else
    {
        Logger::log(std::cout, "Test FAILED!");
    }
}

void test_using_mutex() {

    using namespace TestSpinLocksCommon;
    using namespace TestUsingMutex;

    std::vector<std::thread> threads;

    Logger::log(std::cout, "std::mutex test started");
    Logger::log(std::cout, "Iterations: ", MaxIterations);

    value = 0;

    {
        ScopedTimer watch{};

        for (size_t i{}; i != NumWorkers; ++i) {
            std::thread thread{ mutexTask, MaxIterations };
            threads.push_back(std::move(thread));
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    Logger::log(std::cout, "Threads done.");

    if (value == MaxIterations * NumWorkers)
    {
        Logger::log(std::cout, "Test passed successfully!");
    }
    else
    {
        Logger::log(std::cout, "Test FAILED!");
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
