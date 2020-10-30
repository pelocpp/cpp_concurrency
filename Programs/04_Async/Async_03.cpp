#include <iostream>
#include <future>

#include "../Logger/Logger.h"

namespace SomeAsyncScenarios {

    long fib(long n)
    {
        return n <= 1 ? n : fib(n - 1) + fib(n - 2);
    }

    void test_01() 
    {
        std::future<long> f40 = std::async(fib, 40);
        std::future<long> f41 = std::async(fib, 41);
        std::future<long> f42 = std::async(fib, 42);
        std::future<long> f43 = std::async(fib, 43);

        std::cout << "Please wait ..." << std::endl;

        /* ... here could be more calculations on the current thread ... */

        std::cout << "fib(40): " << f40.get() << std::endl;   // Output: fib(40): 102334155
        std::cout << "fib(41): " << f41.get() << std::endl;   // Output: fib(41): 165580141
        std::cout << "fib(42): " << f42.get() << std::endl;   // Output: fib(42): 267914296
        std::cout << "fib(43): " << f43.get() << std::endl;   // Output: fib(43): 433494437

        std::cout << "Done." << std::endl;
    }

    void test_02()
    {
        std::future<long> f40 = std::async(fib, 40);
        std::future<long> f41 = std::async(fib, 41);
        std::future<long> f42 = std::async(fib, 42);
        std::future<long> f43 = std::async(fib, 43);

        std::cout << "Please wait ..." << std::endl;

        /* ... here could be more calculations on the current thread ... */

        // ... now just retrieve result of one calculation
        std::cout << "fib(40): " << f40.get() << std::endl;   // output: fib(40): 102334155

        std::cout << "Done." << std::endl;

    }   // end of other calculations is awaited in the d'tor of the corresponding std::future

    void test_03()
    {
        std::cout << "Preparing calculations ..." << std::endl;
        std::vector<std::future<long>> fibonaccis;

        for (int n = 0; n < 50; ++n)
        {
            std::future<long> fut = std::async(std::launch::deferred, fib, n);  // <==== std::launch::deferred with std::launch::async
            fibonaccis.push_back(std::move(fut));
        }

        // retrieve single result of fib(42):
        std::cout << "Retrieve single result of fib(42):" << std::endl;
        std::cout << "fib(42): " << fibonaccis[42].get() << std::endl;   // output: fib(42): 267914296

        std::cout << "Done." << std::endl;

    }   // end of other calculations is awaited in the d'tor of the corresponding std::future:
        // depends on launch policy !!!

    void test_04()
    {
        std::cout << "Calculation with wait:" << std::endl;

        std::future<long> f40 = std::async(fib, 40);

        Logger::startWatch();

        while (true) {
            std::future_status done = f40.wait_for(std::chrono::milliseconds(500));
            if (done == std::future_status::timeout) {
                std::cout << "not yet calculated ..." << std::endl;
            }
            else {
                break;
            }
        }

        // retrieve result
        std::cout << "fib(40): " << f40.get() << std::endl; // output: fib(40): 102334155
        Logger::stopWatchMilli();
    }
}

std::chrono::steady_clock::time_point Logger::begin;

void test_async_03() {

    using namespace SomeAsyncScenarios;
    test_01();
    test_02();
    test_03();
    test_04();
}
