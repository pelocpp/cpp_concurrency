// ===========================================================================
// Logger.h
// ===========================================================================

#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <syncstream>
#include <thread> 

class Logger {
public:
    static void enableLogging(bool enable)
    {
        s_loggingEnabled = enable;
    }

    static bool isLoggingEnabled()
    {
        return s_loggingEnabled;
    }

    template<typename ... TArgs>
    static void logInternal(std::ostream& os, TArgs&& ...args)
    {
        std::osyncstream syncStream{ os };
        ((syncStream << getPrefix() << '\t') << ... << std::forward<TArgs>(args)) << '\n';
    }

    // log conditionally
    template<typename ... TArgs>
    static void log(std::ostream& os, TArgs&& ...args)
    {
        if (!s_loggingEnabled)
            return;

        logInternal(os, std::forward<TArgs>(args)...);
    }

    // log unconditionally
    template<typename ... TArgs>
    static void logAbs(std::ostream& os, TArgs&& ...args)
    {
        logInternal(os, std::forward<TArgs>(args)...);
    }

    static size_t readableTID(const std::thread::id id)
    {
        std::lock_guard<std::mutex> guard{ s_mutexIds };
        if (s_mapIds.find(id) == s_mapIds.end()) {
            s_nextIndex++;
            s_mapIds[id] = s_nextIndex;
        }

        return s_mapIds[id];
    }

    static void startWatch() {
        s_begin = std::chrono::steady_clock::now();
    }

    static void stopWatchMilli(std::ostream& os) {
        std::chrono::steady_clock::time_point end{ std::chrono::steady_clock::now() };
        auto duration{ std::chrono::duration_cast<std::chrono::milliseconds>(end - s_begin).count() };
        os << getPrefix() << '\t' << "Elapsed time: " << duration << " [milliseconds]" << std::endl;
    }

    static void stopWatchMicro(std::ostream& os) {
        std::chrono::steady_clock::time_point end{ std::chrono::steady_clock::now() };
        auto duration{ std::chrono::duration_cast<std::chrono::microseconds>(end - s_begin).count() };
        os << getPrefix() << '\t' << "Elapsed time: " << duration << " [microseconds]" << std::endl;
    }

private:
    static std::string getPrefix()
    {
        std::stringstream ss;
        std::thread::id currentThreadId{ std::this_thread::get_id() };
        size_t tid{ readableTID(currentThreadId) };
        std::string prefix{ "[" + std::to_string(tid) + "]: " };
        ss << prefix;
        return ss.str();
    }

    static std::chrono::steady_clock::time_point s_begin;
    static bool s_loggingEnabled;
    static std::mutex s_mutexIds;
    static std::map<std::thread::id, std::size_t> s_mapIds;
    static std::size_t s_nextIndex;
};

// ===========================================================================
// End-of-File
// ===========================================================================
