// ===========================================================================
// Barriers.cpp
// ===========================================================================

#include <iostream>
#include <sstream>
#include <future>
#include <thread>
#include <barrier>
#include <array>
#include <random>
#include <deque>

#include "../Logger/Logger.h"

namespace Barriers {

    constexpr size_t ThreadCount = 4;
    constexpr size_t MaxDelay = 3000;

    size_t calcSumRange(size_t a, size_t b) {
        size_t sum{ 0 };
        for (size_t i = a; i != b; ++i) {
            sum += i;
        }
        return sum;
    }

    void example_barriers_helper(std::array<size_t, ThreadCount>& results)
    {
        std::barrier sync{ ThreadCount };

        std::vector<std::future<void>> tasks;

        std::random_device device;

        auto worker = [&](size_t i, size_t msecs, size_t first, size_t last) {

            Logger::log(std::cout, "Calculating from ", first, " up to ", last, "...");

            size_t result = calcSumRange(first, last);
            results.at(i) = result;

            // simulating still some calculation time ...
            std::this_thread::sleep_for(std::chrono::milliseconds(msecs));

            sync.arrive_and_wait();
            Logger::log(std::cout, "All calculations done :)");
        };

        size_t begin{ 1 };
        size_t increment{ 100 };
        size_t end = begin + increment;

        for (size_t i = 0; i != ThreadCount; ++i) {
            size_t msecs = static_cast<size_t>(device()) % MaxDelay;

            std::future<void> future = std::async(
                std::launch::async,
                worker,
                i,
                msecs,
                begin,
                end
            );
            tasks.push_back(std::move(future));

            begin = end;
            end += increment;
        }

        // Note:
        // the destructors for the std::async objects will wait
        // for them all to finish at the end of this function !
    }

    void example_barriers_01()
    {
        std::array<size_t, ThreadCount> results{ 0 };

        example_barriers_helper(results);

        // add partial results of worker threads
        size_t total = 0;
        for (size_t i = 0; i != ThreadCount; ++i) {
            total += results.at(i);
            Logger::log(std::cout, "Partial result: ", results.at(i));
        }

        // use gauss to verify : n * (n + 1) / 2 ==> 80200, if n == 4
        Logger::log(std::cout, "Total: ", total);
    }

    // =======================================================================

    void example_barriers_02()
    {
    }
}

void test_barriers()
{
    using namespace Barriers;
    example_barriers_01();
    example_barriers_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
