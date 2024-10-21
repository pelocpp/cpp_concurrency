// ===========================================================================
// Examples_04.cpp // Thread Pool
// ===========================================================================

#include <iostream>
#include <mutex>
#include <cstdlib>

// #include "pool.h"
using namespace std;

mutex cout_lock;
#define trace(x) { scoped_lock<mutex> lock(cout_lock); cout << x << endl; }

const int COUNT = thread::hardware_concurrency();
const int WORK = 10'000'000;

//int main(int argc, char** argv)
//{
//    srand((unsigned int)time(NULL));
//
//    thread_pool pool;
//
//    auto result = pool.enqueue_task([](int i) { return i; }, 0xFF);
//    result.get();
//
//    for (int i = 1; i <= COUNT; ++i)
//        pool.enqueue_work([](int workerNumber) {
//        int workOutput = 0;
//        int work = WORK + (rand() % (WORK));
//        trace("work item " << workerNumber << " starting " << work << " iterations...");
//        for (int w = 0; w < work; ++w)
//            workOutput += rand();
//        trace("work item " << workerNumber << " finished");
//            }, i);
//
//    return 1;
//}

void test_concurrency_thread_pool_04()
{

}

// ===========================================================================
// End-of-File
// ===========================================================================
