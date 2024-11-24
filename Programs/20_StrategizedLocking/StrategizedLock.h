// ===========================================================================
// StrategizedLock.h
// ===========================================================================

#pragma once

#include <mutex>

#include "ILock.h"

namespace Concurrency_StrategizedLock
{
    class StrategizedLocking 
    {
    private:
        const ILock& m_lock;

    public:
        StrategizedLocking(const ILock&);
        ~StrategizedLocking();
    };

    struct NullObjectMutex
    {
        void lock();
        void unlock();
    };

    class NoLock : public ILock
    {
    private:
        mutable NullObjectMutex m_nullMutex;

    public:
        void lock() const override;
        void unlock() const override;
    };

    class ExclusiveLock : public ILock
    {
    private:
        mutable std::mutex m_mutex;

    public:
        void lock() const override;
        void unlock() const override;
    };

    class RecursiveLock : public ILock
    {
    private:
        mutable std::recursive_mutex m_recursive_mutex;

    public:
        void lock() const override;
        void unlock() const override;
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

