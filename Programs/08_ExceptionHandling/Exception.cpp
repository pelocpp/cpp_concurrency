// ===========================================================================
// Exception.cpp // Exception Handling
// ===========================================================================

#include "../Logger/Logger.h"

#include <iostream>
#include <thread>
#include <future>

namespace ExceptionHandling
{
    /*
     * propagating exception from std::async invocation  
     */
    static int doSomeWorkWithException()
    {
        Logger::log(std::cout, "Inside thread  ... working hard ...");

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        throw std::out_of_range("==> to be passed between threads");

        return 123;
    }

    static void test_01() {

        std::future<int> futureFunction { 
            std::async(std::launch::async, doSomeWorkWithException) 
        };

        try
        {
            Logger::log(std::cout, "Waiting for Result ... ");
            int result{ futureFunction.get() };
        }
        catch (std::out_of_range ex) {
            Logger::log(std::cout, "Main Thread: got exception [", ex.what(), "]");
        }

        Logger::log(std::cout, "Main Thread: Done.");
    }

    /*
     * propagating exception from std::thread invocation
     */

    static std::exception_ptr g_ep{ nullptr };

    static int doAnotherWorkWithException()
    {
        Logger::log(std::cout, "Inside another thread  ... working hard ...");

        try
        {
            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

            throw std::runtime_error("==> to be passed between threads");

            return 123;
        }
        catch (...)
        {
            g_ep = std::current_exception();
        }

        return -1;
    }

    static void test_02() {

        std::thread t{ doAnotherWorkWithException };

        t.join();

        if (g_ep != nullptr) {
            try {
                std::rethrow_exception(g_ep);
            }
            catch (const std::exception & ex)
            {
                Logger::log(std::cout, "Thread exited with exception: [", ex.what(), "]");
            }
        }

        Logger::log(std::cout, "Main Thread: Done.");
    }
}

void test_exception_handling()
{
    using namespace ExceptionHandling;
    test_01();
    test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
