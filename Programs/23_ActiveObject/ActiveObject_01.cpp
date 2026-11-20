// ===========================================================================
// ActiveObject_01.cpp // Active Object Pattern
// ===========================================================================

#include "../Logger/Logger.h"

#include <chrono>
#include <future>
#include <mutex>
#include <thread>
#include <tuple>

// ===========================================================================
// 
// This Example demonstrates the 'Active Object Pattern'
// pattern as compact as possible.
// The 'activation list' queue consist of a single element.
// The scheduler runs a single task, but:
// A result value is returned from the 'Active Object' to the client.

// ===========================================================================

namespace ActivatorObject01
{
    class AddOperation
    {
    public:
        AddOperation(int a, int b) : m_a{ a }, m_b{ b } {}

        auto operator() () {

            Logger::log(std::cout, "adding ", m_a, " and ", m_b, " ... ");

            std::this_thread::sleep_for(std::chrono::seconds{ 1 });

            auto result{ std::make_tuple(m_a, m_b, m_a + m_b) };

            return result;
        }

    private:
        int m_a;
        int m_b;
    };

    class ActiveObject 
    {
        using Result = std::tuple<int, int, int>;

    private:
        std::packaged_task<Result()> m_activationElement;
        mutable std::mutex           m_mutex;
        std::jthread                 m_thread;
        // declared last: destroyed (joined) first

    public:

        std::future<Result> enqueueTask(int a, int b) {

            AddOperation operation{ a, b };

            std::packaged_task<Result()> task{ operation };

            auto future = task.get_future();

            {
                std::lock_guard<std::mutex> guard{ m_mutex };

                m_activationElement = std::move(task);
            }

            return future;
        }

        void run() {

            m_thread = std::jthread { [this] () { runSingleTask(); } };
        }

    private:

        void runSingleTask() {

            std::packaged_task<Result()> task;
            {
                std::lock_guard<std::mutex> guard{ m_mutex };
                task = std::move(m_activationElement);
            }

            if (task.valid()) {
                task();   // executed without holding the lock
            }
        }
    };
}

void test_active_object_01()
{
    using namespace ActivatorObject01;

    Logger::log(std::cout, "Active Object - Simple Approach");

    ActiveObject activeObject;

    auto future{ activeObject.enqueueTask(3, 5) };

    // start the active object
    activeObject.run();

    // continue working in the client thread
    Logger::log(std::cout, "Continue working in the client thread.");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // get the result from the future
    auto [first, second, sum] = future.get();

    Logger::log(std::cout, first, " + ", second, " = ", sum);
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
