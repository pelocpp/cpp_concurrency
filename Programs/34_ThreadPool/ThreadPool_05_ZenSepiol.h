// ===========================================================================
// ThreadPool_05_ZenSepiol.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus dem Youtube Channel "How to write Thread Pools in C++"
 *
 * https://www.youtube.com/watch?v=6re5U82KwbY
 * 
 * und
 * 
 * https://www.youtube.com/watch?v=meiGRnyRBXM&t=1s
 * 
 * Sources:
 * 
 * https://github.com/ZenSepiol/Dear-ImGui-App-Framework/blob/main/src/lib/thread_pool/thread_pool_test.cpp
 * 
 */

#include "../Logger/Logger.h"

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace ThreadPool_ZenSepiol
{
    using ThreadPoolFunction = std::move_only_function<void(void)>;

    class ThreadPool
    {
    private:
        mutable std::mutex              m_mutex;
        std::condition_variable         m_condition;
        std::vector<std::thread>        m_pool;
        std::queue<ThreadPoolFunction>  m_queue;
        size_t                          m_threads_count;
        size_t                          m_busy_threads;
        bool                            m_shutdown_requested;

    public:
        // c'tors/d'tor
        ThreadPool();
        ~ThreadPool();

        // no copying or moving
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        // public interface
        void start();
        void stop();

        template <typename F, typename... Args>
        auto addTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
        {
            Logger::log(std::cout, "addTask ...");

            auto func{ std::bind(std::forward<F>(f), std::forward<Args>(args)...) };

            auto task{ std::packaged_task<decltype( f(args...)) (void) > { func }};

            auto future = task.get_future();

            // generalized lambda capture
            auto wrapper = [task = std::move(task)]() mutable { task(); };

            {
                std::lock_guard<std::mutex> guard{ m_mutex };
                m_queue.push(std::move(wrapper));
            }

            // wake up one waiting thread if any
            m_condition.notify_one();

            // return future from packaged_task
            return future;
        }

        // getter
        bool empty() const;
        size_t size() const;

    private:
        void worker();
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
