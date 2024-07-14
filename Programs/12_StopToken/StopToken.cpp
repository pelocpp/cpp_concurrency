// ===========================================================================
// StopToken.cpp // std::stop_token
// ===========================================================================

#include <iostream>
#include <queue>
#include <thread>
#include <future>
#include <stop_token>
#include <mutex>
#include <condition_variable>

#include "../Logger/Logger.h"

namespace Stop_Tokens_and_Condition_Variables
{
    static void test_01()
    {
        std::queue<std::string>     m_messages;
        std::mutex                  m_mutex;
        std::condition_variable_any m_condition_variable;

        auto task = [&] (std::stop_token token) {
            
            while (! token.stop_requested()) {

                std::string msg{};

                std::cout << "Waiting  ..." << std::endl;

                {
                    std::unique_lock lock{ m_mutex };

                    // wait for the next message
                    bool stopRequested {
                        m_condition_variable.wait(
                            lock,
                            token,
                            [&] () { return ! m_messages.empty(); }
                        )
                    };

                    if (!stopRequested) {
                        std::cout << "Stop has been requested!" << std::endl;
                        return;
                    }

                    // retrieve the next message from the queue
                    msg = m_messages.front();
                    m_messages.pop();
                }

                // print the next message:
                std::cout << "Message: " << msg << std::endl;
            }
        };

        std::jthread t { task };

        std::cout << "Pushing strings into queue" << std::endl;

        // store three messages
        for (const auto s : { "Tic" , "Tac", "Toe"  }) {

            std::lock_guard guard { m_mutex };
            m_messages.push(s);
        }

        // notify waiting thread
        m_condition_variable.notify_one();

        // after some time, store another message
        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        {
            std::lock_guard guard{ m_mutex };
            m_messages.push("Done");
        }

        // notify waiting thread
        m_condition_variable.notify_one();

        // after some time, end program (requests stop, which interrupts wait())
        std::this_thread::sleep_for(std::chrono::seconds{ 3 });
    }
}

// =============================================================================

namespace Using_Stop_Callbacks
{
    static void task_demo(std::stop_token token, int num)
    {
        // register temporary callback
        std::stop_callback cb{
            token,
            [] { std::cout << "stop requested"; }
        };

        // ...

    } // unregisters callback is unregistered

    // =============================================================================

    static void task(std::stop_token token, int num)
    {
        Logger::log(std::cout, "Task");

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

        std::this_thread::sleep_for(std::chrono::seconds{ 4 });

        Logger::log(std::cout, "Done Task");
    }

    static void test_02()
    {
        Logger::log(std::cout, "Main");

        // create stop source and stop token
        std::stop_source source;
        std::stop_token token{ source.get_token() };

        // register callback
        std::stop_callback cb{ 
            token,
            [] {
                Logger::log(std::cout, "Main: Stop requested");
            } 
        };

        // request stop before task has been created
        source.request_stop();

        std::future<void> future {
            std::async(std::launch::async, [token] { task(token, 123); })
        };

        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        // request stop after task has been created
        // (runs any associated callbacks on this thread)
        // source.request_stop();

        Logger::log(std::cout, "Done Main");
    }
}

void test_stop_tokens_01()
{
    Stop_Tokens_and_Condition_Variables::test_01();
}

void test_stop_tokens_02()
{
    Using_Stop_Callbacks::test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
