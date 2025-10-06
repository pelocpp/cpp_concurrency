// ===========================================================================
// PrimeNumbers.cpp
// ===========================================================================

#include "PrimeCalculator.h"
#include "ThreadsafeStack.h"

#include "StrategizedLock.h"

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <iostream>
#include <optional>
#include <vector>

#ifdef _DEBUG
static constexpr size_t MaxIterations = 10'000'000;     // debug
#else
static constexpr size_t MaxIterations = 10'000'000;     // release
#endif

namespace Globals
{
    // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm

    constexpr size_t LowerLimit = 1;
    constexpr size_t UpperLimit = 10'000'000;
    // Found:  664.579 prime numbers
}

void test_strategized_locking_01()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_StrategizedLock;

    NoLock lock;
    // ExclusiveLock lock;

    ThreadsafeStack<size_t> stack{ lock };

    Logger::log(std::cout, "Calling push ", MaxIterations, " times:");

    ScopedTimer watch{};

    for (size_t i = 0; i != MaxIterations; ++i) {
        stack.push(i);

        size_t value{};
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

    ThreadsafeStack<size_t> stack{ lock };

    // just want to test recursive lock
    stack.push(123);
    size_t value{};
    stack.pop(value);

    Logger::log(std::cout, "Done.");
}

void test_strategized_locking_03()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;
    using namespace Concurrency_StrategizedLock;

    Logger::log(std::cout, "Calcalating Prime Numbers from ", Globals::LowerLimit, " up to ", Globals::UpperLimit, ':');

    // compare these two Lock objects // which one should be used in this example
    NoLock lock;
    // ExclusiveLock lock;

    ThreadsafeStack<size_t> primes{ lock };

    PrimeCalculator<size_t> calc{ primes, Globals::LowerLimit, Globals::UpperLimit + 1 };

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

    Logger::log(std::cout, "Calcalating Prime Numbers from ", Globals::LowerLimit, " up to ", Globals::UpperLimit, ':');

    // NoLock lock;   // crashes sporadically // Access Violation // 0xc0000005 
    ExclusiveLock lock;

    ThreadsafeStack<size_t> primes{ lock };

    unsigned int NumThreads = std::thread::hardware_concurrency();
    Logger::log(std::cout, "Number of concurrent threads currently supported: ", NumThreads, ':');

    std::vector<std::thread> threads;
    threads.reserve(NumThreads);

    size_t range = (Globals::UpperLimit - Globals::LowerLimit) / NumThreads;
    size_t start = Globals::LowerLimit;
    size_t end = start + range;

    ScopedTimer watch{};

    // setup threads
    for (size_t i{}; i != NumThreads - 1; ++i) {

        PrimeCalculator<size_t> calc{ primes, start, end };
        threads.emplace_back(calc);

        start = end;
        end = start + range;
    }

    // setup last thread
    end = Globals::UpperLimit;
    PrimeCalculator<size_t> calc{ primes, start, end + 1 };
    threads.emplace_back(calc);

    // wait for end of all threads
    for (size_t i{}; i != NumThreads; ++i) {
        threads[i].join();
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
