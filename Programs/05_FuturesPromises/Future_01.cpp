// ===========================================================================
// Promises and Futures
// ===========================================================================

#include <iostream>
#include <thread>
#include <future>

namespace PromisesAndFutures01 {

    using namespace std::chrono_literals;

    void doWorkVersion01(std::promise<int>* promise)
    {
        std::cout << "Inside Thread (doWork - 01)" << std::endl;

        std::this_thread::sleep_for(3s);

        promise->set_value(35);
    }

    void testVersion01() {

        std::promise<int> promiseObj{};

        std::future<int> futureObj{ promiseObj.get_future() };

        std::thread t {doWorkVersion01, &promiseObj };

        std::cout << "Waiting for Result - 01: " << std::endl;
        int result{ futureObj.get() };
        std::cout << "Result: " << result << std::endl;

        t.detach();
    }

    void doWorkVersion02(std::promise<int>&& promise)
    {
        std::cout << "Inside Thread (doWork - 02)" << std::endl;

        std::this_thread::sleep_for(5s);

        promise.set_value(35);
    }

    void testVersion02() {

        std::promise<int> promiseObj{};

        std::future<int> futureObj{ promiseObj.get_future() };

        std::thread t{ doWorkVersion02, std::move (promiseObj) };

        std::cout << "Waiting for Result - 02: " << std::endl;
        int result{ futureObj.get() };
        std::cout << "Result: " << result << std::endl;

        t.detach();
    }
}

void test_future_promise_01()
{
    using namespace PromisesAndFutures01;
    testVersion01();
    testVersion02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
