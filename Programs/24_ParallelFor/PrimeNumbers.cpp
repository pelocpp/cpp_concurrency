// ===========================================================================
// PrimeNumbers.cpp
// ===========================================================================

#include "../20_Threadsafe_Stack/ThreadsafeStack.h"
#include "../20_Threadsafe_Stack/PrimeCalculator.h"

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "ParallelFor.h"

constexpr bool Verbose{ false };

namespace Globals
{
    // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm

    constexpr size_t NumThreads = 8;

    // constexpr size_t UpperLimit { 100 };             // Found:  25 prime numbers
    // constexpr size_t UpperLimit { 1000 };            // Found:  168 prime numbers
    // constexpr size_t UpperLimit { 100'000 };         // Found:  9.592 prime numbers
    // constexpr size_t UpperLimit { 1'000'000 };       // Found:  78.498 prime numbers
    // constexpr size_t UpperLimit { 10'000'000 };      // Found:  664.579 prime numbers
    constexpr size_t UpperLimit { 100'000'000 };     // Found:  5.761.455 prime numbers
}

// ===========================================================================

Concurrency_ThreadsafeStack::ThreadsafeStack<size_t> g_primes{};

static void calcPrimesRange(size_t start, size_t end)
{
    for (size_t i{ start }; i != end; ++i) {

        if (Concurrency_PrimeCalculator::PrimeCalculator<size_t>::isPrime(i)) {
            g_primes.push(i);
        }
    }
}

static void test_parallel_for_01(size_t from, size_t to, bool useThreads)
{
    using namespace Concurrency_ParallelFor;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", from,
        " up to ", to, ':');

    {
        ScopedTimer timer{};

        parallel_for(
            from,
            to,
            [] (size_t start, size_t end) {
                calcPrimesRange(start, end);
            },
            useThreads
        );
    }

    Logger::log(std::cout, "Found: ", g_primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_parallel_for_02(size_t from, size_t to, bool useThreads)
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;
    using namespace Concurrency_ParallelFor;

    Logger::log(std::cout,
        "Calcalating Prime Numbers from ", from,
        " up to ", to, ':');

    ThreadsafeStack<size_t> primes{};

    auto calcPrimesRange = [&] (size_t start, size_t end) {

        PrimeCalculator<size_t> calc{ primes, start, end };
        calc();
    };

    {
        ScopedTimer timer{};

        parallel_for(
            from,
            to,
            [&](size_t start, size_t end) {
                calcPrimesRange(start, end);
            },
            useThreads
        );
    }

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
    Logger::log(std::cout, "Done.");
}

void test_parallel_for()
{
    test_parallel_for_01(2, Globals::UpperLimit, true);
    test_parallel_for_02(2, Globals::UpperLimit, true);
    test_parallel_for_02(2, 50, false);
}

// ===========================================================================
// End-of-File
// ===========================================================================
