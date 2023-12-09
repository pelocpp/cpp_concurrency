#include <iostream>
#include <string>

//#include <iomanip>
//#include <sstream>
//#include <thread>
//#include <numeric>
//#include <algorithm>
//#include <functional>
//#include <vector>
//#include <iterator>
//#include <type_traits>
//#include <chrono>

#include <algorithm>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>

namespace Concurrency_Parallel_For
{
/// @param[in] nb_elements : size of your for loop
/// @param[in] functor(start, end) :
/// your function processing a sub chunk of the for loop.
/// "start" is the first index to process (included) until the index "end"
/// (excluded)
/// @code
///     for(int i = start; i < end; ++i)
///         computation(i);
/// @endcode
/// @param use_threads : enable / disable threads.
///
///
    static
        void parallel_for(
            unsigned nb_elements,
            std::function<void(int start, int end)> functor,
            bool use_threads = true)
    {
        // -------
        unsigned nb_threads_hint = std::thread::hardware_concurrency();

        unsigned nb_threads = nb_threads_hint == 0 ? 8 : (nb_threads_hint);

        unsigned batch_size = nb_elements / nb_threads;

        unsigned batch_remainder = nb_elements % nb_threads;

        std::vector<std::thread> my_threads(nb_threads);

        if (use_threads)
        {
            // Multithread execution
            for (unsigned i = 0; i < nb_threads; ++i)
            {
                int start = i * batch_size;
                my_threads[i] = std::thread(functor, start, start + batch_size);
            }
        }
        else
        {
            // Single thread execution (for easy debugging)
            for (unsigned i = 0; i < nb_threads; ++i) {
                int start = i * batch_size; 
                functor(start, start + batch_size);
            }
        }

        // Deform the elements left
        int start = nb_threads * batch_size;
        functor(start, start + batch_remainder);

        // Wait for the other thread to finish their task
        //if (use_threads)
        //    std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));

        if (use_threads) {

            std::for_each(
                my_threads.begin(),
                my_threads.end(),
                [] (auto& t) {
                    t.join();
                }
            );
        }
            
        std::cout << "Done." << std::endl;
    }
}

void computation(int i)
{
    std::cout << std::string{ "... I got a " } + std::to_string(i) + std::string{ "\n" };
}

void test_parallel_for_01()
{
    using namespace Concurrency_Parallel_For;

    int nb_elements = 20;

    /// Say you want to parallelize this:
    //for (int i = 0; i < nb_elements; ++i)
    //    computation(i);

    /// Then you would do:
    parallel_for(nb_elements, [&] (int start, int end) {
        for (int i = start; i < end; ++i)
            computation(i);
        }
    );
}


void test_parallel_for_02()
{
    using namespace Concurrency_Parallel_For;

    std::mutex mtx;

    int nb_elements = 40;

    /// Say you want to parallelize this:
    //for (int i = 0; i < nb_elements; ++i)
    //    computation(i);

    /// Then you would do:
    parallel_for(nb_elements, [&] (int start, int end) {

        std::lock_guard<std::mutex> guard { mtx };
        std::cout << "  ...  " << start << " - " << end << std::endl;
        }
    );
}

void test_parallel_for()
{
    test_parallel_for_02();
}


// ===========================================================================
// End-of-File
// ===========================================================================

