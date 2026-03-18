// ===========================================================================
// ThreadPool.h // Thread Pool
// ===========================================================================

#pragma once

#include "../Logger/Logger.h"

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
    using ThreadPoolFunction = std::move_only_function<void()>;

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

    template <typename TFunc, typename... TArgs>
    auto addTask(TFunc&& func, TArgs&&... args)
        -> std::future<typename std::invoke_result<TFunc, TArgs...>::type>
    {
        Logger::log(std::cout, "addTask ...");

        using ReturnType = std::invoke_result<TFunc, TArgs...>::type;

        auto task = std::packaged_task<ReturnType()>{
            [func = std::forward<TFunc>(func),
            ... args = std::forward<TArgs>(args)]() mutable -> ReturnType
            {
                return std::invoke(std::move(func), std::move(args) ...);
            }
        };

        auto future{ task.get_future() };

        // generalized lambda capture
        auto wrapper{ [task = std::move(task)] () mutable -> void { task(); } };

        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_queue.push(std::move(wrapper));
        }

        // wake up one waiting thread if any
        m_condition.notify_one();

        // return future from packaged_task
        return future;
    }

    template <typename TFunc, typename... TArgs>
    auto addTaskEx(TFunc&& func, TArgs&&... args)
        -> std::future<typename std::invoke_result<TFunc, TArgs...>::type>
    {
        Logger::log(std::cout, "addTaskEx ...");

        using ReturnType = std::invoke_result<TFunc, TArgs...>::type;

        std::shared_ptr<std::promise<ReturnType>> promise{
            std::make_shared<std::promise<ReturnType>>() 
        };

        std::future<ReturnType> future{ promise->get_future() };

        m_queue.push(
            [promise,
            func = std::forward<TFunc>(func),
            ... args = std::forward<TArgs>(args)] () mutable
            {
                try
                {
                    if constexpr (std::is_void_v<ReturnType>)
                    {
                        std::invoke(std::move(func), std::move(args)...);
                        promise->set_value();
                    }
                    else
                    {
                        auto result{ std::invoke(std::move(func), std::move(args)...) };
                        promise->set_value(std::move(result));

                        //promise->set_value(
                        //    std::invoke(std::move(func), std::move(args)...)
                        //);
                    }
                }
                catch (...)
                {
                    promise->set_exception(std::current_exception());
                }
            }
        );

        return future;


    }

    // getter
    bool empty() const;
    size_t size() const;

private:
    void worker();
};


// ===========================================================================
// End-of-File
// ===========================================================================
