// ===========================================================================
// TestPrimeNumbers.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "../Globals/GlobalPrimes.h"
#include "../Globals/IsPrime.h"

#include "PrimeCalculator.h"
#include "ThreadsafeStack.h"

#include <functional>
#include <iostream>
#include <vector>

static auto NumThreads = [] { return std::thread::hardware_concurrency(); }();

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

    bool result{ stack.tryPop(n) };
    std::cout << "got " << n << std::boolalpha << " [" << result << "]" << std::endl;

    std::optional<int> m;
    m = stack.tryPop();
    if (m.has_value()) {
        std::cout << "got " << m.value() << std::endl;
    }

    // stack.pop(n);  // crashes
}

void test_thread_safe_stack_02()
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", 2,
        " up to ", PrimeNumberLimits::UpperLimit, ':');

    ThreadsafeStack<size_t> primes{};

    PrimeCalculator<size_t> calc{ primes, 2, PrimeNumberLimits::UpperLimit };

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
        "Calcalating Prime Numbers from ", 2, " up to ",
        PrimeNumberLimits::UpperLimit, " [", NumThreads, " threads]:");

    ThreadsafeStack<size_t> primes{};

    std::vector<std::thread> threads;
    threads.reserve(NumThreads);

    size_t range = (PrimeNumberLimits::UpperLimit - 2) / NumThreads;
    size_t start = 2;
    size_t end = start + range;

    {
        ScopedTimer timer{};

        // setup threads
        for (size_t i{}; i != NumThreads - 1; ++i) {

            PrimeCalculator<size_t> calc{ primes, start, end };
            threads.emplace_back(calc);

            start = end;
            end = start + range;
        }

        // setup last thread
        PrimeCalculator<size_t> calc{ primes, start, PrimeNumberLimits::UpperLimit };
        threads.emplace_back(calc);

        // wait for end of all threads
        for (size_t i{}; i != NumThreads; ++i) {
            threads[i].join();
        }
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

void test_thread_safe_stack_04()
{
    constexpr bool Verbose{ true };  // set to 'true'

    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", 2, " up to ",
        PrimeNumberLimits::UpperLimit, " [", NumThreads, " threads]:");

    using Callable = std::function<void()>;

    auto callableWrapper = [] (Callable callable) {

        if (Verbose) {
            Logger::log(std::cout, "> TID: ", std::this_thread::get_id());
        }

        callable();

        if (Verbose) {
            Logger::log(std::cout, "< TID: ", std::this_thread::get_id());
        }
    };

    ThreadsafeStack<size_t> primes{};

    std::vector<std::thread> threads;
    threads.reserve(NumThreads);

    size_t range = (PrimeNumberLimits::UpperLimit - 2) / NumThreads;
    size_t start = 2;
    size_t end = start + range;

    {
        ScopedTimer timer{};

        // setup threads
        for (size_t i{}; i != NumThreads - 1; ++i) {

            PrimeCalculator<size_t> calc{ primes, start, end };
            threads.emplace_back(callableWrapper, calc);

            start = end;
            end = start + range;
        }

        // setup last thread
        PrimeCalculator<size_t> calc{ primes, start, PrimeNumberLimits::UpperLimit };
        threads.emplace_back(callableWrapper, calc);

        // wait for end of all threads
        for (size_t i{}; i != NumThreads; ++i) {
            threads[i].join();
        }
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
