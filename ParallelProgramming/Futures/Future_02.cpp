#include <iostream>
#include <thread>
#include <future>

namespace SimplePromises {

    void doWorkVersion01(std::promise<int>* promise)
    {
        std::cout << "Inside Thread (doWork - 01)" << std::endl;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(3s);

        promise->set_value(35);
    }

    void testVersion01() {

        std::promise<int> promiseObj;

        std::future<int> futureObj = promiseObj.get_future();

        std::thread th(doWorkVersion01, &promiseObj);

        std::cout << "Waiting for Result - 01: " << std::endl;
        int result = futureObj.get();

        std::cout << "Result: " << result << std::endl;

        th.join();
    }

    void doWorkVersion02(std::promise<int>&& promise)
    {
        std::cout << "Inside Thread (doWork - 02)" << std::endl;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(3s);

        promise.set_value(35);
    }

    void testVersion02() {

        std::promise<int> promiseObj;

        std::future<int> futureObj = promiseObj.get_future();

        std::thread th(doWorkVersion02, std::move (promiseObj));

        std::cout << "Waiting for Result - 02: " << std::endl;
        int result = futureObj.get();

        std::cout << "Result: " << result << std::endl;

        th.join();
    }
}

//int main()
//{
//    using namespace SimplePromises;
//    // testVersion01();
//    testVersion02();
//
//    return 0;
//}

