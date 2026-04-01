// ===========================================================================
// EventLoop.h
// ===========================================================================

#pragma once

#include "../Logger/Logger.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

class EventLoop
{
private:
    using Event = std::move_only_function<void()>;

private:
    std::vector<Event>       m_events;
    std::mutex               m_mutex;
    std::condition_variable  m_condition;
    std::jthread             m_thread;
    bool                     m_running;

public:
    // c'tor(s) / d'tor
    EventLoop();
    ~EventLoop();

    // prevent copy semantics
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator= (const EventLoop&) = delete;

    // prevent move semantics
    EventLoop(EventLoop&&) noexcept = delete;
    EventLoop& operator= (EventLoop&&) noexcept = delete;

    // public interface
    void enqueue(Event callable);

    template <typename TFunc>
    void enqueue(TFunc&& func) {
        m_events.emplace_back(std::forward<TFunc>(func));
    }

    template<typename TFunc, typename ... TArgs>
    void enqueueTask(TFunc&& func, TArgs&& ...args)
    {
        Logger::log(std::cout, "enqueueTask ...");

        // using "Generalized Lambda Capture" to preserve move semantics
        auto callable {
            [func = std::forward<TFunc>(func),
            ... capturedArgs = std::forward<TArgs>(args)] () -> void {
                std::invoke(std::move(func), std::move(capturedArgs) ...);
            } 
        };

        {
            // RAII guard
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_events.push_back(std::move(callable));
        }

        m_condition.notify_one();
    }

    void start();
    void stop();

private:
    void threadProcedure();
};

// ===========================================================================
// End-of-File
// ===========================================================================
