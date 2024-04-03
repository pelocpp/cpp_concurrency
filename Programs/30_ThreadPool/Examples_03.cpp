// ===========================================================================
// Examples_03.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"

#include "ThreadPool_03_ArthurDwyer.h"
using namespace ThreadPool_ArthurDwyer;


#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

void test_concurrency_thread_pool_03_01()
{
    auto callable = []() {
        std::stringstream ss;
        ss << "Thread " << std::setw(4) << std::setfill('0')
            << std::uppercase << std::hex << std::this_thread::get_id();

        std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });

        Logger::log(std::cout, "###  > ", ss.str());
        };

    ThreadPool pool{3};

    for (size_t i{}; i < 10; ++i) {
        pool.submit(callable);
    }

    Logger::log(std::cout, "Press any key to quit ...");
    char ch;
    std::cin >> ch;
}

// NEU
static auto callable = []() -> int {

    std::cout << "callable\n";
    return 123;
};

static int callableFunc() {

    std::cout << "callableFunc\n";
    return 123;
};

class Callable
{
public:
    auto operator()() -> int {

        std::cout << "callable\n";
        return 123;
    }
};


void test_concurrency_thread_pool_03_02()
{
    ThreadPool pool;

    std::deque<std::future<int>> futures;

    for (int i = 0; i < 10; ++i) {
        std::future<int> f1 = pool.submit(callable);
        futures.push_back(std::move(f1));
        
        // or
        std::future<int> f2 = pool.submit(callableFunc);
        futures.push_back(std::move(f2));
        
        // or
        Callable callableObj{};
        std::future<int> f3 = pool.submit(callableObj);
        futures.push_back(std::move(f3));
    }

    // get the results
    for (size_t i = 0; i != 10; i++) {
        std::future<int> future{ std::move(futures.front()) };
        futures.pop_front();
        int n{ future.get() };
        std::cout << "n =  " << n << std::endl;
    }

    //char ch;
    //std::cin >> ch;
}


void test_concurrency_thread_pool_03_03_Aus_Buch()
{
    std::atomic<int> sum(0);

    ThreadPool tp(4);

    std::vector<std::future<int>> futures;

    for (int i = 0; i < 60000; ++i) {

        auto f = tp.async([i, &sum]() {
            sum += i;
            return i;
            }
        );

        futures.push_back(std::move(f));
    }
    assert(futures[42].get() == 42);
    assert(903 <= sum && sum <= 1799970000);
}


void test_concurrency_thread_pool_03()
{
    test_concurrency_thread_pool_03_01();
}
// ===========================================================================
// End-of-File
// ===========================================================================

