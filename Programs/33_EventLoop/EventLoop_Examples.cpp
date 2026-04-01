// ===========================================================================
// EventLoop_Examples.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include "../Globals/GlobalPrimes.h"
#include "../Globals/IsPrime.h"

#include "EventLoop.h"

// ===========================================================================
// just starting and stopping event loop

void test_event_loop_01()
{
    Logger::log(std::cout, "Start");

    EventLoop eventLoop{};

    eventLoop.start();
    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// demonstrating event loop with one event

void test_event_loop_02()
{
    Logger::log(std::cout, "Start");

    EventLoop eventLoop{};

    std::move_only_function<void()> event{
        []() {
            Logger::log(std::cout, "Message from Function");
        }
    };

    eventLoop.enqueue(std::move(event));

    eventLoop.start();

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// demonstrating event loop with five events

static void function ()
{
    std::thread::id tid{ std::this_thread::get_id() };
    Logger::log(std::cout, "Function starting: TID: ", tid);
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
    Logger::log(std::cout, "Function done.");
}

void test_event_loop_03()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop{};

    for (std::size_t i{}; i != 5; ++i) {

        std::move_only_function<void()> worker{ function };
        eventLoop.enqueue(std::move(worker));
    }

    eventLoop.start();

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// demonstrating event loop with five lambdas

void test_event_loop_04()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop{};

    eventLoop.start();

    for (std::size_t i{}; i != 5; ++i) {

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
// demonstrating enqueuing events with parameters

void test_event_loop_10()
{
    Logger::log(std::cout, "Start");

    EventLoop eventLoop{};

    eventLoop.enqueueTask(
        [](int value) {
            Logger::log(std::cout, "Value: ", value);
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        },
        123
    );

    eventLoop.enqueueTask(
        [](int value) {
            Logger::log(std::cout, "Value: ", value);
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        },
        456
    );

    eventLoop.enqueueTask(
        [](int value) {
            Logger::log(std::cout, "Value: ", value);
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        },
        789
    );

    Logger::log(std::cout, "Starting Event Loop:");

    eventLoop.start();

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// demonstrating enqueuing events with several functions with different signatures

static void firstFunction(int value)
{
    std::thread::id tid{ std::this_thread::get_id() };
    Logger::log(std::cout, "firstFunction starting: TID: ", tid);
    Logger::log(std::cout, "Paramters(s): ", value);
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
    Logger::log(std::cout, "firstFunction done.");
}

static void secondFunction(double d1, double d2)
{
    std::thread::id tid{ std::this_thread::get_id() };
    Logger::log(std::cout, "secondFunction starting: TID: ", tid);
    Logger::log(std::cout, "Paramters(s): ", d1, " - ", d2);
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
    Logger::log(std::cout, "secondFunction done.");
}

static void thirdFunction(const std::string& s)
{
    std::thread::id tid{ std::this_thread::get_id() };
    Logger::log(std::cout, "thirdFunction starting: TID: ", tid);
    Logger::log(std::cout, "Paramters(s): ", s);
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
    Logger::log(std::cout, "thirdFunction done.");
}

void test_event_loop_11()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop{};

    eventLoop.start();

    eventLoop.enqueueTask(std::move(firstFunction), 123);

    eventLoop.enqueueTask(std::move(secondFunction), 123.0, 456.0);

    std::string s{ "Hello" };
    eventLoop.enqueueTask(std::move(thirdFunction), s);

    eventLoop.enqueueTask(std::move(thirdFunction), std::string { "Temporary Hello" });

    Logger::log(std::cout, "All enqueue's done");

    eventLoop.stop();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// searching prime numbers: first enqueuing events, than starting calculations

void test_event_loop_20()
{
    Logger::log(std::cout, "Start");

    ScopedTimer clock{};

    std::size_t foundPrimeNumbers{};

    EventLoop eventLoop{};

    // eventLoop.start(); // <=== add/remove comment

    Logger::log(std::cout, "Enqueuing tasks ...");

    for (std::size_t i{ PrimeNumberLimits::Start }; i < PrimeNumberLimits::End; i += 2) {

        eventLoop.enqueueTask(
            [&] (std::size_t value) {

                bool primeFound{ PrimeNumbers::IsPrime(value) };

                if (primeFound) {
                    Logger::log(std::cout, "> ", value, " is prime.");

                    ++foundPrimeNumbers;
                }
            },
            i
        );
    }

    Logger::log(std::cout, "Enqueuing tasks done.");

    Logger::log(std::cout, "Starting Event Loop:");

    eventLoop.start();   // <=== add/remove comment

    eventLoop.stop();

    Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ",
        PrimeNumberLimits::Start, " and ", PrimeNumberLimits::End, '.'
    );

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
