#pragma once

#include "Core/Timer.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

namespace NFSEngine
{

using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

struct ProfileResult
{
    std::string name;
    FloatingPointMicroseconds start;
    std::chrono::microseconds elapsedTime;
    std::thread::id threadID;
};

struct ProfilingSession
{
    std::string name;
};

class Profiler
{
public:
    Profiler(const Profiler&) = delete;
    Profiler(Profiler&&) = delete;

    void BeginSession(const std::string& name, const std::string& filePath = "profiling_result.json")
    {
        std::lock_guard lock(m_Mutex);
        if (m_CurrentSession)
        {
        }
    }

private:
    std::mutex m_Mutex;
    ProfilingSession* m_CurrentSession;
    std::ofstream m_OutputStream;
};

}