//// ===========================================================================
//// Examples_03.cpp // Thread Pool
//// ===========================================================================
//
//#include "../Logger/Logger.h"
//
//#include "ThreadPool_03_ArthurDwyer.h"
//using namespace ThreadPool_ArthurDwyer;
//
//
//#include <iostream>
//#include <iomanip>
//#include <sstream>
//#include <thread>
//#include <chrono>
//
//void test_concurrency_thread_pool_10()
//{
//    auto callable = []() {
//        std::stringstream ss;
//        ss << "Thread " << std::setw(4) << std::setfill('0')
//            << std::uppercase << std::hex << std::this_thread::get_id();
//
//        std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
//
//        Logger::log(std::cout, "###  > ", ss.str());
//        };
//
//    ThreadPool pool{};
//
//    for (size_t i{}; i < 10; ++i) {
//        pool.submit(callable);
//    }
//
//    Logger::log(std::cout, "Press any key to quit ...");
//    char ch;
//    std::cin >> ch;
//}
//
//// NEU
//static auto callable = []() -> int {
//
//    std::cout << "callable\n";
//    return 123;
//};
//
//static int callableFunc() {
//
//    std::cout << "callableFunc\n";
//    return 123;
//};
//
//
//class Callable
//{
//public:
//    auto operator()() -> int {
//
//        std::cout << "callable\n";
//        return 123;
//    }
//};
//
//
//void test_concurrency_thread_pool_11()
//{
//    ThreadPool pool;
//
//    std::deque<std::future<int>> futures;
//
//    for (int i = 0; i < 10; ++i) {
//        std::future<int> f1 = pool.submit(callable);
//        futures.push_back(std::move(f1));
//        
//        // or
//        std::future<int> f2 = pool.submit(callableFunc);
//        futures.push_back(std::move(f2));
//        
//        // or
//        Callable callableObj{};
//        std::future<int> f3 = pool.submit(callableObj);
//        futures.push_back(std::move(f3));
//    }
//
//    // get the results
//    for (size_t i = 0; i != 10; i++) {
//        std::future<int> future{ std::move(futures.front()) };
//        futures.pop_front();
//        int n{ future.get() };
//        std::cout << "n =  " << n << std::endl;
//    }
//
//    //char ch;
//    //std::cin >> ch;
//}
//
//// ===========================================================================
//// End-of-File
//// ===========================================================================
//
//// testing class template std::invoke_result
//
//template<typename FunctionType>
//void testFunction(FunctionType func)
//{
//#if defined (_MSC_VER)
//    std::cout << __FUNCSIG__ << std::endl;
//#endif
//
//#if defined (__GNUC__)
//    std::cout << __PRETTY_FUNCTION__ << std::endl;
//#endif
//
//    using result_type = typename std::invoke_result<FunctionType>::type;
//
//    result_type xxx{};
//}
//
//template<typename FunctionType>
//void testFunctionSubmit(FunctionType func)
//{
//    using result_type = typename std::invoke_result<FunctionType>::type;
//
//    // invoking an instance of type FunctionType(such as f) with no arguments
//    std::packaged_task<result_type()> task1(std::move(func));
//    // or
//    std::packaged_task task2  (std::move(func));             // using CTAD
//
//    std::future future(task1.get_future());
//
//    task1();
//
//    int result = future.get();
//
//    std::cout << "Ergebnis = " << result << "\n";
//
//    result_type xxx{ result };
//}
//
//template<typename FunctionType>
//using result_type = typename std::invoke_result<FunctionType>::type;
//
//template<typename FunctionType>
//std::future<result_type<FunctionType>>
//testFunctionSubmitEx(FunctionType func)
//{
//    // std::packaged_task<result_type()> task(std::move(func));
//    std::packaged_task<result_type<FunctionType>()> task(std::move(func));
//
//    std::future<result_type<FunctionType>> future(task.get_future());
//
//    task();
//
//    result_type<FunctionType> xxx{};
//
//    return future;
//}
//
//
//void test_concurrency_thread_pool_12()
//{
//    testFunction(callable);
//
//    testFunctionSubmit(callableFunc);
//
//    std::future<int> future = testFunctionSubmitEx(callable);
//
//    int result = future.get();
//
//    std::cout << "Ergebnis = " << result << "\n";
//}
