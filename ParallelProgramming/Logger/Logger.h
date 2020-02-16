#pragma once
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread> 
#include <mutex> 

class Logger {
public:

    // TODO: Folding print function !!!
    static void log(std::ostream& os, std::string s, std::thread::id tid) {
        std::stringstream ss;
        ss << s << tid << std::endl;
        std::cout << ss.str();

    }

    static void log(std::ostream& os, std::string s) {
        std::stringstream ss;
        ss << s << std::endl;
        std::cout << ss.str();
    }

    static void log(std::ostream& os, std::string s, long l) {
        std::stringstream ss;
        ss << s << l << std::endl;
        std::cout << ss.str();
    }

    // ODER

    static void log2(std::ostream& os, std::string s, long l) {
        display_mutex.lock();
        os << s << l << std::endl;
        display_mutex.unlock();
    }

    static void startWatch() {
        begin = std::chrono::steady_clock::now();
    }

    static void stopWatchMilli() {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "Elapsed time in milliseconds = " << duration << " [milliseconds]" << std::endl;

        // restart watch
        begin = std::chrono::steady_clock::now();
    }

    static void stopWatchMicro() {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        std::cout << "Elapsed time in milliseconds = " << duration << " [microseconds]" << std::endl;
    }

private:
    static std::chrono::steady_clock::time_point begin;
    static std::mutex display_mutex;
};
