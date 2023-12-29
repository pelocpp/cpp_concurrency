// ===========================================================================
// 21_LockingMechanisms
// ===========================================================================

#pragma once

//#include <exception>
//#include <stack>
//#include <mutex>
//#include <optional>


// SEHR GUTES .PDF

// https://filebox.ece.vt.edu/~ECE3574/meeting/23-thread-safe-queue/slides.pdf

// https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/



// Die Sache mit dem scoped_lock extra besprechen ............

// https://medium.com/@simontoth/daily-bit-e-of-c-std-scoped-lock-9cab4142f9d4



#include <mutex>
#include <condition_variable>
#include <queue>
// #include <memory>


namespace Concurrency_ThreadsafeQueue
{
    template<typename T>
    class ThreadsafeQueue
    {
    private:
        std::queue<T>            m_data;
        mutable std::mutex       m_mutex;
        std::condition_variable  m_condition;

    public:
        ThreadsafeQueue() {}

        // copy and move constructor may be useful
        ThreadsafeQueue(ThreadsafeQueue const& other)
        {
            std::lock_guard<std::mutex> lock{ other.m_mutex };
            m_data = other.m_data;
        }

        ThreadsafeQueue(ThreadsafeQueue&& other) noexcept
        {
            std::lock_guard<std::mutex> lock{ other.m_mutex };
            m_data = std::move(other.m_data);
        }

        // TODO (( WEITER: Da müssen tatsächlich 2 Locks aufgezogen werden !!!!!!!!!!!!
        // Wie macht man das ????????????????????
        ThreadsafeQueue& operator= (const ThreadsafeQueue& other)
        {
            if (&other == this)
                return *this;

            std::scoped_lock<std::mutex> lock{ m_mutex, other.m_mutex };

            //std::unique_lock lock1(m_mutex, std::defer_lock);
            //std::unique_lock lock2(other.m_mutex, std::defer_lock);
            //std::lock(lock1, lock2);

            m_data = other.m_data;
            return *this;
        }

        ThreadsafeQueue& operator= (ThreadsafeQueue&& other) noexcept
        {
            if (&other == this)
                return *this;

            std::scoped_lock<std::mutex> lock{ m_mutex, other.m_mutex };

            m_data = std::move (other.m_data);
            return *this;
        }

        // check !!!
        void push(const T& value)
        {
            //std::lock_guard<std::mutex> lk(m_mutex);
            //m_data.push(new_value);
            //m_condition.notify_one();

            std::unique_lock<std::mutex> lock{ m_mutex };
            m_data.push(value);
            lock.unlock();
            m_condition.notify_one();
        }

        void push(T&& value)
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_data.push(std::move(value));
            lock.unlock();
            m_condition.notify_one();
        }


        // check !!!
        void wait_and_pop(T& value)
        {
            std::lock_guard<std::mutex> lock{ m_mutex };;
            m_condition.wait(lock, [this] () {
                return !m_data.empty(); 
                }
            );

            value = m_data.front();
            m_data.pop();
        }

        // check !!!
        bool try_pop(T& value)
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            if (m_data.empty()) {
                return false;
            }

            value = m_data.front();
            m_data.pop();
            return true;
        }

        // check !!!
        bool empty() const
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            return m_data.empty();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            return m_data.size();
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
