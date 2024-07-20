// ===========================================================================
// ProducerConsumerProblem.cpp
// ===========================================================================

#include "BlockingQueue.h"
// #include "BlockingQueueEx.h"

constexpr int NumIterations{ 10 };

constexpr std::chrono::milliseconds SleepTimeConsumer{ 120 };
constexpr std::chrono::milliseconds SleepTimeProducer{  80 };

static void test_thread_safe_blocking_queue_01()
{
    using namespace ProducerConsumerQueue;

    {
        constexpr int QueueSize{ 1 };
        BlockingQueue<int, QueueSize> queue{};
        queue.push(101);
        queue.push(102);    // should block
    }

    {
        constexpr int QueueSize{ 1 };
        BlockingQueue<int, QueueSize> queue{};
        int value;
        queue.pop(value);   // should block
    }
}

// ===========================================================================

static void test_thread_safe_blocking_queue_02()
{
    using namespace ProducerConsumerQueue;

    constexpr int QueueSize{ 5 };

    BlockingQueue<int, QueueSize> queue{};

    std::thread producer([&]() {

        Logger::log(std::cout, "Producer");

        for (int i{ 1 }; i <= NumIterations; ++i)
        {
            queue.push(100 + i);
            Logger::log(std::cout, "Pushing ", 100 + i);
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
            Logger::log(std::cout, "Popped  ", value);
        }

        Logger::log(std::cout, "Consumer Done.");
    });

    producer.join();
    consumer.join();
}

// ===========================================================================

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
                std::chrono::milliseconds{ SleepTimeConsumer }
            );

            int value;
            m_queue.pop(value);

            Logger::log(std::cout, "Popped ", value);
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
                std::chrono::milliseconds{ SleepTimeProducer }
            );

            nextNumber++;
            m_queue.push(nextNumber);

            Logger::log(std::cout, "Pushed ", nextNumber);
        }
    }
};

// ===========================================================================

static void test_thread_safe_blocking_queue_03()
{
    constexpr int QueueSize{ 10 };

    ProducerConsumerQueue::BlockingQueue<int, QueueSize> queue{ };

    Consumer c{ queue };
    Producer p{ queue };

    std::thread producer([&] () {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
    });

    std::thread consumer([&] () {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
    });

    producer.join();
    consumer.join();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

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

// ===========================================================================

class Person
{
private:
    std::string m_name;

public:
    Person() {}
    Person(const std::string& name) 
        : m_name{ name }
    { std::cout << "c'tor Person" << std::endl; }

    Person(const Person& person)
        : m_name{ person.m_name }
    { std::cout << "copy c'tor Person" << std::endl; }
    
    Person(Person&& person) noexcept
        : m_name{ std::move(person.m_name) }
    { std::cout << "move c'tor Person" << std::endl; }

    ~Person() { std::cout << "d'tor Person" << std::endl; }

    Person& operator= (const Person& person) {
        std::cout << "Person copy operator=" << std::endl;
        m_name = person.m_name;
        return *this;
    }

    Person& operator= (Person&& person) noexcept {
        std::cout << "Person move operator=" << std::endl;
        m_name = std::move(person.m_name);
        return *this;
    }
};


static void test_thread_safe_blocking_queue_05()
{

    constexpr int QueueSize{ 5 };

    ProducerConsumerQueue::BlockingQueue<Person, QueueSize> queue{ };

    queue.push(Person{ "Hans" });
    queue.push(Person{ "Sepp" });
    queue.push(Person{ "Franz" });

    Person p;
    queue.pop(p);
    queue.pop(p);
    queue.pop(p);
}

// ===========================================================================

void test_producer_consumer_problem()
{
    test_thread_safe_blocking_queue_01();
    test_thread_safe_blocking_queue_02();
    test_thread_safe_blocking_queue_03();
    test_thread_safe_blocking_queue_04();
    test_thread_safe_blocking_queue_05();
}

// ===========================================================================
// End-of-File
// ===========================================================================
