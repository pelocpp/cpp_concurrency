// ===========================================================================
// JThread is cooperatively interruptible // JThread_02.cpp
// ===========================================================================

#include "../Logger/Logger.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

namespace JoinableThreadCooperativeInterruptibility {

    static void sleep(int seconds)
    {
        std::this_thread::sleep_for(std::chrono::seconds{ seconds });
    }

    // =============================================
    // Szenario 1:
    // class std::thread -- endless loop

    static void jthread_01()
    {
        Logger::log(std::cout, "Starting main thread ...");

        std::thread t {

            [] () {
                while (true) {
                    Logger::log(std::cout, "Working ...");
                    sleep(1);
                }
            }
        };

        sleep(5);
        Logger::log(std::cout, "Leaving main thread ...");
        t.join();
    }

    // =============================================
    // Szenario 2:
    // class std::jthread -- endless loop

    static void jthread_02()
    {
        Logger::log(std::cout, "Starting main thread ...");

        std::jthread jt {

            [] () {
                while (true) {
                    Logger::log(std::cout, "Working ...");
                    sleep(1);
                }
            }
        };

        sleep(5);
        Logger::log(std::cout, "Leaving main thread ...");
    }

    // =============================================
    // Szenario 3:
    // class std::jthread -- using request_stop -- still endless loop

    static void jthread_03()
    {
        Logger::log(std::cout, "Starting main thread ...");

        std::jthread jt{

            [] () {
                while (true) {
                    Logger::log(std::cout, "Working ...");
                    sleep(1);
                }
            }
        };

        sleep(5);
        jt.request_stop();
        Logger::log(std::cout, "Leaving main thread ...");
    }

    // =============================================
    // Szenario 4:
    // class std::jthread -- using request_stop -- no more endless loop

    static void jthread_04()
    {
        Logger::log(std::cout, "Starting main thread ...");

        std::jthread jt{
            [] (std::stop_token token) {
                while (!token.stop_requested()) {
                    Logger::log(std::cout, "Working ...");
                    sleep(1);
                }
            }
        };

        sleep(5);
        jt.request_stop();
        Logger::log(std::cout, "Leaving main thread ...");
    }

    // =============================================
    // Szenario 5:
    // class std::jthread -- using request_stop

    static void jthread_05()
    {
        Logger::log(std::cout, "Starting main thread ...");

        std::jthread jt {

            [] (std::stop_token token) {

                std::atomic<bool> running { true };

                // called on a stop request
                std::stop_callback callback {
                    token, 
                    [&] () {
                        Logger::log(std::cout, "Stop requested");
                        running = false;
                    }
                };

                while (running) {
                    Logger::log(std::cout, "Working ...");
                    sleep(1);
                }
            }
        };

        sleep(5);
        jt.request_stop();
        Logger::log(std::cout, "Leaving main thread ...");
    }

    // =============================================
    // Szenario 6:
    // class std::jthread -- using request_stop -- introducing std::stop_source

    static void jthread_06()
    {
        Logger::log(std::cout, "Starting main thread ...");

        std::jthread jt{

            [] (std::stop_token token) {

                std::atomic<bool> running { true };

                // called on a stop request
                std::stop_callback callback {
                    token,
                    [&] () {
                        Logger::log(std::cout, "Stop requested");
                        running = false;
                    }
                };

                while (running) {
                    Logger::log(std::cout, "Working ...");
                    sleep(1);
                }
            }
        };

        sleep(5);
        std::stop_source source{ jt.get_stop_source() };
        source.request_stop();  // request stop on stop token of thread 'jt'
        Logger::log(std::cout, "Leaving main thread ...");
    }
}


void test_jthread_02()
{
    using namespace JoinableThreadCooperativeInterruptibility;

    jthread_01();
    jthread_02();
    jthread_03();
    jthread_04();
    jthread_05();
    jthread_06();
}


// ===========================================================================
// End-of-File
// ===========================================================================
