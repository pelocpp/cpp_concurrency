// ===========================================================================
// PrimeNumbers01.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "../Globals/GlobalPrimes.h"
#include "../Globals/IsPrime.h"

#include "ParallelFor01.h"

#include <mutex>          // std::mutex, std::lock_guard
#include <unordered_set>  // std::unordered_set
#include <thread>         // std::thread::id
#include <vector>         // std::vector

// ===========================================================================

static void test_parallel_for()
{
    std::size_t from{ PrimeNumberLimits::Start };
    std::size_t to{ PrimeNumberLimits::End };

    std::mutex tids_mutex;
    std::mutex primes_mutex;

    std::unordered_set<std::thread::id> tids;
    std::vector<std::size_t> primes;

    auto checkPrime = [&](std::size_t i) {

        std::thread::id tid{ std::this_thread::get_id() };

        {
            std::lock_guard<std::mutex> tids_guard{ tids_mutex };
            tids.insert(tid);
        }

        if (PrimeNumbers::IsPrime(i)) {

            std::lock_guard<std::mutex> primes_guard{ primes_mutex };
            primes.push_back(i);
        }
    };

    Logger::log(std::cout, "Start: ", from, " => ", to);

    {
        ScopedTimer watch{};

        Concurrency_ParallelFor::parallel_for_stl(
            from,
            to,
            checkPrime
        );

        //Concurrency_ParallelFor::parallel_for_ranges(
        //    from,
        //    to,
        //    checkPrime
        //);
    }

    Logger::log(std::cout, "Threads Used: ", tids.size());
    Logger::log(std::cout, "Found Primes: ", primes.size());
}

// ===========================================================================

void test_parallel_for_01()
{
    test_parallel_for();
}

// ===========================================================================
// End-of-File
// ===========================================================================
