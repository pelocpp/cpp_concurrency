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
    constexpr std::chrono::milliseconds SleepTimeConsumer { 100 };
    constexpr std::chrono::milliseconds SleepTimeProducer { 200 };

    class ConsumerProducer
    {
    public:
        ConsumerProducer() : m_index{ -1 } {}

    private:
        // array - considered as stack
        std::array<int, 10> m_data{ 0 };  // shared object
        int m_index{};                    // shared index

        // take care of concurrent stack access
        std::mutex m_mutex{};

        // monitor concept (Dijkstra)
        std::condition_variable m_conditionIsEmpty{};
        std::condition_variable m_conditionIsFull{};

        void produce() {

            int nextNumber{};

            while (true) {

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(SleepTimeProducer)
                );

                nextNumber++;

                // RAII
                {
                    std::unique_lock raii{ m_mutex };

                    // Is stack full?
                    // Note: "Lost Wakeup and Spurious Wakeup"
                    m_conditionIsFull.wait(
                        raii,
                        [this]() -> bool { return m_index < 9; }
                    );

                    // guard
                    if (m_index < 9) {

                        m_index++;
                        m_data.at(m_index) = nextNumber;
                        Logger::log(std::cout, "pushed ", nextNumber, " at index ", m_index);
                    }
                }

                // wakeup any sleeping consuments
                m_conditionIsEmpty.notify_all();
            }
        }

        void consume() {

            while (true) {

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(SleepTimeConsumer)
                );

                {
                    // RAII
                    std::unique_lock raii{ m_mutex };  // Dijkstra Monitor

                    // Is stack empty?
                    // Note: "Lost Wakeup and Spurious Wakeup"
                    m_conditionIsEmpty.wait(
                        raii,
                        [this]() -> bool { return m_index >= 0; }
                    );

                    // guard
                    if (m_index >= 0) {

                        int number{ m_data.at(m_index) };
                        Logger::log(std::cout, "popped ", number, " at index ", m_index);
                        m_index--;
                    }
                }

                // wakeup any sleeping producers
                m_conditionIsFull.notify_all();
            }
        }

    public:
        void run() {
            std::thread producer{ &ConsumerProducer::produce, this };
            std::thread consumer{ &ConsumerProducer::consume, this };
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
