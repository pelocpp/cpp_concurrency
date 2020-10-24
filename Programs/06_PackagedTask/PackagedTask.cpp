// ===========================================================================
// Packaged Task
// ===========================================================================

#include <iostream>
#include <thread>
#include <future>
#include <deque>
#include <utility>

#include "../Logger/Logger.h"

namespace PackagedTask {

    int calcSum(int a, int b) { return a + b; }

    int calcSumRange(int a, int b) {
        int sum{ 0 };
        for (int i = a; i < b; ++i) sum += i;
        return sum;
    }

    void test_01() {

        // define a task
        std::packaged_task<int(int, int)> task(calcSum);

        // get the future object for this task
        std::future<int> future = task.get_future();

        std::thread t{ std::move(task), 123, 456 };
        t.detach();

        // do some arbitrary work ......
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // get the result
        int sum = future.get();

        std::cout << "123 + 456 = " << sum << std::endl;
    }

    void test_02() {

        constexpr int MaxTasks = 4;

        std::deque<std::packaged_task<int(int, int)>> tasks;
        std::deque<std::future<int>> futures;

        // define tasks, store corresponding futures
        for (size_t i = 0; i < MaxTasks; i++) {

            std::packaged_task<int(int, int)> task(calcSumRange);

            std::future<int> future = task.get_future();

            tasks.push_back(std::move(task));

            futures.push_back(std::move(future));
        }

        int begin{ 0 };
        int increment{ 100 };
        int end = begin + increment;

        // execute each task in a separate thread
        for (size_t i = 0; i < MaxTasks; i++) {

            std::packaged_task<int(int, int)> task = std::move(tasks.front());
            tasks.pop_front();

            std::thread t(std::move(task), begin, end);
            t.detach();

            begin = end;
            end += increment;
        }

        // get the results
        int sum{};
        for (size_t i = 0; i < MaxTasks; i++) {

            std::future<int> future = std::move(futures.front());
            futures.pop_front();

            int partialSum = future.get();
            sum += partialSum;
        }

        std::cout << "Sum of 0 " << " .. " << (end-increment-1) << " = " << sum << std::endl;
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
