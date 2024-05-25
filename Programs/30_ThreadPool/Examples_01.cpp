// ===========================================================================
// Examples_01.cpp // Thread Pool
// ===========================================================================

#include "ThreadPool_01_Simple.h"
// using namespace ThreadPool_Simple;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

void test_concurrency_thread_pool_01()
{
    using namespace ThreadPool_Simple;

    auto callable = [] () 
        {
        std::stringstream ss;
        ss << "Thread " << std::setw(4) << std::setfill('0')
            << std::uppercase << std::hex << std::this_thread::get_id();

        std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });

        Logger::log(std::cout, "###  > ", ss.str());
    };

    ThreadPool pool{};

    for (size_t i{}; i < 10; ++i) {
        pool.submit(callable);
    }

    Logger::log(std::cout, "Press any key to quit ...");
    char ch;
    std::cin >> ch;
}

// ===========================================================================
// End-of-File
// ===========================================================================
