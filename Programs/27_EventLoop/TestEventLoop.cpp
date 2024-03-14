// ===========================================================================
// EventLoop.cpp
// ===========================================================================

#include "EventLoop.h"

#include "../Logger/Logger.h"

// ===========================================================================

static void test_event_loop_01()
{
    Logger::log(std::cout, "Start:");

    EventLoop eventLoop;
    eventLoop.start();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_event_loop_02()
{
    Logger::log(std::cout, "Start:");

    EventLoop eventLoop;

    auto event = []() { Logger::log(std::cout, "Message from event function"); };

    eventLoop.enqueue(event);

    eventLoop.start();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void worker()
{
    Logger::log(std::cout, "Worker starting ...");
    std::thread::id tid{ std::this_thread::get_id() };
    Logger::log(std::cout, "Workers TID: ", tid);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    Logger::log(std::cout, "Worker done.");
}

static void test_event_loop_03()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    for (size_t i{}; i != 5; ++i) {

        std::function<void()> w{ worker };
        eventLoop.enqueue(w);
    }

    eventLoop.start();

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
            Logger::log(std::cout, "Worker starting ...");
            std::thread::id tid{ std::this_thread::get_id() };
            Logger::log(std::cout, "Workers TID: ", tid);
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            Logger::log(std::cout, "Worker done."); 
        });
    }

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// dealing with result of an event

static void test_event_loop_05()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    auto func = [](int x, int y, int z) {
        Logger::log(std::cout, "func: adding", x, ", ", y, " and ", z);
        return x + y + z;
    };

    auto result = eventLoop.enqueueSync(func, 1, 2, 3);

    Logger::log(std::cout, "Result: ", result);
    Logger::log(std::cout, "Done.");
}

static void test_event_loop_06()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    auto func = [](int x, int y, int z) {
        Logger::log(std::cout, "func: adding", x, ", ", y, " and ", z);
        return x + y + z;
        };

    eventLoop.enqueue(worker);

    auto result = eventLoop.enqueueSync(func, 1, 2, 3);

    eventLoop.enqueue(worker);

    Logger::log(std::cout, "Result: ", result);
    Logger::log(std::cout, "Done.");
}


// ===========================================================================


void test_event_loop()
{
    // test_event_loop_01();
    //test_event_loop_02();
    //test_event_loop_03();
    test_event_loop_04();
    //test_event_loop_05();
    //test_event_loop_06();
}

// ===========================================================================
// End-of-File
// ===========================================================================
