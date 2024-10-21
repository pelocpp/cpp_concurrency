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
        ThreadPool(size_t size) 
            : m_busy_threads{ size }, m_shutdown_requested{ false }
        {
            m_threads.resize(size);

            for (size_t i = 0; i < size; ++i)
            {
                m_threads[i] = std::thread(&ThreadPool::worker, this);
            }
        }

        ~ThreadPool()
        {
            shutdown();
        }

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;

        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        // waits until threads finish their current task and shutdowns the pool
        void shutdown()
        {
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

        size_t size()
        {
            std::unique_lock<std::mutex> guard{ m_mutex };
            return m_queue.size();
        }


        void worker()
        {
            std::thread::id tid{ std::this_thread::get_id() };
            Logger::log(std::cout, "Started worker [", tid, "]");

            std::unique_lock<std::mutex> guard{ m_mutex };

            while (!m_shutdown_requested || (m_shutdown_requested && !m_queue.empty()))
            {
                m_busy_threads--;

                m_condition_variable.wait(guard, [this] {
                    return this->m_shutdown_requested || !this->m_queue.empty();
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

    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

//
//namespace ThreadPool_ZenSepiol
//{
//    class ThreadPool
//    {
//    private:
//        mutable std::mutex                               m_mutex;
//        std::condition_variable                          m_condition_variable;
//        std::vector<std::thread>                         m_threads;
//        std::queue<std::move_only_function<void(void)>>  m_queue;
//        bool                                             m_shutdown_requested;
//        size_t                                           m_busy_threads;
//
//    public:
//        ThreadPool(size_t size)
//            : m_busy_threads{ size }, m_shutdown_requested{ false }
//        {
//            m_threads.resize(size);
//
//            for (size_t i = 0; i < size; ++i)
//            {
//                m_threads[i] = std::thread(ThreadWorker(this));
//            }
//        }
//
//        ~ThreadPool()
//        {
//            shutdown();
//        }
//
//        ThreadPool(const ThreadPool&) = delete;
//        ThreadPool(ThreadPool&&) = delete;
//
//        ThreadPool& operator=(const ThreadPool&) = delete;
//        ThreadPool& operator=(ThreadPool&&) = delete;
//
//        // waits until threads finish their current task and shutdowns the pool
//        void shutdown()
//        {
//            {
//                std::lock_guard<std::mutex> guard{ m_mutex };
//                m_shutdown_requested = true;
//                m_condition_variable.notify_all();
//            }
//
//            for (size_t i{}; i != m_threads.size(); ++i)
//            {
//                if (m_threads[i].joinable())
//                {
//                    m_threads[i].join();
//                }
//            }
//        }
//
//        template <typename F, typename... Args>
//
//        auto addTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
//        {
//            auto func{ std::bind(std::forward<F>(f), std::forward<Args>(args)...) };
//
//            auto task{ std::packaged_task<decltype(f(args...)) (void) > { func } };
//
//            auto future = task.get_future();
//
//            // generalized lambda capture
//            auto wrapper = [task = std::move(task)]() mutable { task(); };
//
//            {
//                std::lock_guard<std::mutex> guard{ m_mutex };
//
//                m_queue.push(std::move(wrapper));
//
//                // wake up one waiting thread if any
//                m_condition_variable.notify_one();
//            }
//
//            // return future from packaged_task
//            return future;
//        }
//
//        size_t size()
//        {
//            std::unique_lock<std::mutex> guard{ m_mutex };
//            return m_queue.size();
//        }
//
//    private:
//        class ThreadWorker
//        {
//        public:
//            ThreadWorker(ThreadPool* pool) : thread_pool(pool)
//            {
//            }
//
//            void operator()()
//            {
//                std::thread::id tid{ std::this_thread::get_id() };
//                Logger::log(std::cout, "Started worker [", tid, "]");
//
//                std::unique_lock<std::mutex> guard{ thread_pool->m_mutex };
//
//                while (!thread_pool->m_shutdown_requested || (thread_pool->m_shutdown_requested && !thread_pool->m_queue.empty()))
//                {
//                    thread_pool->m_busy_threads--;
//
//                    thread_pool->m_condition_variable.wait(guard, [this] {
//                        return this->thread_pool->m_shutdown_requested || !this->thread_pool->m_queue.empty();
//                        }
//                    );
//
//                    thread_pool->m_busy_threads++;
//
//                    if (!this->thread_pool->m_queue.empty())
//                    {
//                        auto func{ std::move(thread_pool->m_queue.front()) };
//                        thread_pool->m_queue.pop();
//
//                        guard.unlock();
//                        func();
//                        guard.lock();
//                    }
//                }
//
//                Logger::log(std::cout, "Done [", tid, "]");
//            }
//
//        private:
//            ThreadPool* thread_pool;
//        };
//    };
//}
