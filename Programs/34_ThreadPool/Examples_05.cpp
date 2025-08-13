// ===========================================================================
// Examples_05.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "ThreadPool_05_ZenSepiol.h"

namespace ThreadPool_ZenSepiol
{
    // =======================================================================

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

    // =======================================================================

    static void emptyTask()
    {
        Logger::log(std::cout, "Doing nothing :)");
    }

    static void test_concurrency_thread_pool_05_02()
    {
        ThreadPool pool{};

        std::queue<std::future<void>> results{};

        for (size_t n{}; n != 5; ++n)
        {
            auto future{ pool.addTask(emptyTask) };

            results.push(std::move(future));
        }

        pool.start();

        while (results.size())
        {
            auto& future = results.front();
            future.get();
            results.pop();
        }

        pool.stop();

        Logger::log(std::cout, "Done.");
    }

    // =======================================================================

    static void calcChecksum(std::size_t num, std::atomic<size_t>* checksum)
    {
        (*checksum) += num;

        auto currentValue = checksum->load();

        Logger::log(std::cout, "     Adding ", num, " to ===> ", currentValue);
    }

    static void test_concurrency_thread_pool_05_03()
    {
        constexpr size_t NumThreads{ 1'000 };

        ThreadPool pool{};

        std::queue<std::future<void>> results;
        std::atomic<size_t> checksum{ 0 };
        std::size_t localChecksum{ 0 };

        for (std::uint32_t n{}; n != NumThreads; ++n)
        {
            auto future{ pool.addTask(calcChecksum, n, &checksum) };

            results.push(std::move(future));

            localChecksum += n;
        }

        pool.start();

        while (results.size())
        {
            results.front().get();
            results.pop();
        }

        pool.stop();

        auto atomicValue{ checksum.load() };

        Logger::log(std::cout, "Atomic Checksum: ", atomicValue);
        Logger::log(std::cout, "Local Checksum:  ", localChecksum);
        Logger::log(std::cout, "Done.");
    }

    // =======================================================================

    static void calcChecksumRef(std::size_t num, std::atomic<size_t>& checksum)
    {
        checksum += num;

        auto currentValue = checksum.load();

        Logger::log(std::cout, "     Adding ", num, " to ===> ", currentValue);
    }

    static void test_concurrency_thread_pool_05_04()
    {
        constexpr size_t NumThreads{ 1'000 };

        ThreadPool pool{};

        std::queue<std::future<void>> results;
        std::atomic<size_t> checksum{ 0 };
        std::size_t localChecksum{ 0 };

        for (std::uint32_t n{}; n != NumThreads; ++n)
        {   
            auto future{ pool.addTask(calcChecksumRef, n, std::ref(checksum)) };

            results.push(std::move(future));
            
            localChecksum += n;
        }

        pool.start();

        while (results.size())
        {
            results.front().get();
            results.pop();
        }

        pool.stop();

        auto atomicValue{ checksum.load() };

        Logger::log(std::cout, "Atomic Checksum: ", atomicValue);
        Logger::log(std::cout, "Local Checksum:  ", localChecksum);
        Logger::log(std::cout, "Done.");
    }

    // =======================================================================
    // Primzahlenberechnung

    namespace Globals
    {
        // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm
 
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
        //constexpr size_t Start = 1'000'000'000'000'000'001;
        //constexpr size_t End = Start + 100;
    
        // 23 prime numbers
        //constexpr size_t Start = 1'000'000'000'000'000'001;
        //constexpr size_t End = Start + 1'000;

        // 114 prime numbers
        constexpr size_t Start = 1'000'000'000'000'000'001;
        constexpr size_t End = Start + 5'000;
    }

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

    static void test_concurrency_thread_pool_05_10_PrimeNumbers()
    {
        Logger::log(std::cout, "Press any key to start ...");
        char ch;
        std::cin >> ch;

        Logger::log(std::cout, "Start");

        ScopedTimer clock{};

        size_t foundPrimeNumbers{};

        std::queue<std::future<bool>> results;

        ThreadPool pool{};

        pool.start();         // <=== add/remove comment

        Logger::log(std::cout, "Enqueuing tasks");

        Logger::enableLogging(true);

        for (size_t i{ Globals::Start }; i < Globals::End; i += 2) {

            std::future<bool> future{ pool.addTask(isPrime, i) };

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

        Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Globals::Start, " and ", Globals::End, '.');
        
        pool.stop();

        Logger::log(std::cout, "Done.");
    }

    // =======================================================================

    static void test_concurrency_thread_pool_05_11_PrimeNumbers()
    {
        Logger::log(std::cout, "Press any key to start ...");
        char ch;
        std::cin >> ch;

        Logger::log(std::cout, "Start:");

        ScopedTimer clock{};

        std::atomic_uint64_t foundPrimeNumbers{ 0 };

        std::vector<std::future<std::pair<bool, size_t>>> futures;

        ThreadPool pool{ };

        Logger::log(std::cout, "Enqueuing tasks");

        //std::function<std::pair<bool, size_t>(size_t)> primeTask {
        //    
        //    [&](size_t value) {
        //        bool primeFound { isPrime(value) };
        //        if (primeFound) {
        //            Logger::log(std::cout, "> ", value, " is prime.");
        //            ++foundPrimeNumbers;
        //        }
        //        return std::make_pair(primeFound, value);
        //    }
        //};

        auto primeLambda = [&](size_t value) {

            bool primeFound{ isPrime(value) };

            if (primeFound) {
                Logger::log(std::cout, "> ", value, " is prime.");

                ++foundPrimeNumbers;
            }

            return std::make_pair(primeFound, value);
        };

        Logger::enableLogging(false);

        for (size_t i{ Globals::Start }; i < Globals::End; i += 2) {

            std::future<std::pair<bool, size_t>> future{
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

        Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Globals::Start, " and ", Globals::End, '.');
       
        pool.stop();

        Logger::log(std::cout, "Done.");
    }
}

void test_concurrency_thread_pool_05()
{
    using namespace ThreadPool_ZenSepiol;

    //test_concurrency_thread_pool_05_01();     // just starting ... and stopping the thread pool
    //test_concurrency_thread_pool_05_02();     // launching 5 almost empty tasks
    //test_concurrency_thread_pool_05_03();     // launching many tasks ... and working on an atomic variable (using a pointer)
    //test_concurrency_thread_pool_05_04();     // launching many tasks ... and working on an atomic variable (using a reference)

   // test_concurrency_thread_pool_05_10_PrimeNumbers();      // computing prime numbers, using free function
    test_concurrency_thread_pool_05_11_PrimeNumbers();    // computing prime numbers, using lambda
}

// ===========================================================================
// End-of-File
// ===========================================================================
