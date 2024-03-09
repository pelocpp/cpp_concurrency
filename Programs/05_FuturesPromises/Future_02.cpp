// ===========================================================================
// Future_02.cpp // Promises and Futures
// ===========================================================================

#include <iostream>
#include <thread>
#include <future>

#include "../Logger/Logger.h"

namespace PromisesAndFutures02 {

    static void test_shared_future_01()
    {
        Logger::log(std::cout, "Start.");

        std::promise<int> provider;

        // transfer the state
        // from the promise generated future
        // to a shared future
        std::future<int> future{ provider.get_future() };
        std::shared_future<int> sharedFuture{ future.share()};

        std::vector<std::jthread> runners;

        // start a new thread, taking a copy of the future
        runners.push_back(std::jthread([sharedFuture]() {
            // block until the promise is fulfilled
            int value = sharedFuture.get();
            Logger::log(std::cout, "Got result ", value);
            }));

        // start a new thread, taking a copy of the future
        runners.push_back(std::jthread([sharedFuture]() {
            // block until the promise is fulfilled
            int value = sharedFuture.get();
            Logger::log(std::cout, "Got result ", value);
            }));

        std::this_thread::sleep_for(std::chrono::seconds(2));

        // fulfill the promise
        provider.set_value(123);

        Logger::log(std::cout, "Done.");
    }

    // same test scenario - just without a std::vector
    static void test_shared_future_02()
    {
        Logger::log(std::cout, "Start.");

        std::promise<int> provider;

        std::future<int> future{ provider.get_future() };
        std::shared_future<int> sharedFuture{ future.share() };

        // start a new thread, taking a copy of the future
        std::jthread thread1 ([sharedFuture]() {
            int value = sharedFuture.get();
            Logger::log(std::cout, "Got result ", value);
        });

        // start a new thread, taking a copy of the future
        std::jthread thread2([sharedFuture]() {
            int value = sharedFuture.get();
            Logger::log(std::cout, "Got result ", value);
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));

        // fulfill the promise
        provider.set_value(123);

        // watch output of "Done" on the console window
        // std::this_thread::sleep_for(std::chrono::seconds(1));

        Logger::log(std::cout, "Done.");
    }

    // same test scenario - std::vector resides in it's own scope now
    static void test_shared_future_03()
    {
        Logger::log(std::cout, "Start.");

        std::promise<int> provider;

        std::future<int> future{ provider.get_future() };
        std::shared_future<int> sharedFuture{ future.share() };

        {
            std::vector<std::jthread> runners;

            // start a new thread, taking a copy of the future
            runners.push_back(std::jthread([sharedFuture]() {

                int value = sharedFuture.get();
                Logger::log(std::cout, "Got result ", value);
                }));

            // start a new thread, taking a copy of the future
            runners.push_back(std::jthread([sharedFuture]() {

                int value = sharedFuture.get();
                Logger::log(std::cout, "Got result ", value);
                }));

            std::this_thread::sleep_for(std::chrono::seconds(2));

            // fulfill the promise
            provider.set_value(123);
        }

        Logger::log(std::cout, "Done.");
    }
}

void test_future_promise_02()
{
    using namespace PromisesAndFutures02;
    test_shared_future_01();
    test_shared_future_02();
    test_shared_future_03();
}

// ===========================================================================
// End-of-File
// ===========================================================================
