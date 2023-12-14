// ===========================================================================
// StrategizedLock.cpp
// ===========================================================================

#include <iostream>

#include "StrategizedLock.h"

namespace Concurrency_StrategizedLock
{
    // class StrategizedLocking
    StrategizedLocking::StrategizedLocking(const ILock& lock) : m_lock(lock) {
        m_lock.lock();
    }

    StrategizedLocking::~StrategizedLocking() {
        m_lock.unlock();
    }

    // class NullObjectMutex
    void NullObjectMutex::lock() {}
    void NullObjectMutex::unlock() {}

    // class NoLock
    void NoLock::lock() const {
        m_nullMutex.lock();
    }

    void NoLock::unlock() const {
        m_nullMutex.unlock();
    }

    // class ExclusiveLock
    void ExclusiveLock::lock() const {
        m_mutex.lock();
    }

    void ExclusiveLock::unlock() const {
        m_mutex.unlock();
    }

    // class RecursiveLock
    void RecursiveLock::lock() const {
        m_recursive_mutex.lock();
    }

    void RecursiveLock::unlock() const {
        m_recursive_mutex.unlock();
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
