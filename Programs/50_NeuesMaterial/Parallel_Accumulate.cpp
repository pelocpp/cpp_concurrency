#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <iterator>
#include <type_traits>
#include <chrono>

namespace concurrencyParallelAccumulate01
{
    //template<typename Iterator, typename T>
    //class AccumulateBlock
    //{
    //public:
    //    void operator() (Iterator first, Iterator last, T& result)
    //    {
    //        result = std::accumulate(first, last, result);
    //    }
    //};

    //// Original
    //template<typename Iterator, typename T>
    //T parallelAccumulate(Iterator first, Iterator last, T init)
    //{
    //    size_t const length = std::distance(first, last);

    //    if (!length)
    //        return init;

    //    size_t const min_per_thread = 25;

    //    size_t const max_threads =
    //        (length + min_per_thread - 1) / min_per_thread;
    //    // size_t const max_threads = length / min_per_thread;

    //    size_t const hardware_threads = std::thread::hardware_concurrency();

    //    size_t const num_threads =
    //        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    //    size_t const block_size = length / num_threads;

    //    std::vector<T> results(num_threads);

    //    std::vector<std::thread>  threads(num_threads - 1);

    //    Iterator block_start = first;
    //    for (size_t i = 0; i < (num_threads - 1); ++i)
    //    {
    //        Iterator block_end = block_start;
    //        std::advance(block_end, block_size);
    //        threads[i] = std::thread(
    //            AccumulateBlock<Iterator, T>(),
    //            block_start, block_end, std::ref(results[i]));
    //        block_start = block_end;
    //    }
    //    AccumulateBlock<Iterator, T>()(block_start, last, results[num_threads - 1]);

    //    std::for_each(threads.begin(), threads.end(),
    //        std::mem_fn(&std::thread::join));

    //    return std::accumulate(results.begin(), results.end(), init);
    //}

    // ===========================================================================

    template<typename Iterator, typename T>
    class AccumulateBlockEx
    {
    private:
        Iterator m_first;
        Iterator m_last;
        T& m_result;

    public:
        AccumulateBlockEx(Iterator first, Iterator last, T& result)
            : m_first{ first }, m_last{ last }, m_result{ result } 
        {}

        void operator()() {

            std::stringstream ss;
            ss << "  launching " << std::this_thread::get_id() << ":\n";
            std::cout << ss.str();;

            m_result = std::accumulate(m_first, m_last, m_result);
        }
    };

    template<typename Iterator, typename T>
    T parallelAccumulateEx(Iterator first, Iterator last, T init)
    {
        using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;

        const DifferenceType Length{ std::distance(first, last) };
        if (Length == 0) {
            return init;
        }

        const size_t MinimumPerThread{ 25 };

        const size_t MaxThreads {
            (Length + MinimumPerThread - 1) / MinimumPerThread
        };
        
        const size_t NumHardwareThreads{
            std::thread::hardware_concurrency() 
        };

        //const size_t NumThreads {
        //    std::min(NumHardwareThreads != 0 ? NumHardwareThreads : 2, MaxThreads) 
        //};
        //std::cout << "Using " << NumThreads << " Threads." << std::endl;

        const size_t NumThreads{ 1 };
        std::cout << "Using " << NumThreads << " Threads." << std::endl;

        const size_t BlockSize{ Length / NumThreads };
        std::cout << "BlockSize = " << BlockSize << "." << std::endl;

        std::vector<T> results(NumThreads);

        std::vector<std::thread> threads(NumThreads - 1);

        Iterator blockStart{ first };

        for (size_t i{}; i != NumThreads - 1; ++i) {

            Iterator blockEnd{ blockStart };

            std::advance(blockEnd, BlockSize);

            AccumulateBlockEx<Iterator, T> block {
                blockStart,
                blockEnd, 
                std::ref(results[i])
            };

            std::thread t{ block };

            threads[i] = std::move(t);

            blockStart = blockEnd;
        }

        AccumulateBlockEx<Iterator, T> lastBlock { 
            blockStart, 
            last,
            std::ref(results[NumThreads - 1])
        };

        lastBlock();

        std::for_each(
            std::begin(threads),
            std::end(threads),
            [](auto& thread) { 
                thread.join();
            }
        );

        T total{
            std::accumulate(
                std::begin(results),
                std::end(results),
                init
            ) 
        };

        return total;
    }

}

void testMaxSize()
{
    const int LENGTH = 401;

    size_t const min_per_thread = 25;

    for (int length = 0; length < LENGTH; ++length) {

        size_t const max_threads = (length  + min_per_thread - 1 ) / min_per_thread;

        std::cout << "Length: " << length << " - max_threads: " << max_threads << std::endl;
    }
}

void printResultsEx(
    std::string tag,
    std::chrono::high_resolution_clock::time_point startTime,
    std::chrono::high_resolution_clock::time_point endTime)
{
    std::cout
        << tag
        << std::fixed
        << std::setprecision(9)
        << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count()
        << " msecs."
        << std::endl;
}

void test_concurrency_parallel_accumulate()
{
    using namespace concurrencyParallelAccumulate01;

    std::cout << "ParallelAccumulate:" << std::endl;


    //const size_t Length{ 101 };
   // const size_t Length{ 10'001 };
   //const size_t Length{ 1'000'001 };
    const size_t Length{ 10'000'001 };

    std::vector<size_t> numbers (Length);

    std::iota(
        std::begin(numbers),
        std::end(numbers),
        0
    );

    const auto startTime{ std::chrono::high_resolution_clock::now() };


    size_t sum{ 
        parallelAccumulateEx<std::vector<size_t>::iterator, size_t>(
            std::begin(numbers),
            std::end(numbers),
            0
        ) 
    };

    const auto endTime{ std::chrono::high_resolution_clock::now() };

    printResultsEx("Parallel: ", startTime, endTime);

    std::cout << "Sum = " << sum << std::endl;
    std::cout << "Done." << std::endl;
}

