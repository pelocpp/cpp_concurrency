// ===========================================================================
// Eager vs Lazy ASync
// ===========================================================================

#include <future>
#include <iostream>
#include <string>
#include <chrono>

namespace EagerVsLazyASync {

    void test_01() {

        std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();

        std::future<std::chrono::system_clock::time_point> asyncLazy
            = std::async(std::launch::deferred, [] {
                return std::chrono::system_clock::now(); 
            }
        );

        std::future<std::chrono::system_clock::time_point> asyncEager 
            = std::async(std::launch::async, [] {
                return std::chrono::system_clock::now();
            }
        );

        std::this_thread::sleep_for(std::chrono::seconds(4));

        std::chrono::system_clock::duration lazyStart = asyncLazy.get() - begin;
        std::chrono::system_clock::duration eagerStart = asyncEager.get() - begin;

        double lazyDuration = std::chrono::duration<double>(lazyStart).count();
        double eagerDuration = std::chrono::duration<double>(eagerStart).count();

        std::cout
            << "asyncLazy evaluated after : " << lazyDuration 
            << " seconds." << std::endl;

        std::cout
            << "asyncEager evaluated after: " << eagerDuration 
            << " seconds." << std::endl;
    }
}

void test_async_02() {

    using namespace EagerVsLazyASync;
    test_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
