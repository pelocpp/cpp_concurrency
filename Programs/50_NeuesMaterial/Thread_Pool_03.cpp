#include <iostream>
#include <cassert>
#include <chrono>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <utility>

// AUS DEM BUCH VON ARTHUR DWYER

namespace Concurrency_ThreadPool_03
{

    // from Thread_Pool_02
    class function_wrapper
    {
        struct impl_base {
            virtual void call() = 0;
            virtual ~impl_base() {}
        };

        std::unique_ptr<impl_base> impl;

        template<typename F>
        struct impl_type : impl_base
        {
            F f;
            impl_type(F&& f_) : f(std::move(f_)) {}
            void call() { f(); }
        };

    public:
        template<typename F>
        function_wrapper(F&& f) :
            impl(new impl_type<F>(std::move(f)))
        {}

        function_wrapper() = default;

        void operator()() { impl->call(); }

        void call() { impl->call(); }

        function_wrapper(function_wrapper&& other) noexcept :
            impl(std::move(other.impl))
        {}

        function_wrapper& operator=(function_wrapper&& other) noexcept
        {
            impl = std::move(other.impl);
            return *this;
        }

        function_wrapper(const function_wrapper&) = delete;
        function_wrapper(function_wrapper&) = delete;
        function_wrapper& operator=(const function_wrapper&) = delete;
    };




    class ThreadPool
    {
        //using UniqueFunction = std::packaged_task<void()>;
        using UniqueFunction = function_wrapper;

        struct {
            std::mutex mtx;
            std::queue<UniqueFunction> work_queue;
            bool aborting = false;
        } m_state;

        std::vector<std::thread> m_workers;
        std::condition_variable m_cv;

    public:
        ThreadPool(int size) {
            for (int i = 0; i < size; ++i) {
                m_workers.emplace_back([this]() { worker_loop(); });
            }
        }

        ~ThreadPool() {
            if (std::lock_guard lk(m_state.mtx); true) {
                m_state.aborting = true;
            }

            m_cv.notify_all();
            
            for (std::thread& t : m_workers) {
                t.join();
            }
        }

        void enqueue_task(UniqueFunction task) { 
            if (std::lock_guard lk(m_state.mtx); true) {
                m_state.work_queue.push(std::move(task));
            }
            m_cv.notify_one();
        }

    private:
        void worker_loop() {
            while (true) {
                std::unique_lock lk(m_state.mtx);

                while (m_state.work_queue.empty() && !m_state.aborting) {
                    m_cv.wait(lk);
                }
                
                if (m_state.aborting) {
                    break;
                }

                // Pop the next task, while still under the lock.
                assert(!m_state.work_queue.empty());

                UniqueFunction task = std::move(m_state.work_queue.front());

                m_state.work_queue.pop();

                lk.unlock();

                // Actually run the task. This might take a while.
                task();
                // When we're done with this task, go get another.
            }
        }

    // using UniqueFunction = std::packaged_task<void()>;

    public:
        template<class F>
        auto async(F&& func)
        {
            using ResultType = std::invoke_result_t<std::decay_t<F>>;

            std::packaged_task<ResultType()> pt(std::forward<F>(func));

            std::future<ResultType> future = pt.get_future();

            UniqueFunction task(
                [pt1 = std::move(pt)]() mutable { pt1(); }
            );

            enqueue_task(std::move(task));
            
            // Give the user a future for retrieving the result.
            return future;
        }
    };
}

void test_concurrency_thread_pool_03_test_01()
{
    using namespace Concurrency_ThreadPool_03;

    std::atomic<int> sum(0);

    ThreadPool tp(4);

    std::vector<std::future<int>> futures;

    for (int i = 0; i < 60000; ++i) {
        
        auto f = tp.async([i, &sum] () {
            sum += i;
            return i;
            }
        );

        futures.push_back(std::move(f));
    }
    assert(futures[42].get() == 42);
    assert(903 <= sum && sum <= 1799970000);
}

void test_concurrency_thread_pool_03()
{
    std::cout << "Begin: " << std::endl;
    test_concurrency_thread_pool_03_test_01();
    std::cout << "Done." << std::endl;
}