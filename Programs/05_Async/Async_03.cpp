// ===========================================================================
// std::async // eager (std::launch::async) vs lazy (std::launch::deferred)
// ===========================================================================

#include <iostream>
#include <future>

#include "../Logger/Logger.h"

namespace SomeAsyncScenarios {

    static long fib(long n)
    {
        return n <= 1 ? n : fib(n - 1) + fib(n - 2);
    }

    static void test_01()
    {
        std::future<long> f40{ std::async(std::launch::async, fib, 40) };
        std::future<long> f41{ std::async(std::launch::async, fib, 41) };
        std::future<long> f42{ std::async(std::launch::async, fib, 42) };
        std::future<long> f43{ std::async(std::launch::async, fib, 43) };

        std::cout << "Please wait ..." << std::endl;

        /* ... here could be more calculations on the current thread ... */

        std::cout << "fib(40): " << f40.get() << std::endl;   // Output: fib(40): 102334155
        std::cout << "fib(41): " << f41.get() << std::endl;   // Output: fib(41): 165580141
        std::cout << "fib(42): " << f42.get() << std::endl;   // Output: fib(42): 267914296
        std::cout << "fib(43): " << f43.get() << std::endl;   // Output: fib(43): 433494437

        std::cout << "Done." << std::endl;
    }

    static void test_02()
    {
        std::future<long> f40{ std::async(std::launch::async, fib, 40) };
        std::future<long> f41{ std::async(std::launch::async, fib, 41) };
        std::future<long> f42{ std::async(std::launch::async, fib, 42) };
        std::future<long> f43{ std::async(std::launch::async, fib, 43) };

        std::cout << "Please wait ..." << std::endl;

        /* ... here could be more calculations on the current thread ... */

        // ... now just retrieve result of one calculation
        std::cout << "fib(40): " << f40.get() << std::endl;   // output: fib(40): 102334155

        std::cout << "Done." << std::endl;

    }   // end of other calculations is awaited in the d'tor of the corresponding std::future

    static void test_03()
    {
        std::cout << "Preparing calculations ..." << std::endl;
        std::vector<std::future<long>> fibonaccis;

        for (long n{}; n != 50; ++n)
        {
            std::launch policy{ std::launch::deferred };  // <==== std::launch::deferred vs std::launch::async

            std::future<long> fut = std::async(policy, fib, n);

            fibonaccis.push_back(std::move(fut));
        }

        // retrieve single result of fib(42):
        std::cout << "Retrieve single result of fib(42):" << std::endl;
        std::cout << "fib(42): " << fibonaccis[42].get() << std::endl;   // output: fib(42): 267914296

        std::cout << "Done." << std::endl;

    }   // end of other calculations is awaited in the d'tor of the corresponding std::future:
        // depends on launch policy !!!

    static void test_04()
    {
        Logger::log(std::cout, "Calculation with wait:");

        std::future<long> f42 = std::async(fib, 42);

        Logger::startWatch();

        while (true) {
            std::future_status done = f42.wait_for(std::chrono::milliseconds{ 500 });
            if (done == std::future_status::timeout) {
                Logger::log(std::cout, "not yet calculated ...");
            }
            else {
                break;
            }
        }

        // retrieve result
        Logger::log(std::cout, "fib(42): ", f42.get());  // output: fib(42): 267914296
        Logger::stopWatchMilli(std::cout);
    }
}

void test_async_03() {

    using namespace SomeAsyncScenarios;

    test_01();
    test_02();
    test_03();
    test_04();
}

// ===========================================================================
// End-of-File
// ===========================================================================
