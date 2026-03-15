// ===========================================================================
// PrimeNumbers01.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "ParallelFor01.h"

#include <algorithm>
#include <cmath>
#include <execution>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Globals
{
    // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm

    //static constexpr size_t UpperLimit{ 100 };             // Found:  25 prime numbers
    //static constexpr size_t UpperLimit{ 1000 };            // Found:  168 prime numbers
    //static constexpr size_t UpperLimit{ 100'000 };         // Found:  9.592 prime numbers
    //static constexpr size_t UpperLimit{ 1'000'000 };       // Found:  78.498 prime numbers
    //static constexpr size_t UpperLimit{ 10'000'000 };      // Found:  664.579 prime numbers
    //static constexpr size_t UpperLimit{ 100'000'000 };     // Found:  5.761.455 prime numbers

    //// 24 prime numbers
    //static constexpr std::size_t Start{ 1 };
    //static constexpr std::size_t End { Start + 100 };

    //// 6 prime numbers
    //static constexpr std::size_t Start { 100'000'001 };
    //static constexpr std::size_t End { Start + 100 };

    //// 4 prime numbers
    //static constexpr std::size_t Start { 1000000000001 };
    //static constexpr std::size_t End { Start + 100 };

    //// 3614 prime numbers
    //static constexpr std::size_t Start { 1'000'000'000'001 };
    //static constexpr std::size_t End { Start + 100'000 };

    //// 4 prime numbers
    //static constexpr std::size_t Start { 1'000'000'000'000'000'001 };
    //static constexpr std::size_t End { Start + 100 };

    //// 241 prime numbers
    //static constexpr std::size_t Start { 1'000'000'000'000'000'001 };
    //static constexpr std::size_t End { Start + 10'000 };

    // 114 prime numbers
    static constexpr std::size_t Start{ 1'000'000'000'000'000'001 };
    static constexpr std::size_t End{ Start + 5'000 };
}

// ===========================================================================

static bool isPrime(std::size_t number)
{
    if (number <= 2) {
        return number == 2;
    }

    if (number % 2 == 0) {
        return false;
    }

    // check odd divisors from 3 to the square root of the number
    std::size_t end{ static_cast<std::size_t>(std::ceil(std::sqrt(number))) };
    for (std::size_t i{ 3 }; i <= end; i += 2) {

        if (number % i == 0) {
            return false;  // number not prime
        }
    }

    return true; // found prime number
}

// ===========================================================================

static void test_parallel_for()
{
    std::size_t from{ Globals::Start };
    std::size_t to{ Globals::End };

    std::mutex tids_mutex;
    std::mutex primes_mutex;

    std::set<std::thread::id> tids;
    std::vector<std::size_t> primes;

    auto checkPrime = [&](std::size_t i) {

        std::thread::id tid{ std::this_thread::get_id() };

        {
            std::lock_guard tids_guard{ tids_mutex };
            tids.insert(tid);
        }

        if (isPrime(i)) {

            std::lock_guard primes_guard{ primes_mutex };
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
