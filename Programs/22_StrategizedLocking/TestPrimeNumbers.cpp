// ===========================================================================
// TestPrimeNumbers.cpp
// ===========================================================================

#include <iostream>
#include <vector>
#include <optional>
#include <chrono>

#include "ThreadsafeStack.h"
#include "PrimeCalculator.h"
#include "StrategizedLocking.h"

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

void test_thread_safe_stack_01()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;

    Logger::log(std::cout, "Calcalating Prime Numbers from ", Globals::LowerLimit, " up to ", Globals::UpperLimit, ':');

    NoLock noLock;
    ThreadsafeStack<size_t> primes{ noLock };

    PrimeCalculator<size_t> calc{ primes, Globals::LowerLimit, Globals::UpperLimit + 1 };

    const auto startTime{ std::chrono::high_resolution_clock::now() };

    std::thread calculator(calc);
    calculator.join();

    const auto endTime{ std::chrono::high_resolution_clock::now() };
    double msecs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done:  ", msecs, " msecs.");
}

//
//void test_thread_safe_stack_02()
//{
//    using namespace Concurrency_ThreadsafeStack;
//    using namespace Concurrency_PrimeCalculator;
//
//    Logger::log(std::cout, "Calcalating Prime Numbers from ", Globals::LowerLimit, " up to ", Globals::UpperLimit, ':');
//
//    ThreadsafeStack<size_t> primes{};
//
//    std::vector<std::thread> threads;
//    threads.reserve(Globals::NumThreads);
//
//    size_t range = (Globals::UpperLimit - Globals::LowerLimit) / Globals::NumThreads;
//    size_t start = Globals::LowerLimit;
//    size_t end = start + range;
//
//    const auto startTime{ std::chrono::high_resolution_clock::now() };
//
//    // setup threads
//    for (size_t i{}; i != Globals::NumThreads - 1; ++i) {
//
//        PrimeCalculator<size_t> calc{ primes, start, end };
//        threads.emplace_back(calc);
//
//        start = end;
//        end = start + range;
//    }
//
//    // setup last thread
//    end = Globals::UpperLimit;
//    PrimeCalculator<size_t> calc{ primes, start, end + 1 };
//    threads.emplace_back(calc);
//
//    // wait for end of all threads
//    for (size_t i{}; i != Globals::NumThreads; ++i) {
//        threads[i].join();
//    }
//
//    const auto endTime{ std::chrono::high_resolution_clock::now() };
//    double msecs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();
//
//    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
//    Logger::log(std::cout, "Done:  ", msecs, " msecs.");
//}

// ===========================================================================
// End-of-File
// ===========================================================================
