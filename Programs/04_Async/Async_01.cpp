// ===========================================================================
// std::async
// ===========================================================================

#include <future>
#include <iostream>
#include <string>

namespace SimpleAsync {

    std::string helloFunction(std::string s) {

        std::this_thread::sleep_for(std::chrono::seconds(3));
        return "Hello ASync from " + s + ".";
    }

    class HelloFunctor {
    public:
        std::string operator()(std::string s) const {

            std::this_thread::sleep_for(std::chrono::seconds(3));
            return "Hello ASync from " + s + ".";
        }
    };

    auto helloLambda =
        [](std::string s) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return "Hello ASync from " + s + ".";
    };

    void test_01() {

        // std::async with function
        std::future<std::string> futureFunction{ 
            std::async(
            helloFunction,
            "function") 
        };

        // std::async with functor
        HelloFunctor helloFunctor{};
        std::future<std::string> futureFunctionObject{ std::async(
            helloFunctor,
            "functor")
        };

        // std::async with lambda function
        std::future<std::string> futureLambda{ std::async(
            helloLambda,
            "lambda")
        };

        std::cout << futureFunction.get() << std::endl;
        std::cout << futureFunctionObject.get() << std::endl;
        std::cout << futureLambda.get() << std::endl;
    }
}

void test_async_01() 
{
    using namespace SimpleAsync;
    test_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
