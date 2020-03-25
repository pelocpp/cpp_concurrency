// ===========================================================================
// ASync
// ===========================================================================

#include <future>
#include <iostream>
#include <string>

namespace SimpleAsync {

    std::string helloFunction(const std::string& s) {

        std::this_thread::sleep_for(std::chrono::seconds(2));
        return "Hello ASync from " + s + ".";
    }

    class HelloFunctor {
    public:
        std::string operator()(const std::string& s) const {

            std::this_thread::sleep_for(std::chrono::seconds(2));
            return "Hello ASync from " + s + ".";
        }
    };

    auto helloLambda = [](const std::string& s) {

        std::this_thread::sleep_for(std::chrono::seconds(2));
        return "Hello ASync from " + s + "."; 
    };

    void test_01() {

        // std::async with function
        std::future<std::string> futureFunction = std::async(
            std::launch::async,
            helloFunction,
            "function");

        // std::async with functor
        HelloFunctor helloFunctor;
        std::future<std::string> futureFunctionObject = std::async(
            std::launch::async,
            helloFunctor, 
            "functor");

        // std::async with lambda function
        std::future<std::string> futureLambda = std::async(
            std::launch::async,
            helloLambda,
            "lambda");

        std::cout 
            << futureFunction.get() << std::endl
            << futureFunctionObject.get() << std::endl
            << futureLambda.get() << std::endl;
    }
}

//int main(){
//
//    using namespace SimpleAsync;
//    test_01();
//    return 1;
//}

// ===========================================================================
// End-of-File
// ===========================================================================
