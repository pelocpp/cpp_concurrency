// ===========================================================================
// Examples.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "../Globals/GlobalPrimes.h"
#include "../Globals/IsPrime.h"

#include "ThreadPool.h"

#include <vector>

// ===========================================================================

static void test_concurrency_thread_pool01()
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

static void test_concurrency_thread_pool02()
{
    ThreadPool pool{};

    std::queue<std::future<void>> results{};

    for (std::size_t n{}; n != 5; ++n)
    {
        auto future{ pool.addTask(emptyTask) };
        // auto future{ pool.addTaskEx(emptyTask) };
        results.push(std::move(future));
    }

    pool.start();

    while (results.size())
    {
        auto& future{ results.front() };
        future.get();
        results.pop();
    }

    pool.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void calcChecksumWeak(std::size_t num, std::size_t* checksum)
{
    *checksum += num;
}

static void calcChecksumPtr(std::size_t num, std::atomic<std::size_t>* checksum)
{
    *checksum += num;
}

static void calcChecksumRef(std::size_t num, std::atomic<std::size_t>& checksum)
{
    checksum += num;
}

static constexpr std::size_t NumThreads{ 2'000 };

static void test_concurrency_thread_pool03()
{
    // Testing threads accessing thread-global variable / not atomar
    // Note: error occurs sporadically

    ThreadPool pool{};

    std::vector<std::future<void>> results;
    std::size_t checksum{ 0 };
    std::size_t localChecksum{ 0 };

    Logger::enableLogging(false);
    for (std::uint32_t n{}; n != NumThreads; ++n)
    {
        auto future{ pool.addTask(calcChecksumWeak, n, &checksum) };
        results.push_back(std::move(future));
        localChecksum += n;
    }
    Logger::enableLogging(true);

    pool.start();
    results.clear();
    pool.stop();

    Logger::log(std::cout, "Weak Checksum:   ", checksum);
    Logger::log(std::cout, "Local Checksum:  ", localChecksum);
    Logger::log(std::cout, "Done.");
}

static void test_concurrency_thread_pool04()
{
    // Testing threads accessing thread-global variable / atomar

    ThreadPool pool{};

    std::vector<std::future<void>> results;
    std::atomic<std::size_t> atomicChecksum{ 0 };
    std::size_t localChecksum{ 0 };

    Logger::enableLogging(false);
    for (std::uint32_t n{}; n != NumThreads; ++n)
    {
        auto future{ pool.addTask(calcChecksumPtr, n, &atomicChecksum) };
        results.push_back(std::move(future));
        localChecksum += n;
    }
    Logger::enableLogging(true);

    pool.start();
    results.clear();
    pool.stop();

    Logger::log(std::cout, "Safe Checksum:   ", atomicChecksum.load());
    Logger::log(std::cout, "Local Checksum:  ", localChecksum);
    Logger::log(std::cout, "Done.");
}

static void test_concurrency_thread_pool05()
{
    // Testing threads accessing thread-global variable / atomar

    ThreadPool pool{};

    std::vector<std::future<void>> results;
    std::atomic<std::size_t> atomicChecksum{ 0 };
    std::size_t localChecksum{ 0 };

    Logger::enableLogging(false);
    for (std::uint32_t n{}; n != NumThreads; ++n)
    {
        auto future{ pool.addTask(calcChecksumRef, n, std::ref(atomicChecksum)) };
        results.push_back(std::move(future));
        localChecksum += n;
    }
    Logger::enableLogging(true);

    pool.start();
    results.clear();
    pool.stop();

    Logger::log(std::cout, "Safe Checksum:   ", atomicChecksum.load());
    Logger::log(std::cout, "Local Checksum:  ", localChecksum);
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// Primzahlenberechnung

static void test_concurrency_thread_pool10_PrimeNumbers()
{
    Logger::log(std::cout, "Press any key to start ...");
    char ch;
    std::cin >> ch;

    Logger::log(std::cout, "Start");

    ScopedTimer clock{};

    std::size_t foundPrimeNumbers{};

    std::queue<std::future<bool>> results;

    ThreadPool pool{};

    pool.start();         // <=== add/remove comment

    Logger::log(std::cout, "Enqueuing tasks");

    Logger::enableLogging(true);

    for (std::size_t i{ PrimeNumberLimits::Start }; i < PrimeNumberLimits::End; i += 2) {

        std::future<bool> future{ pool.addTask(PrimeNumbers::IsPrime, i) };

        results.push(std::move(future));
    }

    Logger::enableLogging(true);

    Logger::log(std::cout, "Enqueuing tasks done.");

    // pool.start();            // <=== add/remove comment

    while (results.size() != 0)
    {
        auto found = results.front().get();
        if (found) {
            ++foundPrimeNumbers;
        }

        results.pop();
    }

    Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", 
        PrimeNumberLimits::Start, " and ", PrimeNumberLimits::End, '.'
    );
        
    pool.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_concurrency_thread_pool11_PrimeNumbers()
{
    Logger::log(std::cout, "Press any key to start ...");
    char ch;
    std::cin >> ch;

    Logger::log(std::cout, "Start:");

    ScopedTimer clock{};

    std::atomic_uint64_t foundPrimeNumbers{ 0 };

    std::vector<std::future<std::pair<bool, std::size_t>>> futures;

    ThreadPool pool{ };

    Logger::log(std::cout, "Enqueuing tasks");

    //std::function<std::pair<bool, std::size_t>(std::size_t)> primeTask {
    //    
    //    [&](std::size_t value) {
    //        bool primeFound { isPrime(value) };
    //        if (primeFound) {
    //            Logger::log(std::cout, "> ", value, " is prime.");
    //            ++foundPrimeNumbers;
    //        }
    //        return std::make_pair(primeFound, value);
    //    }
    //};

    auto primeLambda = [&](std::size_t value) {

        bool primeFound{ PrimeNumbers::IsPrime(value) };

        if (primeFound) {
            Logger::log(std::cout, "> ", value, " is prime.");

            ++foundPrimeNumbers;
        }

        return std::make_pair(primeFound, value);
    };

    Logger::enableLogging(false);

    for (std::size_t i{ PrimeNumberLimits::Start }; i < PrimeNumberLimits::End; i += 2) {

        std::future<std::pair<bool, std::size_t>> future{
            pool.addTask(primeLambda, i)
        };

        futures.push_back(std::move(future));
    }

    Logger::enableLogging(true);

    Logger::log(std::cout, "Enqueuing tasks done.");

    pool.start();

    for (auto& future : futures) { 

        const auto& [found, value] = future.get();

        if (found) {
            Logger::log(std::cout, "Found prime numer ", value);
        }
    }

    Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", PrimeNumberLimits::Start, " and ", PrimeNumberLimits::End, '.');
       
    pool.stop();

    Logger::log(std::cout, "Done.");
}

void test_concurrency_thread_pool()
{
    // test_concurrency_thread_pool01();     // just launching ... and stopping the thread pool
    // test_concurrency_thread_pool02();     // launching 5 almost empty tasks
    
    test_concurrency_thread_pool03();  // launching many tasks ... and working on the same global variable (by address)
    test_concurrency_thread_pool04();  // launching many tasks ... and working on an atomic variable (by address)
    test_concurrency_thread_pool05();  // launching many tasks ... and working on an atomic variable (by reference)

   // test_concurrency_thread_pool10_PrimeNumbers();      // computing prime numbers, using free function
    //test_concurrency_thread_pool11_PrimeNumbers();    // computing prime numbers, using lambda
}

// ===========================================================================
// End-of-File
// ===========================================================================
