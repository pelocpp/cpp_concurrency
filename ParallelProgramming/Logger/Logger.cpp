#pragma once
#include <chrono>
#include <mutex>
#include "Logger.h"

std::chrono::steady_clock::time_point Logger::begin;

std::mutex Logger::display_mutex;
