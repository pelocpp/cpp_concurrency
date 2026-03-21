// ===========================================================================
// PrimeNumbers02.cpp
// ===========================================================================

#include "../30_Threadsafe_Stack/ThreadsafeStack.h"
#include "../30_Threadsafe_Stack/PrimeCalculator.h"

#include "../Globals/GlobalPrimes.h"
#include "../Globals/IsPrime.h"

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "ParallelFor02.h"

constexpr bool Verbose{ false };

// ===========================================================================

Concurrency_ThreadsafeStack::ThreadsafeStack<std::size_t> g_primes{};

static void calcPrimesRange(std::size_t start, std::size_t end)
{
    for (std::size_t i{ start }; i != end; ++i) {

        if (PrimeNumbers::IsPrime(i)) {
            g_primes.push(i);
        }
    }
}

static void test_parallel_for_01(std::size_t from, std::size_t to, bool useThreads)
{
    using namespace Concurrency_ParallelFor_Legacy;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", from,
        " up to ", to, ':');

    {
        ScopedTimer timer{};

        parallel_for(
            from,
            to,
            [] (std::size_t start, std::size_t end) {
                calcPrimesRange(start, end);
            },
            useThreads
        );
    }

    Logger::log(std::cout, "Found: ", g_primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_parallel_for_02(std::size_t from, std::size_t to, bool useThreads)
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;
    using namespace Concurrency_ParallelFor_Legacy;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", from,
        " up to ", to, ':');

    ThreadsafeStack<std::size_t> primes{};

    auto calcPrimesRange = [&] (std::size_t start, std::size_t end) {

        PrimeCalculator<std::size_t> calc{ primes, start, end };
        calc();
    };

    {
        ScopedTimer timer{};

        parallel_for(
            from,
            to,
            [&](std::size_t start, std::size_t end) {
                calcPrimesRange(start, end);
            },
            useThreads
        );
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

void test_parallel_for_02()
{
    test_parallel_for_01(2, PrimeNumberLimits::UpperLimit, true);
    test_parallel_for_02(2, PrimeNumberLimits::UpperLimit, true);
    //test_parallel_for_02(2, 50, false);
}

// ===========================================================================
// End-of-File
// ===========================================================================
