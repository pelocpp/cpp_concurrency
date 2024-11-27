// ===========================================================================
// Async_01.cpp // std::async
// ===========================================================================

#include <thread>
#include <chrono>
#include <future>
#include <iostream>
#include <string>

#include "../Logger/Logger.h"

namespace SimpleAsync {

    static std::string hello(std::string s) {

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });
        std::string result{ "Hello ASync from " + s + "." };
        Logger::log(std::cout, "returning ", result);
        return result;
    }

    class Hello {
    public:
        std::string operator() (std::string s) const {
            std::this_thread::sleep_for(std::chrono::seconds{ 3 });
            std::string result{ "Hello ASync from " + s + "." };
            Logger::log(std::cout, "returning ", result);
            return result;
        }
    };

    auto helloLambda =
        [](std::string s) {
        std::this_thread::sleep_for(std::chrono::seconds{ 3 });
        std::string result{ "Hello ASync from " + s + "." };
        Logger::log(std::cout, "returning ", result);
        return result;
    };

    static void test_01() {

        Logger::log(std::cout, "Start std::async's:");

        // std::async with function
        std::future<std::string> futureFunction{ 
            std::async( hello, "function") 
        };

        // std::async with callable object
        Hello helloObject{};
        std::future<std::string> futureFunctionObject {
            std::async(helloObject, "callable object")
        };

        // std::async with lambda function
        std::future<std::string> futureLambda {
            std::async(helloLambda, "lambda")
        };

        auto result1{ futureFunction.get() };
        Logger::log(std::cout, "result1: ", result1);

        auto result2{ futureFunctionObject.get() };
        Logger::log(std::cout, "result2: ", result2);

        auto result3{ futureLambda.get() };
        Logger::log(std::cout, "result3: ", result3);
    }

    static void test_02() {

        Logger::log(std::cout, "Start std::async:");

        auto lambda =
            [] () {
                std::this_thread::sleep_for(std::chrono::seconds{ 5 });
                return 123;
        };

        std::future<int> future{ 
            std::async(lambda)
        };

        Logger::log(std::cout, "waiting");

        std::future_status status;

        do
        {
            status = future.wait_for(std::chrono::seconds{ 1 });

            switch (status)
            {
            case std::future_status::deferred:
                Logger::log(std::cout, "Deferred");
                break;
            case std::future_status::timeout:
                Logger::log(std::cout, "Timeout");
                break;
            case std::future_status::ready:
                Logger::log(std::cout, "Ready");
                break;
            }
        }
        while (status != std::future_status::ready);

        auto result{ future.get() };

        Logger::log(std::cout, "Result: ", result);
    }
}

void test_async_01() 
{
    using namespace SimpleAsync;
    test_01();
    test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
