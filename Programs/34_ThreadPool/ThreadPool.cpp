// ===========================================================================
// ThreadPool.cpp // Thread Pool
// ===========================================================================

#include "ThreadPool.h"

ThreadPool::ThreadPool()
    : m_threads_count{}, m_busy_threads{ }, m_shutdown_requested {}
{}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start()
{
    std::size_t numThreads{ std::thread::hardware_concurrency() };

    Logger::log(std::cout, "Number of available concurrent threads: ", numThreads);

    m_pool.resize(numThreads);

    for (std::size_t i{}; i != numThreads; ++i)
    {
        m_pool[i] = std::thread(&ThreadPool::worker, this);
    }

    m_threads_count = numThreads;
    m_busy_threads = numThreads;
}

void ThreadPool::stop()
{
    // waits until threads finish their current task and shutdowns the pool

    {
        std::lock_guard<std::mutex> guard{ m_mutex };
        m_shutdown_requested = true;
    }

    m_condition.notify_all();

    for (std::size_t i{}; i != m_pool.size(); ++i)
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

std::size_t ThreadPool::size() const
{
    std::lock_guard<std::mutex> guard{ m_mutex };
    return m_queue.size();
}

// ===========================================================================
// End-of-File
// ===========================================================================
