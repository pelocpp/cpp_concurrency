// ===========================================================================
// Examples_05.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "ThreadPool_05_ZenSepiol.h"

#include <thread>
#include <chrono>

namespace ThreadPool_ZenSepiol
{
    static void test_concurrency_thread_pool_05_01()
    {
        Logger::log(std::cout, "Start");

        ThreadPool pool{};

        std::this_thread::sleep_for(std::chrono::seconds{ 1 });

        pool.start();

        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        pool.stop();
    
        Logger::log(std::cout, "Done.");
    }

    // ===========================================================================

    static void emptyTask()
    {
        Logger::log(std::cout, "Doing nothing :)");
    }

    static void test_concurrency_thread_pool_05_02()
    {
        ThreadPool pool{};

        std::queue<std::future<void>> results;

        for (size_t n{}; n != 5; ++n)
        {
            auto future = pool.addTask(emptyTask);

            results.emplace(std::move(future));
        }

        pool.start();

        while (results.size())
        {
            results.front().get();
            results.pop();
        }

        pool.stop();

        Logger::log(std::cout, "Done.");
    }

    // ===========================================================================

    static void Checksum(const std::uint32_t num, std::atomic_uint64_t* checksum)
    {
        checksum += num;
    }

    static void test_concurrency_thread_pool_05_03()
    {
        ThreadPool pool{};

        std::queue<std::future<void>> results;
        std::atomic_uint64_t checksum{ 0 };
        std::uint64_t localChecksum{ 0 };

        for (std::uint32_t n{}; n != 1000; ++n)
        {
            results.emplace(pool.addTask(Checksum, n, &checksum));
            localChecksum += n;
        }

        pool.start();

        while (results.size())
        {
            results.front().get();
            results.pop();
        }

        pool.stop();

        Logger::log(std::cout, "Done.");
    }

    // ====================================================

    // Primzahlenberechnung

    static bool isPrime(size_t number)
    {
        if (number <= 2) {
            return number == 2;
        }

        if (number % 2 == 0) {
            return false;
        }

        // check odd divisors from 3 to the square root of the number
        size_t end{ static_cast<size_t>(std::ceil(std::sqrt(number))) };
        for (size_t i{ 3 }; i <= end; i += 2) {

            if (number % i == 0) {
                return false;  // number not prime
            }
        }

        return true; // found prime number
    }

    static void test_concurrency_thread_pool_05_04_PrimeNumbers()
    {
        Logger::log(std::cout, "Start");

        ScopedTimer clock{};

        size_t foundPrimeNumbers{};

        std::queue<std::future<bool>> results;

        ThreadPool pool{ };

        // pool.start();

        Logger::log(std::cout, "Enqueuing tasks");

        // 24 prime numbers
        //constexpr size_t Start = 1;
        //constexpr size_t End = Start + 100;

        // 4 prime numbers
        //constexpr size_t Start = 1000000000001;
        //constexpr size_t End = Start + 100;

        // 37 prime numbers
        //constexpr size_t Start = 1000000000001;
        //constexpr size_t End = Start + 1000;

        // 3614 prime numbers
        //constexpr size_t Start = 1000000000001;
        //constexpr size_t End = Start + 100000;

        // 23 prime numbers
        //constexpr size_t Start = 1'000'000'000'000'000'001;
        //constexpr size_t End = Start + 1'000;

        // 4 prime numbers
        constexpr size_t Start = 1'000'000'000'000'000'001;
        constexpr size_t End = Start + 100;

        Logger::enableLogging(false);

        for (size_t i{ Start }; i < End; i += 2) {

            std::future<bool> future{ pool.addTask(isPrime, i) };

            results.emplace(std::move(future));
        }

        Logger::enableLogging(true);

        Logger::log(std::cout, "Enqueuing tasks done.");

        pool.start();

        while (results.size() != 0)
        {
            auto found = results.front().get();
            if (found) {
                ++foundPrimeNumbers;
            }

            results.pop();
        }

        Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Start, " and ", End, '.');
        
        pool.stop();

        Logger::log(std::cout, "Done.");
    }

    static void test_concurrency_thread_pool_05_05_PrimeNumbers()
    {
        Logger::log(std::cout, "Start:");

        ScopedTimer clock{};

     //   size_t foundPrimeNumbers{};
        std::atomic_uint64_t foundPrimeNumbers{ 0 };

        std::vector<std::future<std::pair<bool, size_t>>> futures;

        ThreadPool pool{ };

        Logger::log(std::cout, "Enqueuing tasks");

        // 24 prime numbers
        //constexpr size_t Start = 1;
        //constexpr size_t End = Start + 100;

        // 4 prime numbers
        //constexpr size_t Start = 1000000000001;
        //constexpr size_t End = Start + 100;

        // 37 prime numbers
        //constexpr size_t Start = 1000000000001;
        //constexpr size_t End = Start + 1000;

        // 3614 prime numbers
        //constexpr size_t Start = 1000000000001;
        //constexpr size_t End = Start + 100000;

        // 23 prime numbers
        constexpr size_t Start = 1'000'000'000'000'000'001;
        constexpr size_t End = Start + 1'000;

        std::function<std::pair<bool, size_t>(size_t)> primeTask {
            
            [&](size_t value) {

                bool primeFound { isPrime(value) };

                if (primeFound) {
                    Logger::log(std::cout, "> ", value, " is prime.");

                    ++foundPrimeNumbers;
                }

                return std::make_pair(primeFound, value);
            }
        };

        Logger::enableLogging(false);

        for (size_t i{ Start }; i < End; i += 2) {

            std::future<std::pair<bool, size_t>> future{
                pool.addTask(primeTask, i)
            };

            futures.push_back(std::move(future));
        }

        Logger::enableLogging(true);

        Logger::log(std::cout, "Enqueuing tasks done.");

        pool.start();

        for (auto& future : futures) { 

            const auto& [found, value] = future.get();

            if (found) {
                // ++foundPrimeNumbers;
                Logger::log(std::cout, "Found prime numer ", value);
            }
        }

        Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Start, " and ", End, '.');
       
        pool.stop();

        Logger::log(std::cout, "Done.");
    }
}

void test_concurrency_thread_pool_05()
{
    using namespace ThreadPool_ZenSepiol;
    //test_concurrency_thread_pool_05_01();
    //test_concurrency_thread_pool_05_02();
    //test_concurrency_thread_pool_05_03();

    test_concurrency_thread_pool_05_04_PrimeNumbers();
    //test_concurrency_thread_pool_05_05_PrimeNumbers();
}
// ===========================================================================
// End-of-File
// ===========================================================================

