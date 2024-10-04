// ===========================================================================
// StopCallback.cpp // std::stop_callback
// ===========================================================================

#include <iostream>
//#include <queue>
#include <thread>
#include <future>
#include <stop_token>
//#include <mutex>
#include <condition_variable>

#include "../Logger/Logger.h"

// =============================================================================
// =============================================================================

namespace Using_Stop_Callbacks
{
    static void task(std::stop_token token, int num)
    {
        Logger::log(std::cout, "Enter Task");

        auto id{ std::this_thread::get_id() };

        // register a stop callback
        std::stop_callback cb{
            token,
            [=] {
                auto currentId{ std::this_thread::get_id() };

                if (currentId == id) {
                    Logger::log(std::cout, "Task: Stop requested - Thread Context = Task");
                }
                else {
                    Logger::log(std::cout, "Task: Stop requested - Thread Context = Main");
                }
            }
        };

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        Logger::log(std::cout, "Done Task");
    }

    static void test()
    {
        Logger::log(std::cout, "Main");

        // create stop source and stop token
        std::stop_source source;
        std::stop_token token{ source.get_token() };

        // A) request stop before task has been created
        source.request_stop();                   // put either this line into comment ...

        std::future<void> future{
            std::async(std::launch::async, [token] { task(token, 123); })
        };

        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        // B) request stop after task has been created
        // (runs any associated callbacks on this thread)
        // source.request_stop();                  // or put this line into comment

        Logger::log(std::cout, "Done Main");
    }
}

void test_stop_callback()
{
    Using_Stop_Callbacks::test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
