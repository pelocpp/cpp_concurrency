// ===========================================================================
// ThreadPool_03_ArthurDwyer.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"

#include "ThreadPool_03_ArthurDwyer.h"

#include <iostream>
#include <thread>

namespace ThreadPool_ArthurDwyer
{
    ThreadPool::ThreadPool() : ThreadPool{ 5 } {}

    ThreadPool::ThreadPool(size_t size) {

        Logger::log(std::cout, "Starting Thread Pool with ", size, " threads.");

        m_state.m_aborting = false;

        for (size_t i{}; i != size; ++i) {

            m_threads.emplace_back(std::thread{ [this] () { worker_loop(); } });
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

        Logger::log(std::cout, "Thread Pool: Done.");
    }
}


// ===========================================================================
// End-of-File
// ===========================================================================
