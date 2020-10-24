// ===========================================================================
// Exception Handling
// ===========================================================================

#include <iostream>
#include <thread>
#include <future>

namespace ExceptionHandling
{
    /*
     * propagating exception from std::async invocation  
     */
    int doSomeWorkWithException()
    {
        std::cout << "Inside thread  ... working hard ..." << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));

        throw std::out_of_range("==> to be passed between threads");

        return 123;
    }

    void test_01() {

        std::future<int> futureFunction = std::async(
            std::launch::async,
            doSomeWorkWithException
        );

        try
        {
            std::cout << "Waiting for Result ... " << std::endl;
            int result = futureFunction.get();
        }
        catch (std::out_of_range ex) {
            std::cout << "Main Thread: got exception [" << ex.what() << "]" << std::endl;
        }

        std::cout << "Main Thread: Done." << std::endl;
    }

    /*
     * propagating exception from std::thread invocation
     */

    std::exception_ptr g_ep = nullptr;

    int doAnotherWorkWithException()
    {
        std::cout << "Inside another thread  ... working hard ..." << std::endl;

        try
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));

            throw std::runtime_error("==> to be passed between threads");

            return 123;
        }
        catch (...)
        {
            g_ep = std::current_exception();
        }

        return -1;
    }

    void test_02() {

        std::thread t (doAnotherWorkWithException);
        t.join();

        if (g_ep != nullptr) {
            try {
                std::rethrow_exception(g_ep);
            }
            catch (const std::exception & ex)
            {
                std::cerr << "Thread exited with exception: " << ex.what() << "\n";
            }
        }

        std::cout << "Main Thread: Done." << std::endl;
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
