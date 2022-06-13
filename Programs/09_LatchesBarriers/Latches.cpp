// ===========================================================================
// Latches.cpp
// ===========================================================================

#include <iostream>
#include <sstream>
#include <future>
#include <thread>
#include <latch>
#include <array>
#include <random>
#include <deque>

#include "../Logger/Logger.h"

namespace Latches_01 {

    constexpr size_t ThreadCount{ 4 };
    constexpr size_t MaxDelay{ 5000 };

    int calcSumRange(int a, int b) {
        int sum{};
        for (int i{ a }; i != b; ++i) {
            sum += i;
        }
        return sum;
    }

    void example_latches_01()
    {
        std::latch done{ ThreadCount };

        std::array<int, ThreadCount> results{};

        std::vector<std::future<void>> tasks(ThreadCount);

        std::random_device device;

        auto worker = [&](size_t index, size_t msecs, int first, int last) {

            Logger::log(std::cout, "Calculating from ", first, " up to ", last, "...");

            int result{ calcSumRange(first, last) };

            results.at(index) = result;

            // simulating still some calculation time ...
            std::this_thread::sleep_for(std::chrono::milliseconds(msecs));

            Logger::log(std::cout, "Done");

            done.count_down();
        };

        int begin{ 1 };
        int increment{ 100 };
        int end{ begin + increment };

        for (size_t i{}; i != ThreadCount; ++i) {

            size_t msecs{ static_cast<size_t>(device()) % MaxDelay };

            std::future<void> future = std::async(
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

        // block until work is done
        done.wait();
        Logger::log(std::cout, "All calculations done :)");

        /*
        * Kontrollfrage:
        * Wie könnte man an dieser Stelle auf das Ende aller Worker-Threads warten
        * ohne Verwendung eines std::latch-Objekts
        * [tasks.clear();]
        */

        // add partial results of worker threads
        int total{};
        for (size_t i{}; i != ThreadCount; ++i) {
            total += results.at(i);
            Logger::log(std::cout, "Partial result: ", results.at(i));
        }

        // use gauss to verify : n * (n + 1) / 2 ==> 80200, if n == 4
        Logger::log(std::cout, "Total: ", total);
    }
}

// =======================================================================

namespace Latches_02 {

    constexpr size_t MaxDelay{ 3000 };

    std::latch workDone{ 5 };
    std::latch doExit{ 1 };

    std::random_device device;

    auto slave = [](std::string name) {
        Logger::log(std::cout, name, ": Started working.");

        // simulating still some calculation time ...
        size_t msecs{ 2000 + static_cast<size_t>(device()) % MaxDelay };
        std::this_thread::sleep_for(std::chrono::milliseconds(msecs));

        // notify the master when work is done
        Logger::log(std::cout, name, ": Work done!");
        workDone.count_down();

        // waiting before exiting ...
        doExit.wait();
        Logger::log(std::cout, name, ": Exit.");
    };

    void example_latches_02()
    {
        std::deque<std::future<void>> tasks;

        Logger::log(std::cout, "Working starts:");

        tasks.push_back(std::async(std::launch::async, slave, "Worker (1)"));
        tasks.push_back(std::async(std::launch::async, slave, "Worker (2)"));
        tasks.push_back(std::async(std::launch::async, slave, "Worker (3)"));
        tasks.push_back(std::async(std::launch::async, slave, "Worker (4)"));
        tasks.push_back(std::async(std::launch::async, slave, "Worker (5)"));

        workDone.wait();

        Logger::log(std::cout, "Working done.");

        doExit.count_down();
    }
}

void test_latches()
{
    using namespace Latches_01;
    using namespace Latches_02;
    example_latches_01();
    example_latches_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
