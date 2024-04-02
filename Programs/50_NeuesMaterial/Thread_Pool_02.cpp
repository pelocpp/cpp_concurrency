#include <iostream>
#include <iomanip>
#include <deque>
#include <future>
#include <thread>
#include <memory>
#include <functional>
#include <queue>
#include <type_traits>
#include <vector>
#include <numeric>
#include <sstream>

namespace Concurrency_ThreadPool_02
{
    // ===============================================================================
// TBD: Das ist DIREKT kopiert aus der Datei Parallel_Accumulate.cpp
    //template<typename Iterator, typename T>
    //class AccumulateBlockExEx
    //{
    //private:
    //    Iterator m_first;
    //    Iterator m_last;
    //    T& m_result;

    //public:
    //    AccumulateBlockEx(Iterator first, Iterator last, T& result)
    //        : m_first{ first }, m_last{ last }, m_result{ result }
    //    {}

    //    void operator()() {

    //        std::stringstream ss;
    //        ss << "  launching " << std::this_thread::get_id() << ":\n";
    //        std::cout << ss.str();

    //        m_result = std::accumulate(m_first, m_last, m_result);
    //    }
    //};



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

    // Mˆglicherweise std::decay verwenden ...........
   // using ResultType = std::invoke_result_t<std::decay_t<F>>;

    template<typename FunctionType>
    using result_type = typename std::invoke_result<FunctionType>::type;

    class ThreadPool
    {
    private:
        std::atomic_bool m_done;

        ThreadsafeQueue<function_wrapper> m_workQueue;
        // ThreadsafeQueue<std::packaged_task<void()>> m_workQueue;

        std::vector<std::thread> m_threads;
        join_threads m_joiner;

    public:
        // Meine Anpassung:
        // Separates Template 'result_type'
        // Letzten Endes geht es um ein paar Klammern, was im Buch falsch ist !!!!
        template<typename FunctionType>
        std::future<result_type<FunctionType>>
        submit(FunctionType f)
        {
            std::packaged_task<result_type<FunctionType>()> task(std::move(f));

            std::future<result_type<FunctionType>> fut(task.get_future());

            m_workQueue.push(std::move(task));
            return fut;
        }

        // Vorlage vom Buch:
        // Geht, aber 2 Zeilen sind falsch:
        // Es muss zweimal std::invoke_result<FunctionType> an Stelle von
        // std::invoke_result<FunctionType()>
        // heiﬂen !!!!!!!!!!!!!!!!!!!!!!!!!
        template<typename FunctionType>
        std::future<typename std::invoke_result<FunctionType>::type>
        submitXX(FunctionType f)
        {
            typedef typename std::invoke_result<FunctionType>::type result_type;
            std::packaged_task<result_type()> task(std::move(f));
            std::future<result_type> res(task.get_future());
            m_workQueue.push(std::move(task));
            return res;
        }

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

    private:

        //  std::function  ==> std::unique_ptr ==> Type Erasure

