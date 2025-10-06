// ===========================================================================
// StrategizedLock.h
// ===========================================================================

#pragma once

#include "ILock.h"

#include <mutex>

namespace Concurrency_StrategizedLock
{
    class StrategizedLocking 
    {
    private:
        const ILock& m_lock;

    public:
        StrategizedLocking(const ILock& lock) : m_lock{ lock } { m_lock.lock(); }
        ~StrategizedLocking() { m_lock.unlock(); };
    };

    class NoLock : public ILock
    {
    public:
        void lock() const override {};
        void unlock() const override {};
    };

    class ExclusiveLock : public ILock
    {
    private:
        mutable std::mutex m_mutex;

    public:
        void lock() const override { m_mutex.lock(); };
        void unlock() const override { m_mutex.unlock(); };
    };

    class RecursiveLock : public ILock
    {
    private:
        mutable std::recursive_mutex m_recursive_mutex;

    public:
        void lock() const override { m_recursive_mutex.lock(); };
        void unlock() const override { m_recursive_mutex.unlock(); };
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

