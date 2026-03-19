// ===========================================================================
// EventLoop.h
// ===========================================================================

#pragma once

#include <iostream>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>
#include <mutex>

#include "../Logger/Logger.h"

class EventLoop
{
private:
    using Event = std::move_only_function<void()>;

private:
    std::vector<Event>        m_events;
    std::mutex                m_mutex;
    std::condition_variable   m_condition;
    std::jthread              m_thread;
    bool                      m_running;

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
    //void enqueue(Event& callable);
    //void enqueue(Event&& callable) noexcept;
    void enqueue(Event callable);

    template<typename TFunc, typename ... TArgs>
    void enqueueTask(TFunc&& callable, TArgs&& ...args)
    {
        Logger::log(std::cout, "enqueueTask ...");

        {
            std::lock_guard<std::mutex> guard{ m_mutex };

            // Note: THIS does not behave as intended because:
            // a) You captured everything with[=], so callable and args... are now copies inside the lambda, not forwarding references anymore.
            // b) std::forward here is misleading and unnecessary.
            
            //m_events.push_back([=] () mutable {
            //    std::forward<TFunc>(callable) (std::forward<TArgs>(args) ...);
            //    }
            //);

            // Using init-capture to preserve move semantics:
            m_events.push_back(
                [func = std::forward<TFunc>(callable),
                ... capturedArgs = std::forward<TArgs>(args)]() mutable   // Hmmm, geht auch ohne mutable
                {
                    std::invoke(std::move(func), std::move(capturedArgs)...);
                }
            );

            // more simpler, but not "perfect"
            // m_events.push_back( [=] () mutable { callable (args ...); } );

            // again more simpler, not "perfect", variables of capture clause explicitely listed
            // m_events.push_back( [callable, args ... ] () mutable { callable (args ...); } );
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
