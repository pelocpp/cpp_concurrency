// ===========================================================================
// ThreadPool_01_Simple.cpp // Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"

#include "ThreadPool_01_Simple.h"

#include <iostream>
#include <thread>

namespace ThreadPool_Simple
{
    ThreadPool::ThreadPool() : m_done{ false }, m_joiner{ m_threads }
    {
        const unsigned int count{ std::thread::hardware_concurrency() };

        try
        {
            for (unsigned i{}; i != count; ++i)
            {
                Logger::log(std::cout, "push_back of next worker_thread function ...");
                std::thread t{ &ThreadPool::worker_thread, this };
                m_threads.push_back(std::move(t));
            }
        }
        catch (...)
        {
            m_done = true;
            throw;
        }

        Logger::log(std::cout, "Created pool with ", count, " threads.");
    }

    ThreadPool::~ThreadPool()
    {
        m_done = true;
    }

    void ThreadPool::worker_thread()
    {
        Logger::log(std::cout, "> worker_thread ...");

        while (!m_done)
        {
            ThreadPoolFunction task{};  // same as std::function<void()>

            if (m_workQueue.tryPop(task))
            {
                task();
            }
            else
            {
                // Logger::log(std::cout, "std::this_thread::yield ...");
                std::this_thread::yield();
            }
        }

        Logger::log(std::cout, "< worker_thread ...");
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
