// ===========================================================================
// Producer Consumer Problem
// ===========================================================================

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <chrono>
#include <thread>

#include "../Logger/Logger.h"

namespace ConsumerProducerTwo
{
    class ConsumerProducer {

    private:
        std::queue<int> m_data;
        std::mutex m_mutex;
        std::condition_variable m_condition;

    public:
        ConsumerProducer() = default;

        void produce() {

            int nextNumber = 0;

            while (true) {

                // sleep a second
                std::this_thread::sleep_for(std::chrono::seconds(1));

                nextNumber++;

                // RAII idiom
                {
                    std::scoped_lock<std::mutex> lock(m_mutex);

                    m_data.push(nextNumber);
                    m_condition.notify_one();  // wakeup consumer, if any
                }

                Logger::log(std::cout, "Added ", nextNumber, " to queue.");
            }
        }

        void consume() {
            int number;

            while (true) {

                number = 0;

                // RAII idiom
                {
                    std::unique_lock<std::mutex> lock(m_mutex);

                    m_condition.wait(
                        lock,
                        [this]() -> bool {
                            // return 'false' if waiting should be continued
                            bool condition = ! m_data.empty();
                            Logger::log(std::cout, "Wait -> ", condition);
                            return condition;
                        }
                    );

                    if (!m_data.empty()) {
                        number = m_data.front();
                        m_data.pop();
                    }
                }

                if (number > 0) {
                    Logger::log(std::cout, "Popped ", number, '.');
                }
                else {
                    Logger::log(std::cout, "Stack empty.");
                }
            }
        }

    public:
        void run() {
            std::thread t1(&ConsumerProducer::produce, this);
            std::thread t2(&ConsumerProducer::consume, this);
            t1.join();
            t2.join();
        }
    };
}

void test_mutual_exclusion_02() {
    using namespace ConsumerProducerTwo;
    ConsumerProducer cp;
    cp.run();
}

// ===========================================================================
// End-of-File
// ===========================================================================