        void worker_thread()
        {
            while (!m_done)
            {
                //std::function<void()> task{};
                function_wrapper task{};

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

auto callable = []() -> int {

    std::cout << "callable\n";
    return 123;
};

int callableFunc() {

    std::cout << "callableFunc\n";
    return 123;
};


void test_concurrency_thread_pool_02_a()
{
    using namespace Concurrency_ThreadPool_02;

    ThreadPool pool;

    std::deque<std::future<int>> futures;

    for (int i = 0; i < 10; ++i) {
        std::future<int> f = pool.submit(callableFunc);
        // or
        // std::future<int> f = pool.submitXX(callableFunc);
        futures.push_back(std::move(f));
    }

    // get the results
    for (size_t i = 0; i != 10; i++) {
        std::future<int> future{ std::move(futures.front()) };
        futures.pop_front();
        int n{ future.get() };
        std::cout << "n =  " << n << std::endl;
    }

    //char ch;
    //std::cin >> ch;
}


//
//template<typename Iterator, typename T>
//auto parallelAccumulate_with_thread_pool(Iterator first, Iterator last, T init)
//{
//    using namespace Concurrency_ThreadPool_02;
//
//    // to be fixed
//    int num_threads = 5;
//
//    unsigned long const length = std::distance(first, last);
//
//    if (!length)
//        return init;
//
//    unsigned long const block_size = 25;
//    unsigned long const num_blocks = (length + block_size - 1) / block_size;
//
//    std::vector<std::future<T> > futures(num_blocks - 1);
//
//    ThreadPool pool;
//
//    Iterator block_start = first;
//
//    for (unsigned long i = 0; i < (num_threads - 1); ++i)
//    {
//        Iterator block_end = block_start;
//
//        std::advance(block_end, block_size);
//
//        //futures[i] = pool.submit(AccumulateBlockExEx<Iterator, T>(block_start, block_end));
//
//        futures[i] = pool.submit([=] {
//            AccumulateBlockExEx<Iterator, T> tmp (block_start, block_end);
//            }
//        );
//
//        block_start = block_end;
//    }
//
//    T last_result = AccumulateBlockExEx(block_start, last);
//    T result = init;
//    for (unsigned long i = 0; i < (num_blocks - 1); ++i)
//    {
//        result += futures[i].get();
//    }
//    result += last_result;
//    return result;
//}
//
//void test_concurrency_thread_pool_02_b()
//{
//    using namespace Concurrency_ThreadPool_02;
//
//    const size_t Length{ 10'001 };
//
//
//    std::vector<size_t> numbers(Length);
//
//    std::iota(
//        std::begin(numbers),
//        std::end(numbers),
//        0
//    );
//
//    size_t sum{
//        parallelAccumulate_with_thread_pool<std::vector<size_t>::iterator, size_t>(
//            std::begin(numbers),
//            std::end(numbers),
//            0
//        )
//    };
//
//    std::cout << "Sum = " << sum << std::endl;
//    std::cout << "Done." << std::endl;
//}

// ===============================================================================

void test_concurrency_thread_pool_02()
{
    test_concurrency_thread_pool_02_a();
    // test_concurrency_thread_pool_02_b();
}

// ===========================================================================
// End-of-File
// ===========================================================================

template<typename FunctionType>
void testFunction(FunctionType func)
{
#if defined (_MSC_VER)
    std::cout << __FUNCSIG__ << std::endl;
#endif

#if defined (__GNUC__)
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

    using result_type = typename std::invoke_result<FunctionType>::type;

    result_type xxx{};
}

template<typename FunctionType>
void testFunctionSubmit(FunctionType func)
{
    using result_type = typename std::invoke_result<FunctionType>::type;

    // invoking an instance of type FunctionType(such as f) with no arguments
    std::packaged_task<result_type()> task(std::move(func));
    // or
    // std::packaged_task task(std::move(func));  // using CTAD

    std::future future(task.get_future());

    task();

    int result = future.get();

    std::cout << "Ergebnis = " << result << "\n";

    result_type xxx{ result };
}

// --------------------
// geht ...

template<typename FunctionType>
using result_type = typename std::invoke_result<FunctionType>::type;

template<typename FunctionType>
std::future<result_type<FunctionType>>
testFunctionSubmitEx(FunctionType func)
{
    // std::packaged_task<result_type()> task(std::move(func));
    std::packaged_task<result_type<FunctionType>()> task(std::move(func));

    std::future<result_type<FunctionType>> future(task.get_future());

    task();

    result_type<FunctionType> xxx{};

    return future;
}

void test_concurrency_thread_pool_02_test()
{
    testFunction(callable);

    testFunctionSubmit(callableFunc);

    std::future<int> future = testFunctionSubmitEx(callable);

    int result = future.get();

    std::cout << "2. Ergebnis = " << result << "\n";
}
