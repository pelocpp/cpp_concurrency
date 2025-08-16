// ===========================================================================
// ProducerConsumerProblem.cpp
// ===========================================================================


#include "BlockingQueue.h"
// #include "BlockingQueueEx.h"

#include <iostream>

constexpr int NumIterations{ 10 };

constexpr std::chrono::milliseconds SleepTimeConsumer{ 120 };
constexpr std::chrono::milliseconds SleepTimeProducer{  80 };

void test_thread_safe_blocking_queue_01()
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

void test_thread_safe_blocking_queue_02()
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

template<typename T, size_t QueueSize = 10>
class Consumer
{
private:
    ProducerConsumerQueue::BlockingQueue<T, QueueSize>& m_queue;

public:
    // c'tor
    Consumer(ProducerConsumerQueue::BlockingQueue<T, QueueSize>& queue)
        : m_queue{ queue }
    {}

    void consume() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds{ SleepTimeConsumer }
            );

            T value;
            m_queue.pop(value);

            Logger::log(std::cout, "Popped ", value);
        }
    }
};

template<typename T, size_t QueueSize = 10>
class Producer
{
private:
    ProducerConsumerQueue::BlockingQueue<T, QueueSize>& m_queue;

public:
    // c'tor
    Producer(ProducerConsumerQueue::BlockingQueue<T, QueueSize>& queue)
        : m_queue{ queue }
    {}

    void produce()
    {
        T elem{};

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds{ SleepTimeProducer }
            );

            m_queue.push(elem);

            Logger::log(std::cout, "Pushed ", elem);
        }
    }
};

// ===========================================================================

void test_thread_safe_blocking_queue_03()
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

void test_thread_safe_blocking_queue_04()
{
    constexpr int QueueSize{ 10 };

    ProducerConsumerQueue::BlockingQueue<int, QueueSize> queue{ };

    Consumer<int> consumer{ queue };
    Producer<int> producer{ queue };

    auto producerProcedure = [&]() {
        Logger::log(std::cout, "Producer");
        producer.produce();
        Logger::log(std::cout, "Producer Done.");
        };

    auto consumerProcedure = [&]() {
        Logger::log(std::cout, "Consumer");
        consumer.consume();
        Logger::log(std::cout, "Consumer Done.");
        };

    std::thread producer1(producerProcedure);
    std::thread producer2(producerProcedure);
    std::thread producer3(producerProcedure);

    std::thread consumer1(consumerProcedure);
    std::thread consumer2(consumerProcedure);
    std::thread consumer3(consumerProcedure);

    producer1.join();
    producer2.join();
    producer3.join();

    consumer1.join();
    consumer2.join();
    consumer3.join();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

constexpr bool Verbose{ true };

class Person
{
private:
    std::string m_name;

public:
    Person() {}

    Person(const std::string& name) : m_name{ name }
    { 
        if (Verbose) {
            std::cout << "c'tor Person" << std::endl;
        }
    }

    Person(const Person& person) : m_name{ person.m_name }
    {
        if (Verbose) {
            std::cout << "copy c'tor Person" << std::endl;
        }
    }
    
    Person(Person&& person) noexcept : m_name{ std::move(person.m_name) }
    { 
        if (Verbose) {
            std::cout << "move c'tor Person" << std::endl;
        }
    }

    ~Person()
    {
        if (Verbose) {
            std::cout << "d'tor Person" << std::endl;
        }
    }

    Person& operator= (const Person& person) {
        if (Verbose) {
            std::cout << "Person copy operator=" << std::endl;
        }

        m_name = person.m_name;
        return *this;
    }

    Person& operator= (Person&& person) noexcept {
        if (Verbose) {
            std::cout << "Person move operator=" << std::endl;
        }

        m_name = std::move(person.m_name);
        return *this;
    }

    // getter
    const std::string& getName() const { return m_name; }
};

static std::ostream& operator<< (std::ostream& os, const Person& p) {
    os << "Person: " << p.getName();
    return os;
}

// ===========================================================================

void test_thread_safe_blocking_queue_05()
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

void test_thread_safe_blocking_queue_06()
{
    constexpr int QueueSize{ 10 };

    ProducerConsumerQueue::BlockingQueue<Person, QueueSize> queue{ };

    Consumer<Person> consumer{ queue };
    Producer<Person> producer{ queue };

    auto producerProcedure = [&]() {
        Logger::log(std::cout, "Producer");
        producer.produce();
        Logger::log(std::cout, "Producer Done.");
        };

    auto consumerProcedure = [&]() {
        Logger::log(std::cout, "Consumer");
        consumer.consume();
        Logger::log(std::cout, "Consumer Done.");
        };

    std::thread producer1(producerProcedure);
    std::thread producer2(producerProcedure);
    std::thread producer3(producerProcedure);

    std::thread consumer1(consumerProcedure);
    std::thread consumer2(consumerProcedure);
    std::thread consumer3(consumerProcedure);

    producer1.join();
    producer2.join();
    producer3.join();

    consumer1.join();
    consumer2.join();
    consumer3.join();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

// ===========================================================================
// End-of-File
// ===========================================================================
