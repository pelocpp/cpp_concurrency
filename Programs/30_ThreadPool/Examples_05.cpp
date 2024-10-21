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
    void test_concurrency_thread_pool_05_01()
    {
        Logger::log(std::cout, "Start");

        size_t numThreads{ std::thread::hardware_concurrency() };

        ThreadPool pool{ numThreads };

        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    
        Logger::log(std::cout, "Done.");
    }

    static void emptyTask()
    {
    }

    void test_concurrency_thread_pool_05_02()
    {
        size_t numThreads{ std::thread::hardware_concurrency() };

        ThreadPool pool{ numThreads };

        std::queue<std::future<void>> results;

        for (int n = 0; n < 5; ++n)
        {
            auto future = pool.addTask(emptyTask);

            results.emplace(std::move(future));
        }

        while (results.size())
        {
            results.front().get();
            results.pop();
        }
    }

    void Checksum(const std::uint32_t num, std::atomic_uint64_t* checksum)
    {
        *checksum += num;
    }

    void test_concurrency_thread_pool_05_03()
    {
        size_t numThreads{ std::thread::hardware_concurrency() };

        ThreadPool pool{ numThreads };

        std::queue<std::future<void>> results;
        std::atomic_uint64_t checksum{ 0 };
        std::uint64_t localChecksum{ 0 };

        for (std::uint32_t n = 0; n < 1000; ++n)
        {
            results.emplace(pool.addTask(Checksum, n, &checksum));
            localChecksum += n;
        }

        while (results.size())
        {
            results.front().get();
            results.pop();
        }
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
        size_t end{ static_cast<size_t>(ceil(std::sqrt(number))) };
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

        //std::function<void(size_t)> primeTask{ [&] (size_t value) {

        //        bool primeFound { isPrime(value) };

        //        if (primeFound) {

        //            Logger::log(std::cout, "> ", value, " IS prime.");
        //            ++foundPrimeNumbers;
        //        }

        //        return primeFound;
        //    }
        //};

        //EventLoop eventLoop;

        std::queue<std::future<bool>> results;

        size_t numThreads{ std::thread::hardware_concurrency() };

        ThreadPool pool{ numThreads };

        Logger::log(std::cout, "Enqueuing tasks");

        // 24 prime numbers
        //constexpr size_t Start = 1;
        //constexpr size_t End = Start + 100;

        // 4 prime numbers
        //constexpr size_t Start = 1000000000001;
        //constexpr size_t End = Start + 100;

        // 3614 prime numbers
        constexpr size_t Start = 1000000000001;
        constexpr size_t End = Start + 100000;

        for (size_t i{ Start }; i < End; i += 2) {

            std::future<bool> future{ pool.addTask(isPrime, i) };

            results.emplace(std::move(future));
        }

        Logger::log(std::cout, "Enqueuing tasks done.");

        while (results.size() != 0)
        {
            auto found = results.front().get();
            if (found) {
                ++foundPrimeNumbers;
            }

            results.pop();
        }

        Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Start, " and ", End, '.');
        Logger::log(std::cout, "Done.");
    }

    //static void test_concurrency_thread_pool_05_05_PrimeNumbers()
    //{
    //    Logger::log(std::cout, "Start:");

    //    size_t foundPrimeNumbers{};

    //    //EventLoop eventLoop;

    //    std::vector<std::future<std::pair<bool, size_t>>> futures;

    //    size_t numThreads{ std::thread::hardware_concurrency() };
    //    ThreadPool pool{ numThreads };

    //    Logger::log(std::cout, "Enqueuing tasks");

    //    //const size_t Start = 1000000000001;
    //    //const size_t End = Start + 100000;

    //    const size_t Start = 1;
    //    const size_t End = Start + 100;

    //    for (size_t i{ Start }; i < End; i += 2) {

    //        auto future = pool.async([i]() {

    //            bool primeFound{ isPrime(i) };

    //            if (primeFound) {

    //                Logger::log(std::cout, "> ", i, " IS prime.");
    //            }

    //            return std::make_pair(primeFound, i);
    //            });

    //        futures.push_back(std::move(future));
    //    }

    //    Logger::log(std::cout, "Enqueuing tasks done.");

    //    for (auto& future : futures) {

    //        const auto& [found, value] = future.get();
    //        if (found) {
    //            ++foundPrimeNumbers;
    //        }
    //    }

    //    Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Start, " and ", End, '.');
    //    Logger::log(std::cout, "Done.");
    //}
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

