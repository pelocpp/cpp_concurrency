// ===========================================================================
// StrategizedLockTemplate.h
// ===========================================================================

#pragma once

#include <mutex>

namespace Concurrency_StrategizedCompileTime
{
    template <typename T>
    concept LockableConcept = requires(T object) {
        object.lock();
        object.unlock();
    };

    template <typename TLock>
        requires LockableConcept<TLock>
    class StrategizedLocking
    {
    private:
        TLock& m_lock;

    public:
        StrategizedLocking(TLock&) { m_lock.lock(); }
        ~StrategizedLocking() { m_lock.unlock(); }
    };

    struct NullObjectMutex
    {
        void lock();
        void unlock();
    };

    class NoLock
    {
    private:
        mutable NullObjectMutex m_nullMutex;

    public:
        void lock() const{ m_nullMutex.lock(); }
        void unlock() const { m_nullMutex.unlock(); }
    };

    class ExclusiveLock
    {
    private:
        mutable std::mutex m_mutex;

    public:
        void lock() const { m_mutex.lock(); }
        void unlock()const { m_mutex.unlock(); }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

