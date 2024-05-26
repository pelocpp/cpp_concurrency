// ===========================================================================
// Examples_03.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"

#include "ThreadPool_03_ArthurDwyer.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

namespace ThreadPool_ArthurDwyer
{
    static void test_concurrency_thread_pool_03_01()
    {
        auto callable = []()
            {
                std::stringstream ss;
                ss << "Thread " << std::setw(4) << std::setfill('0')
                    << std::uppercase << std::hex << std::this_thread::get_id();

                std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });

                Logger::log(std::cout, "###  > ", ss.str());
            };

        ThreadPool pool{ 3 };

        for (size_t i{}; i < 10; ++i) {
            pool.submit(callable);
        }

        Logger::log(std::cout, "Press any key to quit ...");
        char ch;
        std::cin >> ch;
    }

    // NEU
    static auto callable = []() -> int {

        std::cout << "callable\n";
        return 123;
        };

    static int callableFunc() {

        std::cout << "callableFunc\n";
        return 123;
    };

    class Callable
    {
    public:
        auto operator()() -> int {

            std::cout << "callable\n";
            return 123;
        }
    };


    static void test_concurrency_thread_pool_03_02()
    {
        ThreadPool pool;

        std::deque<std::future<int>> futures;

        for (int i = 0; i < 10; ++i) {
            std::future<int> f1 = pool.submit(callable);
            futures.push_back(std::move(f1));

            // or
            std::future<int> f2 = pool.submit(callableFunc);
            futures.push_back(std::move(f2));

            // or
            Callable callableObj{};
            std::future<int> f3 = pool.submit(callableObj);
            futures.push_back(std::move(f3));
        }

        // get the results
        for (size_t i = 0; i != 10; i++) {
            std::future<int> future{ std::move(futures.front()) };
            futures.pop_front();
            int n{ future.get() };
            std::cout << "n =  " << n << std::endl;
        }

        //char ch;
        //std::cin >> ch;
    }


    static void test_concurrency_thread_pool_03_03_Aus_Buch()
    {
        std::atomic<int> sum(0);

        ThreadPool tp(4);

        std::vector<std::future<int>> futures;

        for (int i = 0; i < 60000; ++i) {

            auto f = tp.async([i, &sum]() {
                sum += i;
                return i;
                }
            );

            futures.push_back(std::move(f));
        }
        assert(futures[42].get() == 42);
        assert(903 <= sum && sum <= 1799970000);
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
        size_t end{ static_cast<size_t>(ceil(std::sqrt(number))) };
        for (size_t i{ 3 }; i <= end; i += 2) {

            if (number % i == 0) {
                return false;  // number not prime
            }
        }

        return true; // found prime number
    }

    static void test_concurrency_thread_pool_03_04_PrimeNUmbers()
    {
        Logger::log(std::cout, "Start:");

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

        std::vector<std::future<bool>> futures;


        size_t numThreads{ std::thread::hardware_concurrency() };
        ThreadPool pool{ numThreads };

        Logger::log(std::cout, "Enqueuing tasks");

        //const size_t Start = 1000000000001;
        //const size_t End = Start + 100;

        const size_t Start = 1;
        const size_t End = Start + 100;

        for (size_t i = Start; i < End; i += 2) {

            auto f = pool.async( [i] () {
                return isPrime(i);
                }
            );

            futures.push_back(std::move(f));
        }

        //for (size_t i = Start; i < End; i += 2) {
        //    eventLoop.enqueueTask(primeTask, i);
        //}



        Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Start, " and ", End, '.');

        Logger::log(std::cout, "Done.");
    }
}




void test_concurrency_thread_pool_03()
{
    using namespace ThreadPool_ArthurDwyer;
    //test_concurrency_thread_pool_03_01();
    //test_concurrency_thread_pool_03_02();
    //test_concurrency_thread_pool_03_03_Aus_Buch();

    test_concurrency_thread_pool_03_04_PrimeNUmbers();
}
// ===========================================================================
// End-of-File
// ===========================================================================

