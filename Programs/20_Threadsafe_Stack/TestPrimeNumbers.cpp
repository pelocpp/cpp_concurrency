// ===========================================================================
// TestPrimeNumbers.cpp
// ===========================================================================

#include <iostream>
#include <vector>
#include <optional>
#include <functional>
#include <chrono>

#include "ThreadsafeStack.h"
#include "PrimeCalculator.h"

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

namespace Globals
{
    // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm

    constexpr size_t NumThreads = 16;

    // constexpr size_t UpperLimit { 100 };
    // Found:  25 prime numbers

    constexpr size_t UpperLimit{ 1000 };
    // Found:  168 prime numbers

    // constexpr size_t UpperLimit { 100'000 };
    // Found:  9.592 prime numbers

    // constexpr size_t UpperLimit { 1'000'000 };
    // Found:  78.498 prime numbers

    // constexpr size_t UpperLimit { 10'000'000 };
    // Found:  664.579 prime numbers

    // constexpr size_t UpperLimit { 100'000'000 };
    // Found:  5.761.455 prime numbers
}

void test_thread_safe_stack_01()
{
    using namespace Concurrency_ThreadsafeStack;

    ThreadsafeStack<int> stack{};

    stack.push(1);
    stack.push(2);
    stack.push(3);

    int n{};    
    stack.pop(n);
    std::cout << "got " << n << std::endl;

    std::optional<int> m;
    m = stack.tryPopOptional();
    std::cout << "got " << m.value() << std::endl;

    n = stack.tryPop();
    std::cout << "got " << n << std::endl;

    // n = stack.tryPop();  // crashes
}

void test_thread_safe_stack_02()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", 2,
        " up to ", Globals::UpperLimit, ':');

    ThreadsafeStack<size_t> primes{};

    PrimeCalculator<size_t> calc{ primes, 2, Globals::UpperLimit };

    {
        ScopedTimer timer{};

        std::thread calculator(calc);
        calculator.join();
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

void test_thread_safe_stack_03()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", 2, 
        " up to ", Globals::UpperLimit, ':');

    ThreadsafeStack<size_t> primes{};

    std::vector<std::thread> threads;
    threads.reserve(Globals::NumThreads);

    size_t range = (Globals::UpperLimit - 2) / Globals::NumThreads;
    size_t start = 2;
    size_t end = start + range;

    {
        ScopedTimer timer{};

        // setup threads
        for (size_t i{}; i != Globals::NumThreads - 1; ++i) {

            PrimeCalculator<size_t> calc{ primes, start, end };
            threads.emplace_back(calc);

            start = end;
            end = start + range;
        }

        // setup last thread
        PrimeCalculator<size_t> calc{ primes, start, Globals::UpperLimit };
        threads.emplace_back(calc);

        // wait for end of all threads
        for (size_t i{}; i != Globals::NumThreads; ++i) {
            threads[i].join();
        }
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

void test_thread_safe_stack_04()
{
    constexpr bool Verbose{ false };

    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", 2,
        " up to ", Globals::UpperLimit, ':');

    using Callable = std::function<void()>;

    auto callableWrapper = [] (Callable callable) {

        if (Verbose) {
            Logger::log(std::cout, "TID: ", std::this_thread::get_id());
        }

        callable();
    };

    ThreadsafeStack<size_t> primes{};

    std::vector<std::thread> threads;
    threads.reserve(Globals::NumThreads);

    size_t range = (Globals::UpperLimit - 2) / Globals::NumThreads;
    size_t start = 2;
    size_t end = start + range;

    {
        ScopedTimer timer{};

        // setup threads
        for (size_t i{}; i != Globals::NumThreads - 1; ++i) {

            PrimeCalculator<size_t> calc{ primes, start, end };
            threads.emplace_back(callableWrapper, calc);

            start = end;
            end = start + range;
        }

        // setup last thread
        PrimeCalculator<size_t> calc{ primes, start, Globals::UpperLimit };
        threads.emplace_back(callableWrapper, calc);

        // wait for end of all threads
        for (size_t i{}; i != Globals::NumThreads; ++i) {
            threads[i].join();
        }
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
