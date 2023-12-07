// ===========================================================================
// TestPrimeNumbersTemplate.cpp
// ===========================================================================

#include <iostream>
#include <vector>
#include <optional>
#include <chrono>

#include "ThreadsafeStack.h"
#include "PrimeCalculator.h"

// #include "StrategizedLock.h"
#include "StrategizedLockTemplate.h"

#include "../Logger/Logger.h"

namespace Globals
{
    // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm

    constexpr size_t NumThreads = 8;

    //constexpr size_t LowerLimit = 1;
    //constexpr size_t UpperLimit = 100;

    //constexpr size_t LowerLimit = 1;
    //constexpr size_t UpperLimit = 10'000'000;
    //// Found:  664579 prime numbers

    //constexpr size_t LowerLimit = 1'000'000;
    //constexpr size_t UpperLimit = 10'000'000;

    //constexpr size_t LowerLimit = 1;
    //constexpr size_t UpperLimit = 100'000;
    //// Found: 9.592 prime numbers

    //constexpr size_t LowerLimit = 1;
    //constexpr size_t UpperLimit = 1'000'000;
    //// Found: 78.498 prime numbers

    constexpr size_t LowerLimit = 1;
    constexpr size_t UpperLimit = 10'000'000;
    // Found:  664.579 prime numbers

    //constexpr size_t LowerLimit = 1;
    //constexpr size_t UpperLimit = 100'000'000;
    //// Found:  5.761.455 prime numbers
}

void test_strategized_locking_01()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_StrategizedCompileTime;

    // NoLock lock;
    // ExclusiveLock lock;

    NoLock noLock;
    StrategizedLocking<NoLock> lock{ noLock };

    ThreadsafeStack<size_t> stack{ lock };

    const auto startTime{ std::chrono::high_resolution_clock::now() };

    constexpr size_t MaxIterations = 1'000'000;

    Logger::log(std::cout, "Calling push: ", MaxIterations);

    for (size_t i = 0; i != MaxIterations; ++i) {
        stack.push(i);

        size_t value{};
        stack.pop(value);
    }

    const auto endTime{ std::chrono::high_resolution_clock::now() };
    double msecs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

    Logger::log(std::cout, "Done:  ", msecs, " msecs.");
}


void test_strategized_locking_02()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;
    using namespace Concurrency_StrategizedLockRuntime;
    // using namespace Concurrency_StrategizedCompileTime;


    Logger::log(std::cout, "Calcalating Prime Numbers from ", Globals::LowerLimit, " up to ", Globals::UpperLimit, ':');

    NoLock lock;
    //ExclusiveLock lock;

    ThreadsafeStack<size_t> primes{ lock };

    PrimeCalculator<size_t> calc{ primes, Globals::LowerLimit, Globals::UpperLimit + 1 };

    const auto startTime{ std::chrono::high_resolution_clock::now() };

    std::thread calculator(calc);
    calculator.join();

    const auto endTime{ std::chrono::high_resolution_clock::now() };
    double msecs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done:  ", msecs, " msecs.");
}

void test_strategized_locking_03()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;

    using namespace Concurrency_StrategizedLockRuntime;
    // using namespace Concurrency_StrategizedCompileTime;

    Logger::log(std::cout, "Calcalating Prime Numbers from ", Globals::LowerLimit, " up to ", Globals::UpperLimit, ':');

    NoLock lock;
    // ExclusiveLock lock;

    ThreadsafeStack<size_t> primes{ lock };

    std::vector<std::thread> threads;
    threads.reserve(Globals::NumThreads);

    size_t range = (Globals::UpperLimit - Globals::LowerLimit) / Globals::NumThreads;
    size_t start = Globals::LowerLimit;
    size_t end = start + range;

    const auto startTime{ std::chrono::high_resolution_clock::now() };

    // setup threads
    for (size_t i{}; i != Globals::NumThreads - 1; ++i) {

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
    for (size_t i{}; i != Globals::NumThreads; ++i) {
        threads[i].join();
    }

    const auto endTime{ std::chrono::high_resolution_clock::now() };
    double msecs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done:  ", msecs, " msecs.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
