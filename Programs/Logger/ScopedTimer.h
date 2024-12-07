// ===========================================================================
// ScopedTimer.h
// ===========================================================================

#pragma once

#include <iostream>

#include "Logger.h"

class ScopedTimer {
public:
    ScopedTimer() {
        Logger::startWatch();
    }

    ~ScopedTimer() {
        Logger::stopWatchMilli(std::cout);
    }

    // no copying or moving
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

    ScopedTimer(ScopedTimer&&) = delete;
    ScopedTimer& operator=(ScopedTimer&&) = delete;
};

// ===========================================================================
// End-of-File
// ===========================================================================
