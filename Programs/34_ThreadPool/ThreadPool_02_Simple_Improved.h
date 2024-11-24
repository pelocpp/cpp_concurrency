// ===========================================================================
// ThreadPool_02_Simple_Improved.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus dem Buch "Concurrency in Action - 2nd Edition", Kapitel 9.2
 * 
 * Nachteil: Busy Polling
 * 
 * Vorteil:  Ergebnisse der Threads können mit std::future-Objekten abgeholt werden 
 *
 */

#include "../31_Threadsafe_Queue/ThreadsafeQueue.h"

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

    //using ThreadPoolFunction = std::packaged_task<void()>;
    //using ThreadPoolFunction = FunctionWrapper;
    using ThreadPoolFunction = PolymorphicObjectWrapper;

    template<typename TFunc>
    using TReturn = typename std::invoke_result<TFunc>::type;

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

        template<typename TFunc>
        std::future<TReturn<TFunc>>
        submit(TFunc&& func)
        {
            std::packaged_task<TReturn<TFunc>()> task{ std::move(func) };

            std::future<TReturn<TFunc>> future{ task.get_future() };

            // Diese Zeile übersetzt nicht: Beachte Type Erasure !
            m_workQueue.push(std::move(task));

            // ausführlich
            //ThreadPoolFunction tmp{ std::move(task) };
            //m_workQueue.push(std::move(tmp));

            return future;
        }

    private:
        void worker_thread();
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
