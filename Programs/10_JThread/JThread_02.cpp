// ===========================================================================
// JThread is cooperatively interruptible // JThread_02.cpp
// ===========================================================================

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
        std::thread t {

            [] () {
                while (true) {
                    std::cout << "Working ..." << std::endl;
                    sleep(1);
                }
            }
        };

        sleep(5);
        t.join();
    }

    // =============================================
    // Szenario 2:
    // class std::jthread -- endless loop

    static void jthread_02()
    {
        std::jthread jt { 

            [] () {
                while (true) {
                    std::cout << "Working ..." << std::endl;
                    sleep(1);
                }
            }
        };

        sleep(5);
        jt.join();
    }

    // =============================================
    // Szenario 3:
    // class std::jthread -- using request_stop -- still endless loop

    static void jthread_03()
    {
        std::jthread jt{

            [] () {
                while (true) {
                    std::cout << "Working ..." << std::endl;
                    sleep(1);
                }
            }
        };

        sleep(5);
        jt.request_stop();
        jt.join();
    }

    // =============================================
    // Szenario 4:
    // class std::jthread -- using request_stop -- no more endless loop

    static void jthread_04()
    {
        std::jthread jt{
            [] (std::stop_token token) {
                while (!token.stop_requested()) {
                    std::cout << "Working ..." << std::endl;
                    sleep(1);
                }
            }
        };

        sleep(5);
        jt.request_stop();
        jt.join();
    }

    // =============================================
    // Szenario 5:
    // class std::jthread -- using request_stop -- still endless loop

    static void jthread_05()
    {
        std::jthread jt {

            [] (std::stop_token token) {

                std::atomic<bool> running { true };

                // called on a stop request
                std::stop_callback callback {
                    token, 
                    [&] () {
                        std::cout << "Stop requested" << std::endl;
                        running = false;
                    }
                };

                while (running) {
                    std::cout << "Working ..." << std::endl;
                    sleep(1);
                }
            }
        };

        sleep(5);
        jt.request_stop();
        jt.join();
    }

    // =============================================
    // Szenario 6:
    // class std::jthread -- using request_stop -- introducing std::stop_source

    static void jthread_06()
    {
        std::jthread jt{

            [] (std::stop_token token) {

                std::atomic<bool> running { true };

                // called on a stop request
                std::stop_callback callback {
                    token,
                    [&] () {
                        std::cout << "Stop requested" << std::endl;
                        running = false;
                    }
                };

                while (running) {
                    std::cout << "Working ..." << std::endl;
                    sleep(1);
                }
            }
        };

        sleep(5);

        std::stop_source source{ jt.get_stop_source() };
        
        source.request_stop();  // request stop on stop token of thread 'jt'
        jt.join();
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
