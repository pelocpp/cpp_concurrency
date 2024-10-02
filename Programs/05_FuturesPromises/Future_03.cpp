// ===========================================================================
// Future_03.cpp // Promises and Futures
// ===========================================================================

#include "../Logger/Logger.h"

#include <iostream>
#include <future>

namespace PromisesAndFutures03 {

    static void test()
    {
        Logger::log(std::cout, "Main: Start ...");

        std::promise<void> promiseThread1Started;
        std::promise<void> promiseThread2Started;

        std::promise<int> promiseResult;

        std::future<int> future{ promiseResult.get_future() };
        std::shared_future<int> signalFuture{ future.share() };

        auto function1 = [&promiseThread1Started, signalFuture] {

            Logger::log(std::cout, "Lambda 01:");

            promiseThread1Started.set_value();

            std::this_thread::sleep_for(std::chrono::seconds{ 2 });

            // wait until parameter is set
            int parameter = signalFuture.get();
            Logger::log(std::cout, "Lambda 01 - get returned ", parameter);

            // ... now thread starts to work ...
            std::this_thread::sleep_for(std::chrono::seconds{ 2 });
            Logger::log(std::cout, "End of Lambda 01.");
        };

        auto function2 = [&promiseThread2Started, signalFuture] {

            Logger::log(std::cout, "Lambda 02: ");

            promiseThread2Started.set_value();

            std::this_thread::sleep_for(std::chrono::seconds{ 4 });

            // wait until parameter is set
            int parameter = signalFuture.get();
            Logger::log(std::cout, "Lambda 02 - get returned ", parameter);

            // ... now thread starts to work ...
            std::this_thread::sleep_for(std::chrono::seconds{ 2 });
            Logger::log(std::cout, "End of Lambda 02.");
        };

        Logger::log(std::cout, "Main: before sleep_for ...");

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        // run both lambda expressions asynchronously.
        // (capture the future returned by async()!)
        std::future<void> result1 { std::async(std::launch::async, function1) };
        std::future<void> result2 { std::async(std::launch::async, function2) };

        Logger::log(std::cout, "Main: after starting threads ...");

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        Logger::log(std::cout, "Main: vor wait() ...");

        // wait until both threads have started.
        std::future<void> future1{ promiseThread1Started.get_future() };
        std::future<void> future2{ promiseThread2Started.get_future() };
        future1.wait();
        future2.wait();

        // both threads are now waiting for the parameter,
        // set the parameter to wake up both of them.
        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        Logger::log(std::cout, "Main: vor set_value() ...");
        promiseResult.set_value(123);
        Logger::log(std::cout, "End of Main");
    }
}

void test_future_promise_03()
{
    using namespace PromisesAndFutures03;
    test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
