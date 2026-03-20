// ===========================================================================
// PrimeNumbers.cpp
// ===========================================================================

#include "../Globals/GlobalPrimes.h"

#include "PrimeCalculator.h"
#include "ThreadsafeStack.h"

#include "StrategizedLock.h"

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <cstddef>
#include <iostream>
#include <optional>
#include <vector>

#ifdef _DEBUG
static constexpr std::size_t MaxIterations = 10'000'000;     // debug
#else
static constexpr std::size_t MaxIterations = 10'000'000;     // release
#endif

void test_strategized_locking_01()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_StrategizedLock;

    NoLock lock;
    // ExclusiveLock lock;

    ThreadsafeStack<std::size_t> stack{ lock };

    Logger::log(std::cout, "Calling push ", MaxIterations, " times:");

    ScopedTimer watch{};

    for (std::size_t i = 0; i != MaxIterations; ++i) {
        stack.push(i);

        std::size_t value{};
        stack.pop(value);
    }
}

void test_strategized_locking_02()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_StrategizedLock;

    Logger::log(std::cout, "Testing RecursiveLock");

    // ExclusiveLock lock;      // crashes // need to modify pop method, calling size or empty method
    RecursiveLock lock;         // works

    ThreadsafeStack<std::size_t> stack{ lock };

    // just want to test recursive lock
    stack.push(123);
    std::size_t value{};
    stack.pop(value);

    Logger::log(std::cout, "Done.");
}

void test_strategized_locking_03()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;
    using namespace Concurrency_StrategizedLock;

    Logger::log(std::cout, "Calcalating Prime Numbers from ", PrimeNumberLimits::LowerLimit, " up to ", PrimeNumberLimits::UpperLimit, ':');

    // compare these two Lock objects // which one should be used in this example
    NoLock lock;
    // ExclusiveLock lock;

    ThreadsafeStack<std::size_t> primes{ lock };

    PrimeCalculator<std::size_t> calc{ primes, PrimeNumberLimits::LowerLimit, PrimeNumberLimits::UpperLimit + 1 };

    ScopedTimer watch{};

    std::thread calculator{ calc };
    calculator.join();

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
}

void test_strategized_locking_04()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;
    using namespace Concurrency_StrategizedLock;

    Logger::log(std::cout, "Calcalating Prime Numbers from ", PrimeNumberLimits::LowerLimit, " up to ", PrimeNumberLimits::UpperLimit, ':');

    // NoLock lock;   // crashes sporadically // Access Violation // 0xc0000005 
    ExclusiveLock lock;

    ThreadsafeStack<std::size_t> primes{ lock };

    unsigned int NumThreads = std::thread::hardware_concurrency();
    Logger::log(std::cout, "Number of concurrent threads currently supported: ", NumThreads, ':');

    std::vector<std::thread> threads;
    threads.reserve(NumThreads);

    std::size_t range = (PrimeNumberLimits::UpperLimit - PrimeNumberLimits::LowerLimit) / NumThreads;
    std::size_t start = PrimeNumberLimits::LowerLimit;
    std::size_t end = start + range;

    ScopedTimer watch{};

    // setup threads
    for (std::size_t i{}; i != NumThreads - 1; ++i) {

        PrimeCalculator<std::size_t> calc{ primes, start, end };
        threads.emplace_back(calc);

        start = end;
        end = start + range;
    }

    // setup last thread
    end = PrimeNumberLimits::UpperLimit;
    PrimeCalculator<std::size_t> calc{ primes, start, end + 1 };
    threads.emplace_back(calc);

    // wait for end of all threads
    for (std::size_t i{}; i != NumThreads; ++i) {
        threads[i].join();
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
