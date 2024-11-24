// ===========================================================================
// Async_01.cpp // std::async
// ===========================================================================

#include <thread>
#include <chrono>
#include <future>
#include <iostream>
#include <string>

namespace SimpleAsync {

    static std::string hello(std::string s) {

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });
        return "Hello ASync from " + s + ".";
    }

    class Hello {
    public:
        std::string operator() (std::string s) const {
            std::this_thread::sleep_for(std::chrono::seconds{ 3 });
            return "Hello ASync from " + s + ".";
        }
    };

    auto helloLambda =
        [](std::string s) {
        std::this_thread::sleep_for(std::chrono::seconds{ 3 });
        return "Hello ASync from " + s + ".";
    };

    static void test_01() {

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

        std::cout << futureFunction.get() << std::endl;
        std::cout << futureFunctionObject.get() << std::endl;
        std::cout << futureLambda.get() << std::endl;
    }

    static void test_02() {

        auto threadLambda =
            [] () {
                std::this_thread::sleep_for(std::chrono::seconds{ 5 });
                return 123;
        };

        std::future<int> future{ 
            std::async(threadLambda) 
        };

        std::cout << "waiting ..." << std::endl;

        std::future_status status;

        do
        {
            status = future.wait_for(std::chrono::seconds{ 1 });

            switch (status)
            {
            case std::future_status::deferred:
                std::cout << "Deferred\n";
                break;
            case std::future_status::timeout:
                std::cout << "Timeout\n";
                break;
            case std::future_status::ready:
                std::cout << "Ready!\n";
                break;
            }
        }
        while (status != std::future_status::ready);

        std::cout << "Result is " << future.get() << '\n';
    }
}

void test_async_01() 
{
    using namespace SimpleAsync;
    // test_01();
    test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
