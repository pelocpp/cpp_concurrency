// ===========================================================================
// ActiveObject_02.cpp // Active Object Pattern
// ===========================================================================

// #include <iostream>

#include "../Logger/Logger.h"

#include <algorithm>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <tuple>
#include <vector>

// ===========================================================================
// 
// This Example demonstrates the 'Active Object Pattern'
// in a "Real-World" example.
// Result values are returned from the 'Active Object' to the client.
// But: The Client request are all invoked from a single thread,
// see the corresponding output.

// ===========================================================================

namespace ActivatorObject02
{
    class SumRange {

    private:
        size_t m_a;
        size_t m_b;

    public:
        SumRange(size_t a, size_t b) : m_a{ a }, m_b{ b } {}

        std::tuple<size_t, size_t, size_t> operator() () {

            Logger::log(std::cout, "   calculating range [ ", m_a, ",", m_b, "]");

            size_t sum{};
            for (size_t i{ m_a }; i != m_b; ++i) {
                sum += i;
            }
            return std::make_tuple(m_a, m_b, sum);
        }
    };

    class ActiveObject {

    private:
        std::deque<std::packaged_task<std::tuple<size_t, size_t, size_t>()>> m_activationList;

        std::mutex m_mutex;

    public:

        std::future<std::tuple<size_t, size_t, size_t>> enqueueTask(size_t a, size_t b) {

            SumRange range{ a, b };

            std::packaged_task<std::tuple<size_t, size_t, size_t>()> task{ range };

            std::future<std::tuple<size_t, size_t, size_t>> future{ task.get_future() };

            Logger::log(std::cout, "   queueing task [", a, ",", b, "]");

            {
                std::lock_guard<std::mutex> guard{ m_mutex };

                m_activationList.push_back(std::move(task));
            }

            return future;
        }

        void run() {

            std::jthread jt([this] () {

                while (!runNextTask()) {
                    ;
                }
            });
        }

    private:

        bool runNextTask() {

            std::unique_lock<std::mutex> guard{ m_mutex };

            bool isEmpty{ m_activationList.empty() };

            if (!isEmpty) {

                auto task{ std::move(m_activationList.front()) };

                m_activationList.pop_front();

                guard.unlock();  // don't run task within locked context

                task();
            }

            return isEmpty;
        }
    };

    static std::vector<std::future<std::tuple<size_t, size_t, size_t>>>
    enqueueTasksSynchronously(ActiveObject& activeObject, size_t start, size_t length, size_t count) {

        std::vector<std::future<std::tuple<size_t, size_t, size_t>>> futures{};

        for (size_t i{}; i != count; ++i) {

            std::future<std::tuple<size_t, size_t, size_t>> future {
                activeObject.enqueueTask(start, start + length) 
            };

            start += length;

            futures.push_back(std::move(future));
        }

        return futures;
    }
}

void test_active_object_02()
{
    using namespace ActivatorObject02;

    Logger::log(std::cout, "Active Object Demo (Synchron)");

    ActiveObject activeObject{};

    // enqueue work concurrently
    Logger::log(std::cout, "Enqueue tasks synchronously ...");

    // range from 1 to 3000
    std::vector<std::future<std::tuple<size_t, size_t, size_t>>> futures {
        enqueueTasksSynchronously(activeObject, 1, 100, 30)  
    };

    // activate the active object
    Logger::log(std::cout, "Run ...");
    activeObject.run();

    // get the results from the futures
    std::vector<std::tuple<size_t, size_t, size_t>> results;
    results.reserve(futures.size());

    for (auto& future : futures) { 
        results.push_back(future.get());
    }

    // calculate final sum result
    size_t totalSum{};

    std::for_each(
        results.begin(),
        results.end(),
        [&](const auto& tuple){
        
            auto partialSum = std::get<2>(tuple);
            totalSum += partialSum;
        }
    );

    Logger::log(std::cout, "TotalSum: ", totalSum);   // expecting 4'501'500
}

// ===========================================================================
// End-of-File
// ===========================================================================
