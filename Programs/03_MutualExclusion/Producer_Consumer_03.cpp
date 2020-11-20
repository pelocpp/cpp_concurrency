// ===========================================================================
// Producer Consumer Problem: Monitor Concept by Dijkstra
// ===========================================================================

#include <iostream>
#include <array>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

#include "../Logger/Logger.h"

namespace ConsumerProducerThree
{
    class ConsumerProducer
    {
    public:
        ConsumerProducer() : m_index{ -1 }, m_counter{ 1 } {}

    private:
        // array - considered as stack
        std::array<int, 10> m_data;  // shared object
        int m_index;

        // take care of concurrent stack access
        std::mutex m_mutex;

        // monitor concept (Dijkstra)
        std::condition_variable m_conditionIsEmpty;
        std::condition_variable m_conditionIsFull;

        int sleepTimeProducer = 2000;  // milli seconds
        int sleepTimeConsumer = 2000;  // milli seconds

        int m_counter;

        void produce() {

            while (true) {

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(sleepTimeProducer)
                );

                m_counter++;

                // RAII
                std::unique_lock guard(m_mutex);

                // is stack full?
                m_conditionIsFull.wait(
                    guard,
                    [&]() -> bool { return m_index < 9; }
                );

                // "Lost Wakeup and Spurious Wakeup"
                if (m_index < 9) {

                    m_index++;
                    m_data.at(m_index) = m_counter;
                    Logger::log(std::cout, "pushed ", m_counter, " at index ", m_index);

                    // wakeup any sleeping consuments
                    m_conditionIsEmpty.notify_all();
                }
            }
        }

        void consume() {

            while (true) {

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(sleepTimeConsumer));

                int gotNumber = 0;

                {
                    // RAII
                    std::unique_lock guard(m_mutex);  // Dijkstra Monitor

                    // is stack empty?
                    m_conditionIsEmpty.wait(
                        guard,
                        [&]() -> bool { return m_index >= 0; }
                    );

                    // "Lost Wakeup and Spurious Wakeup"
                    if (m_index >= 0) {

                        int value = m_data.at(m_index);
                        m_index--;
                        Logger::log(std::cout, "popped ", (m_index + 1), " at index ", m_index);

                        // wakeup any sleeping producers
                        m_conditionIsFull.notify_all();
                    }
                }
            }
        }

    public:
        void run() {
            std::thread producer(&ConsumerProducer::produce, this);
            std::thread consumer(&ConsumerProducer::consume, this);
            producer.join();
            consumer.join();
        }
    };
}

void test_mutual_exclusion_03()
{
    using namespace ConsumerProducerThree;
    ConsumerProducer cp;
    cp.run();
}

// ===========================================================================
// End-of-File
// ===========================================================================
