// ===========================================================================
// Packaged Task
// ===========================================================================

#include <iostream>
#include <thread>
#include <future>
#include <deque>
#include <utility>

namespace PackagedTask {

    static int calcSum(int a, int b) {
        return a + b;
    }

    static int calcSumRange(int a, int b) {
        int sum{};
        for (int i{ a }; i != b; ++i) {
            sum += i;
        }
        return sum;
    }

    static void test_01() {

        // define a task
        std::packaged_task<int(int, int)> task{ calcSum };

        // get the future object for this task
        std::future<int> future{ task.get_future() };

        // invoke the function
        task(123, 456);

        // do some arbitrary work ......
        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        // get the result
        int sum{ future.get() };

        std::cout << "123 + 456 = " << sum << std::endl;
    }

    static void test_02() {

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

            task(begin, end);

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
        std::cout
            << "Sum of 0 " << " .. " << (end-increment-1) 
            << " = " << sum << std::endl;
    }
}

void test_packaged_task()
{
    using namespace PackagedTask;
    test_01();
    test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
