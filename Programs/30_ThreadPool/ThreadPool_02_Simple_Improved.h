// ===========================================================================
// ThreadPool_02_Simple_Improved.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus dem Buch "Concurrency in Action - 2nd Edition", Kapitel 9.2
 * 
 * A thread pool with waitable tasks
 *
 */

#include "../25_Threadsafe_Queue/ThreadsafeQueue.h"

#include "FunctionWrapper.h"
#include "JoinThreads.h"

#include <iostream>
#include <functional>
#include <thread>
#include <future>
#include <vector>
#include <deque>

namespace ThreadPool_Simple_Improved
{
    using namespace Concurrency_ThreadsafeQueue;

    // using ThreadPoolFunction = std::packaged_task<void()>;
    using ThreadPoolFunction = FunctionWrapper;

    class ThreadPool
    {
    private:
        std::atomic_bool                     m_done;
        ThreadsafeQueue<ThreadPoolFunction>  m_workQueue;
        std::vector<std::thread>             m_threads;
        JoinThreads                          m_joiner;

    public:
        ThreadPool();
        ~ThreadPool();

        template<typename TFunctionType>
        std::future<typename std::invoke_result<TFunctionType>::type>
        submit(TFunctionType&& f)
        {
            typedef typename std::invoke_result<TFunctionType>::type result_type;

            std::packaged_task<result_type()> task{ std::move(f) };

            std::future<result_type> future{ task.get_future() };

            // Diese Zeile übersetzt nicht: // Type Erasure ... stimmt das ...
            m_workQueue.push(std::move(task));

            return future;
        }

    private:
        void worker_thread();
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
