// ===========================================================================
// PackagedTask_02.cpp // Packaged Task
// ===========================================================================

#include <iostream>
#include <thread>
#include <future>
#include <deque>
#include <utility>

#include "../Logger/Logger.h"

namespace PackagedTask {

    static int calcSum(int a, int b) {
        Logger::log(std::cout, "calcSum: ", a, " + ", b);
        return a + b;
    }

    static int calcSumRange(int a, int b) {
        Logger::log(std::cout, "calcSumRange: ", a, " => ", b);
        int sum{};
        for (int i{ a }; i != b; ++i) {
            sum += i;
        }
        Logger::log(std::cout, "sum: ", sum);
        return sum;
    }

    static void test_01() {

        // define a task
        std::packaged_task<int(int, int)> task{ calcSum };

        // get the future object for this task
        std::future<int> future{ task.get_future() };

        // invoke the function synchronously
        // task(123, 456);

        // or

        // invoke the function asynchronously
        std::thread t{ std::move(task), 123, 456 };
        t.detach();

        // do some arbitrary work ......
        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        // get the result
        int sum{ future.get() };

        Logger::log(std::cout, "123 + 456 = ", sum);
    }

    static void test_02() {

        Logger::log(std::cout, "Start: ");

        constexpr size_t MaxTasks{ 4 };

        std::deque<std::packaged_task<int(int, int)>> tasks;

        std::deque<std::future<int>> futures;

        // define tasks, store corresponding futures
        for (size_t i{}; i != MaxTasks; i++) {

            std::packaged_task<int(int, int)> task{ calcSumRange };

            std::future<int> future{ task.get_future() };

            tasks.push_back(std::move(task));

            futures.push_back(std::move(future));
        }

        int begin{ 1 };
        int increment{ 100 };
        int end{ begin + increment };

        // execute each task in a separate thread
        for (size_t i = 0; i != MaxTasks; i++) {

            std::packaged_task<int(int, int)> task{ std::move(tasks.front()) };
            
            tasks.pop_front();

            // invoke the function synchronously
            // task(begin, end);

            // or
            
            // invoke the function asynchronously
            std::thread t{ std::move(task), begin, end };
            t.detach();

            begin = end;
            end += increment;
        }

        // get the results
        int sum{};
        for (size_t i = 0; i != MaxTasks; i++) {

            std::future<int> future{ std::move(futures.front()) };
            futures.pop_front();

            int partialSum{ future.get() };
            sum += partialSum;
        }

        // use gauss to verify: n * (n+1) / 2 ==> 80200
        Logger::log(std::cout, "Sum of 0 ", " .. ", (end - increment - 1), " = ", sum);

        Logger::log(std::cout, "Done.");
    }
}

void test_packaged_task_02 ()
{
    using namespace PackagedTask;
    test_01();
    test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
