// ===========================================================================
// ThreadPool_05_ZenSepiol.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus dem Buch Youtube Channel "How to write Thread Pools in C++"
 *
 * https://www.youtube.com/watch?v=6re5U82KwbY
 *
 */

#include "ThreadPool_05_ZenSepiol.h"

#include <iostream>

namespace ThreadPool_ZenSepiol
{
    ThreadPool::ThreadPool(size_t size)
        : m_busy_threads{ size }, m_shutdown_requested{ false }
    {
        m_threads.resize(size);

        for (size_t i = 0; i < size; ++i)
        {
            m_threads[i] = std::thread(&ThreadPool::worker, this);
        }
    }

    ThreadPool::~ThreadPool()
    {
        shutdown();
    }

    size_t ThreadPool::size()
    {
        std::unique_lock<std::mutex> guard{ m_mutex };
        return m_queue.size();
    }

    void ThreadPool::shutdown()
    {
        // waits until threads finish their current task and shutdowns the pool

        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_shutdown_requested = true;
            m_condition_variable.notify_all();
        }

        for (size_t i{}; i != m_threads.size(); ++i)
        {
            if (m_threads[i].joinable())
            {
                m_threads[i].join();
            }
        }
    }

    void ThreadPool::worker()
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "Started worker [", tid, "]");

        std::unique_lock<std::mutex> guard{ m_mutex };

        while (!m_shutdown_requested || (m_shutdown_requested && !m_queue.empty()))
        {
            m_busy_threads--;

            m_condition_variable.wait(guard, [this] {
                return m_shutdown_requested || !m_queue.empty();
                }
            );

            m_busy_threads++;

            if (!this->m_queue.empty())
            {
                auto func{ std::move(m_queue.front()) };
                m_queue.pop();

                guard.unlock();

                func();

                guard.lock();
            }
        }

        Logger::log(std::cout, "Done [", tid, "]");
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================

