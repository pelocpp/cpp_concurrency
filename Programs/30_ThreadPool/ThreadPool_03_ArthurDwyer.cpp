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

    ThreadPool::ThreadPool(size_t size) : m_aborting{ false }
    {
        Logger::log(std::cout, "Starting Thread Pool with ", size, " threads.");

        for (size_t i{}; i != size; ++i) {

            m_threads.emplace_back(std::thread{ [this] () { worker_loop(); } });
        }
    }

    ThreadPool::~ThreadPool() 
    {
        {
            std::lock_guard<std::mutex> guard{ m_mutex };

            m_aborting = true;
        }

        m_condition.notify_all();

        for (auto& t : m_threads) {
            t.join();
        }

        Logger::log(std::cout, "Thread Pool: Done.");
    }
}


// ===========================================================================
// End-of-File
// ===========================================================================
