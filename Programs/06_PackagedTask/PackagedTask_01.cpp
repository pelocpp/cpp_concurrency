// ===========================================================================
// PackagedTask_01.cpp // Packaged Task
// ===========================================================================

#include <iostream>
#include <thread>
#include <future>
#include <chrono>

namespace PackagedTaskComparison {

    // -----------------------------------------------------------------------
    // demonstrating std::packaged_task with thread (asynchronous execution)

    static void test_01() {

        // create packaged_task object
        std::packaged_task<int(void)> task{
            [] () {
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
                return 123;
            }
        };

        // retrieve future object from task
        std::future<int> future{ task.get_future() };

        // create a thread with this task
        std::thread t{ std::move(task) };

        // retrieve result from future object
        int result{ future.get() };
        std::cout << "Result: " << result << std::endl;

        t.join();
    }

    // -----------------------------------------------------------------------
    // demonstrating std::packaged_task without thread (synchronous execution)

    static void test_02() {

        // create packaged_task object
        std::packaged_task<int(void)> task { 
            [] () {
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
                return 123;
            }
        };

        // retrieve future object from task
        std::future<int> future{ task.get_future() };
        
        // execute task
        task();

        // retrieve result from future object
        int result{ future.get() };
        std::cout << "Result: " << result << std::endl;
    }

    // -----------------------------------------------------------------------
    // equivalent code, demonstrating std::function and std::promise

    static void test_03() {

        std::promise<int> promise;

        std::future<int> future{ promise.get_future() };

        std::function<void(std::promise<int>&&)> function {
            [] (std::promise<int>&& promise) {
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
                promise.set_value(123);
            }
        };

        // create a thread with this function
        std::thread t{ std::move(function), std::move(promise) };

        // retrieve result from future object
        int result = future.get();
        std::cout << "Result: " << result << std::endl;

        t.join();
    }
}

void test_packaged_task_01 ()
{
    using namespace PackagedTaskComparison;
    test_01();
    test_02();
    test_03();
}

// ===========================================================================
// End-of-File
// ===========================================================================
