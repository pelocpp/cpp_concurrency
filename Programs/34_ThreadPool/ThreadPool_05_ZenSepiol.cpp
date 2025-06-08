// ===========================================================================
// ThreadPool_05_ZenSepiol.cpp // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus dem Youtube Channel "How to write Thread Pools in C++"
 *
 * https://www.youtube.com/watch?v=6re5U82KwbY
 *
 */

#include "ThreadPool_05_ZenSepiol.h"

namespace ThreadPool_ZenSepiol
{
    ThreadPool::ThreadPool()
        : m_threads_count{}, m_busy_threads{ }, m_shutdown_requested {}
    {}

    ThreadPool::~ThreadPool()
    {
        stop();
    }

    void ThreadPool::start()
    {
        size_t size{ std::thread::hardware_concurrency() };

        m_pool.resize(size);

        for (size_t i{}; i != size; ++i)
        {
            m_pool[i] = std::thread(&ThreadPool::worker, this);
        }

        m_threads_count = size;
        m_busy_threads = size;
    }

    void ThreadPool::stop()
    {
        // waits until threads finish their current task and shutdowns the pool

        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_shutdown_requested = true;
        }

        m_condition.notify_all();

        for (size_t i{}; i != m_pool.size(); ++i)
        {
            if (m_pool[i].joinable())
            {
                m_pool[i].join();
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

            m_condition.wait(guard, [this] {
                return m_shutdown_requested || !m_queue.empty();
            });

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

        Logger::log(std::cout, "Worker Done [", tid, "]");
    }

    bool ThreadPool::empty() const
    {
        std::lock_guard<std::mutex> guard{ m_mutex };
        return m_queue.empty();
    }

    size_t ThreadPool::size() const
    {
        std::lock_guard<std::mutex> guard{ m_mutex };
        return m_queue.size();
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================

