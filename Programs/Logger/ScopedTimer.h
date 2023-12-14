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
    auto operator=(const ScopedTimer&)->ScopedTimer& = delete;

    ScopedTimer(ScopedTimer&&) = delete;
    auto operator=(ScopedTimer&&)->ScopedTimer& = delete;
};

// ===========================================================================
// End-of-File
// ===========================================================================
