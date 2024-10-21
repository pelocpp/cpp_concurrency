// ===========================================================================
// ThreadPool_05_ZenSepiol.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus dem Buch Youtube Channel "How to write Thread Pools in C++"
 *
 * https://www.youtube.com/watch?v=6re5U82KwbY
 * 
 * Sources:
 * 
 * https://github.com/ZenSepiol/Dear-ImGui-App-Framework/blob/main/src/lib/thread_pool/thread_pool_test.cpp
 * 
 */

#include "../Logger/Logger.h"

#include <iostream>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include <future>
#include <type_traits>

namespace ThreadPool_ZenSepiol
{
    class ThreadPool
    {
    private:
        mutable std::mutex                               m_mutex;
        std::condition_variable                          m_condition_variable;
        std::vector<std::thread>                         m_threads;
        std::queue<std::move_only_function<void(void)>>  m_queue;
        bool                                             m_shutdown_requested;
        size_t                                           m_busy_threads;

    public:
        ThreadPool(size_t size);
        ~ThreadPool();

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;

        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        void shutdown();

        template <typename F, typename... Args>
        auto addTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
        {
            auto func{ std::bind(std::forward<F>(f), std::forward<Args>(args)...) }; 

            auto task{ std::packaged_task<decltype( f(args...)) (void) > { func }};

            auto future = task.get_future();

            // generalized lambda capture
            auto wrapper = [task = std::move(task)]() mutable { task(); };

            {
                std::lock_guard<std::mutex> guard{ m_mutex };

                m_queue.push(std::move(wrapper));

                // wake up one waiting thread if any
                m_condition_variable.notify_one();
            }

            // return future from packaged_task
            return future;
        }

        size_t size();

    private:
        void worker();
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
