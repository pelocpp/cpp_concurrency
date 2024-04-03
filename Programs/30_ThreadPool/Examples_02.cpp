// ===========================================================================
// Examples_02.cpp // Thread Pool
// ===========================================================================

#include "ThreadPool_02_Simple_Improved.h"
using namespace ThreadPool_Simple_Improved;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

void test_concurrency_thread_pool_02_01()
{
    auto callable = []() {
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

auto callable = []() -> int {

    std::cout << "callable\n";
    return 123;
    };

int callableFunc() {

    std::cout << "callableFunc\n";
    return 123;
};

void test_concurrency_thread_pool_02_02()
{
    ThreadPool pool;

    std::deque<std::future<int>> futures;

    for (int i = 0; i < 10; ++i) {
        std::future<int> f = pool.submit(callableFunc);
        // or
        // std::future<int> f = pool.submitXX(callableFunc);
        futures.push_back(std::move(f));
    }

    // get the results
    for (size_t i = 0; i != 10; i++) {
        std::future<int> future{ std::move(futures.front()) };
        futures.pop_front();
        int n{ future.get() };
        Logger::log(std::cout, "####################### n = ", n);
    }

    Logger::log(std::cout, "Press any key to quit ...");
    char ch;
    std::cin >> ch;
}

void test_concurrency_thread_pool_02()
{
   // test_concurrency_thread_pool_02_01();
    test_concurrency_thread_pool_02_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
