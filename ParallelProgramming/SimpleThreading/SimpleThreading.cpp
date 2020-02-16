#include <iostream>
#include <thread> 
#include <chrono>
#include "../Logger/Logger.h"

namespace SimpleThreading {

    constexpr long NumIterations = 5;

    void function(int value) {

        std::thread::id tid = std::this_thread::get_id();
        Logger::log(std::cout, "tid:  ", tid);

        for (int i = 0; i < NumIterations; ++i) {
            Logger::log2(std::cout, "in thread ", value);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void test() {
        Logger::log(std::cout, "Begin");

        std::thread::id mainTID = std::this_thread::get_id();
        Logger::log(std::cout, "main: ", mainTID);

        std::thread t1(function, 1);
        std::thread t2(function, 2);

        t1.join();
        t2.join();

        Logger::log(std::cout, "Done.");
    }
}

int main_simple()
{
    using namespace SimpleThreading;
    test();
    return 1;
}
