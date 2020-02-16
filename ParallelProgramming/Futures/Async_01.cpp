#include <future>
#include <iostream>
#include <string>

namespace SimpleAsync {

    std::string helloFunction(const std::string& s) {

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);

        return "Hello ASync from " + s + ".";
    }

    class HelloFunctor {
    public:
        std::string operator()(const std::string& s) const {

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(2s);

            return "Hello ASync from " + s + ".";
        }
    };

    auto helloLambda = [](const std::string& s) {

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(3s);

        return "Hello ASync from " + s + "."; 
    };

    void test_01() {

        // future with function
        auto futureFunction = std::async(helloFunction, "function");

        // future with functor
        HelloFunctor HelloFunctor;
        auto futureFunctionObject = std::async(HelloFunctor, "functor");

        // future with lambda function
        auto futureLambda = std::async(helloLambda, "lambda");

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