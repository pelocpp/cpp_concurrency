// ===========================================================================
// PrimeNumbers.cpp
// ===========================================================================

#include "../20_Threadsafe_Stack/ThreadsafeStack.h"
#include "../20_Threadsafe_Stack/PrimeCalculator.h"

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "ParallelFor.h"

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

    //constexpr size_t LowerLimit = 1;
    constexpr size_t UpperLimit = 10'000'000;
    // Found:  664.579 prime numbers

    //constexpr size_t LowerLimit = 1;
    //constexpr size_t UpperLimit = 100'000'000;
    //// Found:  5.761.455 prime numbers
}

// ===========================================================================

Concurrency_ThreadsafeStack::ThreadsafeStack<size_t> g_primes{};

void calcPrimesRange(size_t start, size_t end)
{
    Logger::log(std::cout, std::this_thread::get_id(), "primes range: ", start, " -> ", end);

    for (size_t i{ start }; i != end; ++i) {

        if (Concurrency_PrimeCalculator::PrimeCalculator<size_t>::isPrime(i)) {
            g_primes.push(i);
        }
    }
}

void test_parallel_for_01(size_t from, size_t to, bool useThreads)
{
    using namespace Concurrency_Parallel_For;

    parallel_for(
        from,
        to,
        [](size_t start, size_t end) {
            calcPrimesRange(start, end);
        },
        useThreads
    );

    Logger::log(std::cout, "Found: ", g_primes.size(), " prime numbers.");
}

// ===========================================================================

void test_parallel_for_02(size_t from, size_t to, bool useThreads)
{
    using namespace Concurrency_ThreadsafeStack;
    using namespace Concurrency_PrimeCalculator;
    using namespace Concurrency_Parallel_For;

    ThreadsafeStack<size_t> primes{};

    auto calcPrimesRange = [&] (size_t start, size_t end) {

        Logger::log(std::cout, std::this_thread::get_id(), "primes range: ", start, " -> ", end);

        PrimeCalculator<size_t> calc{ primes, start, end };
        calc();
    };

    parallel_for (
        from,
        to,
        [&](size_t start, size_t end) {
            calcPrimesRange(start, end);
        },
        useThreads
    );

    Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
}

void test_parallel_for()
{
    test_parallel_for_01(2, Globals::UpperLimit, true);
    test_parallel_for_02(2, Globals::UpperLimit, true);
}

// ===========================================================================
// End-of-File
// ===========================================================================
