//// ===========================================================================
//// ThreadPool_03_ArthurDwyer.cpp // Thread Pool
//// ===========================================================================
//
//#include "../Logger/Logger.h"
//
//#include "ThreadPool_03_ArthurDwyer.h"
//using namespace ThreadPool_ArthurDwyer;
//
//#include <iostream>
//#include <thread>
//
////  Logger::log(std::cout, "Created pool with ", count, " threads.");
//
//ThreadPool::ThreadPool() : ThreadPool{5} {}
//
//ThreadPool::ThreadPool(int size) {
//    for (int i = 0; i < size; ++i) {
//        m_threads.emplace_back([this]() { worker_loop(); });
//    }
//}
//
//ThreadPool::~ThreadPool() {
//    if (std::lock_guard lk(m_state.m_mutex); true) {
//        m_state.m_aborting = true;
//    }
//
//    m_condition.notify_all();
//
//    for (std::thread& t : m_threads) {
//        t.join();
//    }
//}
//
//// ===========================================================================
//// End-of-File
//// ===========================================================================
