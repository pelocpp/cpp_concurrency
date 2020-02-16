#include <iostream>
#include <thread> 
#include <mutex> 
#include <chrono>
#include "../Logger/Logger.h"

namespace MutexDemo {

    constexpr long NumIterations = 5;

    std::mutex m;
    std::condition_variable cv;

    void function(int value) {

        std::thread::id tid = std::this_thread::get_id();
        Logger::log(std::cout, "tid:  ", tid);

        std::unique_lock<std::mutex> lock{ m };
        cv.wait(lock, [&]()
            {
                std::cout << "Waking up ..." << std::endl;
                return true;
            });

        Logger::log(std::cout, "Done Thread");
    }

    void test() {
        Logger::log(std::cout, "Begin");

        std::thread::id mainTID = std::this_thread::get_id();
        Logger::log(std::cout, "main: ", mainTID);

        std::thread t1;
        std::thread t2;

        {
            std::lock_guard<std::mutex> lock(m);

            t1 = std::thread (function, 1);
            t2 = std::thread (function, 2);

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(3s);
        }

        cv.notify_all();

        t1.join();
        t2.join();

        Logger::log(std::cout, "Done Main.");
    }
}

int main_cond()
{
    using namespace MutexDemo;
    test();
    return 1;
}
