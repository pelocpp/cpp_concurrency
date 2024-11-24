// ===========================================================================
// ConceptualExample.cpp // Active Object Pattern
// ===========================================================================

#include <iostream>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>

#include "../Logger/Logger.h"

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
        m_value = 1.0;
    }

    void doSomethingElse() {
        Logger::log(std::cout, "doSomethingElse");
        m_value = 2.0;
    }
};

// ---------------------------------------------------------------------------
// Alternate implementation using an Active Object approach

using Operation = std::function<void()>;

class DispatchQueue {

    std::mutex              m_mutex;
    std::condition_variable m_empty;
    std::queue<Operation>   m_queue;

public:
    void put(Operation op)
    {
        std::lock_guard<std::mutex> guard{ m_mutex };

        m_queue.push(op);
        m_empty.notify_one();
    }

    Operation take()
    {
        std::unique_lock<std::mutex> guard{ m_mutex };

        m_empty.wait(guard, [&] { return ! m_queue.empty(); });

        Operation op = m_queue.front();
        m_queue.pop();
        return op;
    }

    size_t size()
    {
        std::lock_guard<std::mutex> guard{ m_mutex };
        return m_queue.size();
    }
};

class ActiveObject
{
private:
    DispatchQueue                m_dispatchQueue;
    std::atomic<bool>            m_done;
    std::unique_ptr<std::thread> m_runnable;
    double                       m_value;

public:
    // c'tors/ d'tor
    ActiveObject() : m_value{}, m_done{}
    {
        // Start the thread that this object will "occupy".
        // The public methods of this object will run in the 
        // context of this new thread
        m_runnable = std::make_unique<std::thread>(&ActiveObject::run, this);
    }

    ~ActiveObject() { 
        m_runnable->join();
    }

    // public interface
    void run()
    {
        while (! m_done) 
        {
            Operation operation = m_dispatchQueue.take();

            m_done = m_dispatchQueue.size() == 0;

            operation();
        }
    }

    // This is part of the public interface of this class.
    // Each method is composed of the actual code we want to execute,
    // and adorn code which queues our code.
    void doSomething()
    {
        Logger::log(std::cout, "preparing doSomething");

        // Actual code to be executed is stored in a lambda
        // and pushed to the 'activation list' queue
        auto task{ [this] () {
            Logger::log(std::cout, "doSomething");
            m_value = 1.0; }
        };

        m_dispatchQueue.put(task);
    }

    void doSomethingElse()
    {
        Logger::log(std::cout, "preparing doSomethingElse");

        m_dispatchQueue.put( { [this] () {
            Logger::log(std::cout, "doSomethingElse");
            m_value = 2.0; }
        });
    }
};

static void test_conceptual_example_00()
{
    Logger::log(std::cout, "OriginalClass Object");

    OriginalClass object;
    object.doSomething();
    object.doSomethingElse();

    Logger::log(std::cout, "Done.");
}

static void test_conceptual_example_01()
{
    Logger::log(std::cout, "Active Object");

    ActiveObject activeObject;
    activeObject.doSomething();
    activeObject.doSomethingElse();

    Logger::log(std::cout, "Done.");
}

void test_conceptual_example()
{
    test_conceptual_example_00();
    test_conceptual_example_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
