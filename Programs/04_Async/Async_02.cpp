// ===========================================================================
// std::async // eager (std::launch::async) vs lazy (std::launch::deferred)
// ===========================================================================

#include <future>
#include <iostream>
#include <string>
#include <chrono>

#include "../Logger/Logger.h"

namespace EagerVsLazyASync {

    static void test_01() {

        Logger::log(std::cout, "Preparing calculations ...");

        std::chrono::system_clock::time_point begin{ std::chrono::system_clock::now() };

        std::future<std::chrono::system_clock::time_point> asyncLazy = std::async(
            std::launch::deferred, 
            [] () {
                Logger::log(std::cout, "launch::deferred thread done!");
                return std::chrono::system_clock::now(); 
            }
        );

        std::future<std::chrono::system_clock::time_point> asyncEager = std::async(
            std::launch::async,
            [] () {
                Logger::log(std::cout, "launch::async thread done!");
                return std::chrono::system_clock::now();
            }
        );

        Logger::log(std::cout, "Now waiting for 5 seconds ...");
        std::this_thread::sleep_for(std::chrono::seconds{ 5 });

        std::chrono::system_clock::duration lazyStart{ asyncLazy.get() - begin };
        std::chrono::system_clock::duration eagerStart{ asyncEager.get() - begin };

        double lazyDuration{ std::chrono::duration<double>(lazyStart).count() };
        double eagerDuration{ std::chrono::duration<double>(eagerStart).count() };

        Logger::log(std::cout, "asyncLazy evaluated after : ", lazyDuration, " seconds.");
        Logger::log(std::cout, "asyncEager evaluated after: ", eagerDuration, " seconds.");
    }
}

void test_async_02()
{
    using namespace EagerVsLazyASync;
    test_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
