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

        // get the result
        int sum = future.get();

        std::cout << "123 + 456 = " << sum << std::endl;
    }

    void test_02() {

        // define the tasks
        std::packaged_task<int(int, int)> sumTask1(calcSumRange);
        std::packaged_task<int(int, int)> sumTask2(calcSumRange);
        std::packaged_task<int(int, int)> sumTask3(calcSumRange);
        std::packaged_task<int(int, int)> sumTask4(calcSumRange);

        // get the futures
        std::future<int> sumResult1 = sumTask1.get_future();
        std::future<int> sumResult2 = sumTask2.get_future();
        std::future<int> sumResult3 = sumTask3.get_future();
        std::future<int> sumResult4 = sumTask4.get_future();

        // push the tasks on the container
        std::deque<std::packaged_task<int(int, int)>> allTasks;
        allTasks.push_back(std::move(sumTask1));
        allTasks.push_back(std::move(sumTask2));
        allTasks.push_back(std::move(sumTask3));
        allTasks.push_back(std::move(sumTask4));

        int begin{ 1 };
        int increment{ 1000 };
        int end = begin + increment;

        // execute each task in a separate thread
        while (! allTasks.empty()) {
            std::packaged_task<int(int, int)> myTask = std::move(allTasks.front());
            allTasks.pop_front();
            std::thread t(std::move(myTask), begin, end);
            begin = end;
            end += increment;
            t.detach();
        }

        // get the results
        int sum = sumResult1.get() + sumResult2.get() + sumResult3.get() + sumResult4.get();

        std::cout << "Sum of 0 .. 10000 = " << sum << std::endl;
    }
}

//int main()
//{
//    using namespace PackagedTask;
//    test_01();
//    test_02();
//    return 1;
//}

// ===========================================================================
// End-of-File
// ===========================================================================
