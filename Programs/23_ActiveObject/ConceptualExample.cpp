// ===========================================================================
// ConceptualExample.cpp // Active Object Pattern
// ===========================================================================

#include "../Logger/Logger.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

// ---------------------------------------------------------------------------
// OriginalClass is a regular class,
// that provides two methods that set a double to be a certain value.
// The two methods shouldn't be called at the same time in different threads,
// furthermore does this class NOT conform to the active object pattern.

class OriginalClass
{
private:
    double m_value;

public:
    OriginalClass() : m_value{} {}

    void doSomething() {
        Logger::log(std::cout, "doSomething");
        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        m_value = 1.0;
    }

    void doSomethingElse() {
        Logger::log(std::cout, "doSomethingElse");
        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        m_value = 2.0;
    }
};

// ---------------------------------------------------------------------------
// Alternate implementation using an Active Object approach

using Operation = std::function<void()>;

class DispatchQueue {

    mutable std::mutex      m_mutex;
    std::condition_variable m_empty;
    std::queue<Operation>   m_queue;

public:
    void put(Operation op)
    {
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_queue.push(std::move(op));
        }

        m_empty.notify_one();
    }

    Operation take()
    {
        std::unique_lock<std::mutex> guard{ m_mutex };

        m_empty.wait(
            guard,
            [&] { return !m_queue.empty(); }
        );

        Operation op = std::move(m_queue.front());
        m_queue.pop();
        return op;
    }

    std::size_t size() const
    {
        std::lock_guard<std::mutex> guard{ m_mutex };
        return m_queue.size();
    }
};

class ActiveObject
{
private:
    DispatchQueue                m_dispatchQueue;
    bool                         m_done;
    double                       m_value;
    std::thread                  m_worker; // Keep last so other members initialize first!

public:
    // c'tors/ d'tor
    ActiveObject() 
        : m_value{}, m_done{}, m_worker{ &ActiveObject::run, this }
    {}

    ~ActiveObject() { 
        // enqueue a "poison pill" to stop the loop after all pending tasks have finished
        m_dispatchQueue.put(
            [this]() { m_done = true; }
        );
        
        if (m_worker.joinable()) {
            m_worker.join();
        }
    }

    // no copy/move operations since a running thread holds 'this'
    ActiveObject(const ActiveObject&) = delete;
    ActiveObject& operator=(const ActiveObject&) = delete;
    ActiveObject(ActiveObject&&) noexcept = delete;
    ActiveObject& operator=(ActiveObject&&) noexcept = delete;

private:
    void run()
    {
        Logger::log(std::cout, "> Worker thread started");

        while (! m_done) 
        {
            Operation op = m_dispatchQueue.take();
            op();
        }

        Logger::log(std::cout, "< Worker thread stopped");
    }
    
public:
    // This is part of the public interface of this class.
    // Each method is composed of the actual code we want to execute,
    // and adorn code which queues our code.

    void doSomething()
    {
        Logger::log(std::cout, "Client: queuing  doSomething");

        // Actual code to be executed is stored in a lambda
        // and pushed to the 'activation list' queue
        m_dispatchQueue.put(
            [this] () {
                Logger::log(std::cout, "Worker: executing doSomething");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                m_value = 1.0;
            }
        );
    }

    void doSomethingElse()
    {
        Logger::log(std::cout, "Client: queuing  doSomethingElse");

        m_dispatchQueue.put(
            [this] () {
                Logger::log(std::cout, "Worker: executing doSomethingElse");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                m_value = 2.0; 
            }
        );
    }
};

static void test_conceptual_example_01()
{
    Logger::log(std::cout, "OriginalClass Object");

    OriginalClass object;
    object.doSomething();
    object.doSomethingElse();

    Logger::log(std::cout, "Done.");
}

static void test_conceptual_example_02()
{
    Logger::log(std::cout, "Active Object");

    ActiveObject activeObject;
    activeObject.doSomething();
    activeObject.doSomethingElse();

    Logger::log(std::cout, "Done.");
}

void test_conceptual_example()
{
    Logger::log(std::cout, "Start");

    test_conceptual_example_01();
    test_conceptual_example_02();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
