#include <iostream>
#include <iomanip>
#include <sstream>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>

namespace Concurrency_ThreadPool_01
{
    // Nachteil:  Die Threads warten mit yield auf einen nächsten Jobn
    // ==> das ist eine Busy-Loop

    // Listing 6.2

    template<typename T>
    class ThreadsafeQueue
    {
    private:
        std::queue<T> m_queue;
        std::condition_variable m_cond;
        std::mutex m_mutex;

    public:
        ThreadsafeQueue() {}

        void push(T value)
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            m_queue.push(std::move(value));
            m_cond.notify_one();
        }

        void wait_and_pop(T& value)
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_queue.wait(lock, [this] {return !m_queue.empty(); });
            value = std::move(m_queue.front());
            m_queue.pop();
        }

        //std::shared_ptr<T> wait_and_pop()
        //{
        //    std::unique_lock<std::mutex> lock{ m_mutex };
        //    m_cond.wait(lock, [this] {return !m_queue.empty(); });
        //    std::shared_ptr<T> sp{
        //        std::make_shared<T>(std::move(m_queue.front())) 
        //    };
        //    m_queue.pop();
        //    return sp;
        //}

        bool try_pop(T& value)
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            if (m_queue.empty()) {
                return false;
            }

         //   std::cout << "try_pop ... \n";

            value = std::move(m_queue.front());
            m_queue.pop();
            return true;
        }

        //std::shared_ptr<T> try_pop()
        //{
        //    std::lock_guard<std::mutex> lock{ m_mutex };
        //    if (m_queue.empty())
        //        return std::shared_ptr<T>();
        //    std::shared_ptr<T> res(
        //        std::make_shared<T>(std::move(m_queue.front())));
        //    m_queue.pop();
        //    return res;
        //}

        //bool empty() const
        //{
        //    std::lock_guard<std::mutex> lock{ m_mutex };
        //    return m_queue.empty();
        //}
    };

    class join_threads
    {
        std::vector<std::thread>& m_threads;

    public:
        explicit join_threads(std::vector<std::thread>& threads) : m_threads{ threads } {}
        
        ~join_threads()
        {
            //std::cout << "> ~join_threads ...\n";

            for (size_t i{}; i < m_threads.size(); ++i)
            {
                if (m_threads[i].joinable())
                    m_threads[i].join();
            }

           // std::cout << "< ~join_threads ...\n";
        }
    };

    class ThreadPool
    {
    private:
        std::atomic_bool m_done;
        ThreadsafeQueue<std::function<void()>> m_workQueue;
        std::vector<std::thread> m_threads;
        join_threads m_joiner;

    public:
        ThreadPool() : m_done{ false }, m_joiner{ m_threads }
        {
            const unsigned int count{ std::thread::hardware_concurrency() };

            try
            {
                for (unsigned i{}; i < count; ++i)
                {
                    std::thread tmp{ &ThreadPool::worker_thread, this };
                    m_threads.push_back(std::move(tmp));
                }
            }
            catch (...)
            {
                m_done = true;
                throw;
            }

            std::cout << "Created pool with " << count << " threads.\n";
        }

        ~ThreadPool()
        {
            m_done = true;
        }

        template<typename FunctionType>
        void submit(FunctionType f)
        {
           // std::cout << "pushed ...\n";
            m_workQueue.push(std::function<void()>(f)); 
        }

    private:
        void worker_thread()
        {
            while (!m_done)
            {
                std::function<void()> task{};

                if (m_workQueue.try_pop(task))
                {
                    task();
                }
                else
                {
                    // std::cout << "yield ...\n";
                    std::this_thread::yield();
                }
            }
        }
    };
}

void test_concurrency_thread_pool_01 ()
{
    using namespace Concurrency_ThreadPool_01;

    auto callable = []() { 
        std::stringstream ss;
        ss << "Thread " << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << std::this_thread::get_id() << '\n';
        std::cout << ss.str(); 
    };

    ThreadPool pool;

    for (int i = 0; i < 2; ++i) {
        pool.submit(callable);
    }

    char ch;
    std::cin >> ch;
}


