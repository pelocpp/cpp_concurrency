// ===========================================================================
// Producer Consumer Problem
// ===========================================================================

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <chrono>
#include <thread>
#include <functional>

#include "../Logger/Logger.h"

namespace ProducerConsumer {

    class ProducerConsumer {

    public:
        ProducerConsumer() : m_stopped(false) {}

    private:
        std::queue<int> m_queue;
        bool m_stopped;
        std::condition_variable m_condition;
        std::mutex m_mutex;

        std::function<void()> producer = [&]() {

            int count = 5;
            while (count--)
            {
                {
                    // RAII idiom
                    std::scoped_lock<std::mutex> lock{ m_mutex };
                    m_queue.push(count);
                    Logger::log(std::cout, "producer pushed: ", count);

                    // wakeup consumer
                    m_condition.notify_one();
                }

                Logger::log(std::cout, "sleeping ... ", count);
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }

            // all done - acquire the lock, set the stopped flag,
            // finally inform the consumer.
            std::scoped_lock<std::mutex> lock{ m_mutex };
            m_stopped = true;
            m_condition.notify_one();
            Logger::log(std::cout, "< Producer");
        };

        std::function<void()> consumer = [&]() {

            do {
                // RAII idiom
                std::unique_lock<std::mutex> lock(m_mutex);

                m_condition.wait(lock, [&]() {
                    // returns 'false' if waiting should be continued
                    bool condition = m_stopped || !m_queue.empty();
                    Logger::log(std::cout, "wait -> ", condition, '\n');
                    return condition;
                    }
                );

                // we own the mutex here; pop the queue until it is empty
                std::cout << "testing rest ...." << std::endl;

                while (!m_queue.empty())
                {
                    int elem = m_queue.front();
                    m_queue.pop();
                    Logger::log(std::cout, "Consumer popped: ", elem);
                }

                if (m_stopped) {
                    Logger::log(std::cout, "Consumer is done!");
                    break;
                }

            } while (true);

        };

    public:
        void run() {
            std::thread t1(producer);
            std::thread t2(consumer);
            t1.join();
            t2.join();
        }
    };

    void test() {
        ProducerConsumer cp;
        cp.run();
    }
}

//int main()
//{
//    using namespace ProducerConsumer;
//    test();
//    return 1;
//}

// ===========================================================================
// End-of-File
// ===========================================================================
