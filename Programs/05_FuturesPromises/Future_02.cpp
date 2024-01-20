// ===========================================================================
// Promises and Futures
// ===========================================================================

#include <iostream>
#include <future>

#include "../Logger/Logger.h"

namespace PromisesAndFutures02 {

    void test()
    {
        Logger::log(std::cout, "Main: Start ...");

        std::promise<void> thread1Started;
        std::promise<void> thread2Started;

        std::promise<int> signalPromise;

        std::shared_future<int> signalFuture{ signalPromise.get_future().share() };

        auto function1 = [&thread1Started, signalFuture] {
            Logger::log(std::cout, "Lambda 01:");

            thread1Started.set_value();

            std::this_thread::sleep_for(std::chrono::seconds{ 2 });

            // wait until parameter is set
            int parameter = signalFuture.get();
            Logger::log(std::cout, "Lambda 01 - get returned ", parameter);

            // ... now thread starts to work ...
            std::this_thread::sleep_for(std::chrono::seconds{ 2 });
            Logger::log(std::cout, "End of Lambda 01.");
        };

        auto function2 = [&thread2Started, signalFuture] {
            Logger::log(std::cout, "Lambda 02: ");

            thread2Started.set_value();

            std::this_thread::sleep_for(std::chrono::seconds{ 4 });

            // wait until parameter is set
            int parameter = signalFuture.get();
            Logger::log(std::cout, "Lambda 02 - get returned ", parameter);

            // ... now thread starts to work ...
            std::this_thread::sleep_for(std::chrono::seconds{ 2 });
            Logger::log(std::cout, "End of Lambda 02.");
        };

        Logger::log(std::cout, "Main: (1) ...");

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        // run both lambda expressions asynchronously.
        // Remember to capture the future returned by async()!
        auto result1 = std::async(std::launch::async, function1);
        auto result2 = std::async(std::launch::async, function2);

        Logger::log(std::cout, "Main: (2) ...");

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        Logger::log(std::cout, "Main: vor get_future().wait() ...");

        // wait until both threads have started.
        thread1Started.get_future().wait();
        thread2Started.get_future().wait();

        // both threads are now waiting for the parameter,
        // set the parameter to wake up both of them.
        std::this_thread::sleep_for(std::chrono::seconds{ 2 });
        Logger::log(std::cout, "Main: vor set_value ...");
        signalPromise.set_value(42);

        Logger::log(std::cout, "End of Main");
    }
}

void test_future_promise_02()
{
    using namespace PromisesAndFutures02;
    test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
