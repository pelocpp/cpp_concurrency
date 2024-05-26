// ===========================================================================
// ThreadPool_03_ArthurDwyer.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus Buch von Arthur Dwyer
 * 
 */

#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <future>
#include <queue>
#include <type_traits>
#include <cassert>

#include "FunctionWrapper.h"

namespace ThreadPool_ArthurDwyer {

    class ThreadPool
    {
        //using UniqueFunction = std::packaged_task<void()>;
        using ThreadPoolFunction = FunctionWrapper;

        // KL�REN: Warum nicht  ThreadsafeQueue<ThreadPoolFunction> m_workQueue;

        //struct
        //{
        std::mutex                        m_mutex;
        std::queue<ThreadPoolFunction>    m_workQueue;
        bool                              m_aborting;
        //} 
        //m_state;

        std::vector<std::thread>          m_threads;
        std::condition_variable           m_condition;

    public:
        ThreadPool();
        ThreadPool(size_t size);
        ~ThreadPool();

        void enqueue_task(ThreadPoolFunction task)
        {
            //if (std::lock_guard lk(m_state.m_mutex); true) {
            //    m_state.m_workQueue.push(std::move(task));
            //}

            {
                std::lock_guard guard(m_mutex);
                m_workQueue.push(std::move(task));
            }

            m_condition.notify_one();
        }

    private:
        void worker_loop() 
        {
            std::thread::id tid{ std::this_thread::get_id() };
            Logger::log(std::cout, "Started worker [", tid, "]");

            while (true)
            {
                std::unique_lock guard{ m_mutex };

                while (m_workQueue.empty() && !m_aborting) {
                    m_condition.wait(guard);
                }

                if (m_aborting) {
                    break;
                }

                // Pop the next task, while still under the lock.
                // was ist wenn empty ????????????????
                assert(!m_workQueue.empty());

                ThreadPoolFunction task{ std::move(m_workQueue.front()) };

                m_workQueue.pop();

                guard.unlock();

                // Actually run the task. This might take a while.
                task();
                // When we're done with this task, go get another.
            }

            Logger::log(std::cout, "Done [", tid, "]");
        }

    public:

        //template<typename FunctionType>
        //using result_type = typename std::invoke_result<FunctionType>::type;

        // Die Submit Methode stammt aus dem 2. Beispiel

        template<typename FunctionType>
        std::future<typename std::invoke_result<FunctionType>::type>
            submit(FunctionType f)
        {
            // typedef typename std::result_of<FunctionType>::type result_type;
            typedef typename std::invoke_result<FunctionType>::type result_type;

            std::packaged_task<result_type()> task(std::move(f));

            std::future<result_type> res(task.get_future());

            // DIESE ZEILE GEHT NICHT !!!!!!!!!!!!!!!!!!!!
            m_workQueue.push(std::move(task));  // Hier wird implicit ein FunctionWrapper angelegt !!!

            return res;
        }

        template<class F>
        auto async(F&& func)
        {
            using ResultType = std::invoke_result_t<std::decay_t<F>>;

            std::packaged_task<ResultType()> pt(std::forward<F>(func));

            std::future<ResultType> future = pt.get_future();

            ThreadPoolFunction task(
                [pt1 = std::move(pt)]() mutable { pt1(); }
            );

            enqueue_task(std::move(task));

            // Give the user a future for retrieving the result.
            return future;
        }
    };
}
