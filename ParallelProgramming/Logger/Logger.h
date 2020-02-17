#pragma once
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread> 
#include <mutex> 

class Logger {
public:
    template<typename ...Args>
    static void log(std::ostream& os, Args ...args) {
        std::stringstream ss;
        (ss << ... << args) << std::endl;
        os << ss.str();
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
};
