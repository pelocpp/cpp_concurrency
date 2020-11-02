// ===========================================================================
// Logger
// ===========================================================================

#pragma once
#include <chrono>
#include <mutex>

#include "Logger.h"

std::chrono::steady_clock::time_point Logger::s_begin;

bool Logger::s_loggingEnabled = false;
// std::ostream& Logger::m_os = std::cout;
// bool Logger::m_logActive = false;
// std::mutex Logger::m_mutexOutput;

std::size_t Logger::s_nextIndex = 0;
std::mutex Logger::s_mutexIds;
std::map<std::thread::id, std::size_t> Logger::s_mapIds;

// ===========================================================================
// End-of-File
// ===========================================================================
