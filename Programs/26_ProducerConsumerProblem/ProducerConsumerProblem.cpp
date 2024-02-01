// ===========================================================================
// ProducerConsumerProblem_01.cpp
// ===========================================================================

#include "BlockingQueue.h"
//#include "BlockingQueueEx.h"

// TBD: test_thread_safe_blocking_queue_02: Stürzt ab ...


constexpr int NumIterations{ 10 };

constexpr std::chrono::milliseconds SleepTimeConsumer{ 105 };
constexpr std::chrono::milliseconds SleepTimeProducer{ 100 };

static void test_thread_safe_blocking_queue_01()
{
    using namespace ProducerConsumerQueue;

    constexpr int QueueSize{ 5 };

    BlockingQueue<int, QueueSize> queue{};

    queue.push(1);
    queue.push(2);
    queue.push(3);

    std::cout << "Size: " << queue.size() << std::endl;
}

static void test_thread_safe_blocking_queue_02()
{
    using namespace ProducerConsumerQueue;

    constexpr int QueueSize{ 5 };

    BlockingQueue<int, QueueSize> queue{};

    std::thread producer([&]() {

        Logger::log(std::cout, "Producer");

        for (int i{ 1 }; i <= NumIterations; ++i)
        {
            queue.push(i);
            Logger::log(std::cout, "Pushing ", i);
        }

        Logger::log(std::cout, "Producer Done.");
    });

    std::thread consumer([&]() {

        Logger::log(std::cout, "Consumer");

        for (int i{ 1 }; i <= NumIterations; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{ SleepTimeConsumer });
            int value;
            queue.pop(value);
            Logger::log(std::cout, "Popped  ", i);
        }

        Logger::log(std::cout, "Consumer Done.");
    });

    producer.join();
    consumer.join();
}

class Consumer
{
private:
    ProducerConsumerQueue::BlockingQueue<int>& m_queue;

public:
    // c'tor
    Consumer(ProducerConsumerQueue::BlockingQueue<int>& queue)
        : m_queue{ queue }
    {}

    void consume() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeConsumer)
            );

            int value;
            m_queue.pop(value);
        }
    }
};

class Producer
{
private:
    ProducerConsumerQueue::BlockingQueue<int>& m_queue;

public:
    // c'tor
    Producer(ProducerConsumerQueue::BlockingQueue<int>& queue)
        : m_queue{ queue }
    {}

    void produce()
    {
        int nextNumber{};

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeProducer)
            );

            nextNumber++;
            m_queue.push(nextNumber);
        }
    }
};

static void test_thread_safe_blocking_queue_03()
{

    constexpr int QueueSize{ 10 };

    ProducerConsumerQueue::BlockingQueue<int, QueueSize> queue{ };

    Consumer c{ queue };
    Producer p{ queue };

    std::thread producer([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
    });

    std::thread consumer([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
    });

    producer.join();
    consumer.join();

    Logger::log(std::cout, "Done.");
}

static void test_thread_safe_blocking_queue_04()
{

    constexpr int QueueSize{ 10 };

    ProducerConsumerQueue::BlockingQueue<int, QueueSize> queue{ };

    Consumer c{ queue };
    Producer p{ queue };

    std::thread producer1([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
    });

    std::thread producer2([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
    });

    std::thread producer3([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
    });


    std::thread consumer1([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
    });

    std::thread consumer2([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
    });

    std::thread consumer3([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
    });

    producer1.join();
    producer2.join();
    producer3.join();

    consumer1.join();
    consumer2.join();
    consumer3.join();

    Logger::log(std::cout, "Done.");
}


void test_producer_consumer_problem()
{
    //test_thread_safe_blocking_queue_01();
    test_thread_safe_blocking_queue_02();
    //test_thread_safe_blocking_queue_03();
    //test_thread_safe_blocking_queue_04();
}

// ===========================================================================
// End-of-File
// ===========================================================================
