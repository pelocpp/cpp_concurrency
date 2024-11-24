// ===========================================================================
// Future_01.cpp // Promises and Futures
// ===========================================================================

#include "../Logger/Logger.h"

#include <iostream>
#include <thread>
#include <future>

namespace PromisesAndFutures01 {

    using namespace std::chrono_literals;

    static void doWorkVersion01(std::promise<int>* promise)
    {
        Logger::log(std::cout, "Inside Thread - 01");

        std::this_thread::sleep_for(3s);

        promise->set_value(123);
    }

    static void testVersion01() {

        std::promise<int> promiseObj{};

        std::future<int> futureObj{ promiseObj.get_future() };

        std::thread t {doWorkVersion01, &promiseObj };

        Logger::log(std::cout, "Waiting for Result - 01");

        int result{ futureObj.get() };
        Logger::log(std::cout, "Result: ", result);

        t.detach();
    }

    static void doWorkVersion02(std::promise<int>&& promise)
    {
        Logger::log(std::cout, "Inside Thread - 02");

        std::this_thread::sleep_for(5s);

        promise.set_value(123);
    }

    static void testVersion02() {

        std::promise<int> promiseObj{};

        std::future<int> futureObj{ promiseObj.get_future() };

        std::thread t{ doWorkVersion02, std::move (promiseObj) };

        Logger::log(std::cout, "Waiting for Result - 02");

        int result{ futureObj.get() };
        Logger::log(std::cout, "Result: ", result);

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
