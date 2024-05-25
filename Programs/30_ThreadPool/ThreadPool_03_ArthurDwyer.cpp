// ===========================================================================
// ThreadPool_03_ArthurDwyer.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"

#include "ThreadPool_03_ArthurDwyer.h"

#include <iostream>
#include <thread>

namespace ThreadPool_ArthurDwyer
{

    //  Logger::log(std::cout, "Created pool with ", count, " threads.");

    ThreadPool::ThreadPool() : ThreadPool{ 5 } {}

    ThreadPool::ThreadPool(int size) {

        m_state.m_aborting = false;

        for (size_t i{}; i != size; ++i) {
            m_threads.emplace_back([this]() { worker_loop(); });
        }
    }

    ThreadPool::~ThreadPool() {
        //if (std::lock_guard lk(m_state.m_mutex); true) {
        //    m_state.m_aborting = true;
        //}

        {
            std::lock_guard guard(m_state.m_mutex);
            m_state.m_aborting = true;
        }

        m_condition.notify_all();

        for (std::thread& t : m_threads) {
            t.join();
        }
    }

}


// ===========================================================================
// End-of-File
// ===========================================================================
