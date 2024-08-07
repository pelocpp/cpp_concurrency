// ===========================================================================
// TestEventLoop.cpp
// ===========================================================================

#include "EventLoop.h"

#include "../Logger/Logger.h"

// ===========================================================================

static void test_event_loop_01()
{
    Logger::log(std::cout, "Start:");

    EventLoop eventLoop;

    eventLoop.start();
    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_event_loop_02()
{
    Logger::log(std::cout, "Start:");

    EventLoop eventLoop;

    std::function<void()> event{
        [] () {
            Logger::log(std::cout, "Message from Function"); 
        } 
    };

    eventLoop.enqueue(event);

    eventLoop.start();

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void function ()
{
    std::thread::id tid{ std::this_thread::get_id() };
    Logger::log(std::cout, "Function starting: TID: ", tid);
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
    Logger::log(std::cout, "Function done.");
}

static void test_event_loop_03()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    for (size_t i{}; i != 5; ++i) {

        std::function<void()> worker{ function };

        eventLoop.enqueue(worker);
    }

    eventLoop.start();

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_event_loop_04()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    eventLoop.start();

    for (size_t i{}; i != 5; ++i) {

        eventLoop.enqueue([](){
            std::thread::id tid{ std::this_thread::get_id() };
            Logger::log(std::cout, "Worker starting: TID: ", tid);
            std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
            Logger::log(std::cout, "Worker done."); 
        });
    }

    Logger::log(std::cout, "All enqueue's done");

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_event_loop_10()
{
    Logger::log(std::cout, "Start:");

    std::function<void(int)> task{ [] (int value) {
        Logger::log(std::cout, "Value: ", value); }
    };

    EventLoop eventLoop;

    eventLoop.enqueueTask(task, 123);
    eventLoop.enqueueTask(task, 456);
    eventLoop.enqueueTask(task, 789);

    Logger::log(std::cout, "Starting Event Loop:");

    eventLoop.start();

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static bool isPrime(size_t number)
{
    if (number <= 2) {
        return number == 2;
    }

    if (number % 2 == 0) {
        return false;
    }

    // check odd divisors from 3 to the square root of the number
    size_t end{ static_cast<size_t>(ceil(std::sqrt(number))) };
    for (size_t i{ 3 }; i <= end; i += 2) {

        if (number % i == 0) {
            return false;  // number not prime
        }
    }

    return true; // found prime number
}

static void test_event_loop_20()
{
    Logger::log(std::cout, "Start:");

    size_t foundPrimeNumbers{};

    std::function<void(size_t)> primeTask{ [&] (size_t value) {

            bool primeFound { isPrime(value) };

            if (primeFound) {
                Logger::log(std::cout, "> ", value, " IS prime.");

                ++foundPrimeNumbers;
            }
        }
    };

    EventLoop eventLoop;

    Logger::log(std::cout, "Enqueuing tasks");

    const size_t Start = 1000000000001;
    const size_t End = Start + 100;

    //const size_t Start = 1;
    //const size_t End = Start + 100;

    for (size_t i = Start; i < End; i += 2) {
        eventLoop.enqueueTask(primeTask, i);
    }

    Logger::log(std::cout, "Starting Event Loop:");

    eventLoop.start();

    eventLoop.stop();

    Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Start, " and ", End, '.');

    Logger::log(std::cout, "Done.");
}

static void test_event_loop_21()
{
    Logger::log(std::cout, "Start:");

    size_t foundPrimeNumbers{};

    std::function<void(size_t)> primeTask{ [&] (size_t value) {

            bool primeFound { isPrime(value) };

            if (primeFound) {
                Logger::log(std::cout, "> ", value, " IS prime.");

                ++foundPrimeNumbers;
            }
        }
    };

    EventLoop eventLoop;

    Logger::log(std::cout, "Enqueuing tasks");

    const size_t Start = 1000000000001;
    const size_t End = Start + 100;

    Logger::log(std::cout, "Starting Event Loop:");

    eventLoop.start();

    for (size_t i = Start; i < End; i += 2) {
        eventLoop.enqueueTask(primeTask, i);
    }

    eventLoop.stop();

    Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Start, " and ", End, '.');

    Logger::log(std::cout, "Done.");
}

void test_event_loop()
{
    test_event_loop_01();  // just starting and stopping event loop
    test_event_loop_02();  // demonstrating event loop with one event
    test_event_loop_03();  // demonstrating event loop with five events
    test_event_loop_04();  // demonstrating event loop with five lambdas
    test_event_loop_10();  // demonstrating enqueuing events with parameters
    test_event_loop_20();  // searching prime numbers: first enqueuing events, than starting calculations
    test_event_loop_21();  // searching prime numbers: enqueuing events while event loop is active
}

// ===========================================================================
// End-of-File
// ===========================================================================
