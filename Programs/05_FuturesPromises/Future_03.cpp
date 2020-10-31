// ===========================================================================
// Promises and Futures
// ===========================================================================

#include <iostream>
#include <mutex>
#include <thread>
#include <future>

#include "../Logger/Logger.h"

namespace PromisesAndFutures03 {

    using namespace std::chrono_literals;

    // global function
    void product(std::promise<int>&& intPromise, int a, int b) {

        std::thread::id tid = std::this_thread::get_id();
        Logger::log(std::cout, "tid:  ", tid);

        std::this_thread::sleep_for(3s);

        intPromise.set_value(a * b);
    }

    // functor notation
    struct Div {
        void operator() (std::promise<int>&& intPromise, int a, int b) const {

            std::thread::id tid = std::this_thread::get_id();
            Logger::log(std::cout, "tid:  ", tid);

            std::this_thread::sleep_for(5s);
            intPromise.set_value(a / b);
        }
    };

    void test () {

        int a = 20;
        int b = 10;

        // define the promises
        std::promise<int> prodPromise;
        std::promise<int> divPromise;

        // get the futures
        std::future<int> prodResult = prodPromise.get_future();
        std::future<int> divResult = divPromise.get_future();

        // calculate results in a separat thread
        std::thread prodThread(product, std::move(prodPromise), a, b);

        Div div;
        std::thread divThread(div, std::move(divPromise), a, b);

        // get the result
        std::cout << "20 * 10 = " << prodResult.get() << std::endl;
        std::cout << "20 / 10 = " << divResult.get() << std::endl;

        prodThread.join();
        divThread.join();
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

