#pragma once

#include "Core/Timer.hpp"
#include "Core/Log.hpp"

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
            if (Log::GetCoreLogger())
            {
                NFS_CORE_ERROR("Profiler::BeginSession'{0}' when session '{1}' already open.", name, m_CurrentSession->name);
            }
            InternalEndSession();
        }
        m_OutputStream.open(filePath);

        if (m_OutputStream.is_open())
        {
            m_CurrentSession = new ProfilingSession({ name });
            WriteHeader();
        }
        else
        {
            if (Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
            {
                NFS_CORE_ERROR("Profiler could not open results file '{0}'.", filePath);
            }
        }
    }

    void EndSession()
    {
        std::lock_guard lock(m_Mutex);
        InternalEndSession();
    }

    void WriteProfile(const ProfileResult& result)
    {
        std::stringstream json;

        json << std::setprecision(3) << std::fixed;
        json << ",{";
        json << "\"cat\":\"function\",";
        json << "\"dur\":" << (result.elapsedTime.count()) << ',';
        json << "\"name\":\"" << result.name << "\",";
        json << "\"ph\":\"X\",";
        json << "\"pid\":0,";
        json << "\"tid\":" << result.threadID << ",";
        json << "\"ts\":" << result.start.count();
        json << "}";

        std::lock_guard lock(m_Mutex);
        if (m_CurrentSession)
        {
            m_OutputStream << json.str();
            m_OutputStream.flush();
        }
    }

    static Profiler& Get()
    {
        static Profiler instance;
        return instance;
    }

private:
    Profiler() { }

    ~Profiler() { EndSession(); }

    void WriteHeader()
    {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
        m_OutputStream.flush();
    }
    void WriteFooter()
    {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

    void InternalEndSession()
    {
        if (m_CurrentSession)
        {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
        }
    }

    std::mutex m_Mutex;
    ProfilingSession* m_CurrentSession { };
    std::ofstream m_OutputStream;
};

class ProfilerTimer
{
public:
    ProfilerTimer(const char* name)
        : m_Name(name)
    {
        m_StartTimepoint = std::chrono::steady_clock::now();
    }

    ~ProfilerTimer()
    {
        if (!m_Stopped) Stop();
    }

    void Stop()
    {
        auto endTimepoint = std::chrono::steady_clock::now();
        auto highResStart = FloatingPointMicroseconds { m_StartTimepoint.time_since_epoch() };
        auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch()
            - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

        Profiler::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

        m_Stopped = true;
    }

private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
    bool m_Stopped = false;
};

namespace ProfilerUtils
{
    template <size_t N> struct ChangeResult
    {
        char data[N];
    };

    template <size_t N, size_t K> constexpr auto CleanupOutputString(const char (&expr)[N], const char (&remove)[K])
    {
        ChangeResult<N> result = { };

        size_t srcIndex = 0;
        size_t dstIndex = 0;
        while (srcIndex < N)
        {
            size_t matchIndex = 0;
            while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
                matchIndex++;
            if (matchIndex == K - 1) srcIndex += matchIndex;
            result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
            srcIndex++;
        }
        return result;
    }
}

}

// ========================================================================
// PROFILER'S MACROS (USE THOSE INSTEAD OF CLASSES)
// ========================================================================

// CHANGE TO 0 TURN OFF PROFILER FOR THE PROJECT
#define NFS_PROFILE 1

#if NFS_PROFILE

// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define NFS_FUNC_SIG __PRETTY_FUNCTION__

#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define NFS_FUNC_SIG __PRETTY_FUNCTION__

#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define NFS_FUNC_SIG __FUNCSIG__

#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define NFS_FUNC_SIG __FUNCTION__

#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define NFS_FUNC_SIG __FUNC__

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define NFS_FUNC_SIG __func__

#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define NFS_FUNC_SIG __func__

#else
#define NFS_FUNC_SIG "NFS_FUNC_SIG unknown!"
#endif

#define NFS_PROFILE_SCOPE_LINE2(name, line)                                                                                      \
    constexpr auto fixedName##line = ::NFSEngine::ProfilerUtils::CleanupOutputString(name, "__cdecl ");                          \
    ::NFSEngine::ProfilerTimer timer##line(fixedName##line.data)

#define NFS_PROFILE_SCOPE_LINE(name, line) NFS_PROFILE_SCOPE_LINE2(name, line)

// MACROS TO USE
#define NFS_PROFILE_BEGIN_SESSION(name, filepath) ::NFSEngine::Profiler::Get().BeginSession(name, filepath)
#define NFS_PROFILE_END_SESSION() ::NFSEngine::Profiler::Get().EndSession()

// ==============================================================
// Used to profile specific fragment of code
// void foo(){
//   {
//     NFS_PROFILE_SCOPE("Doing something important")
//     DoSomethingImportant();
//   }
//   DoSomethingElse();
// }
// it will measure code only within {}
// ==============================================================
#define NFS_PROFILE_SCOPE(name) NFS_PROFILE_SCOPE_LINE(name, __LINE__)

// ==============================================================
// Used to profile entire function
// void foo(){
//   NFS_PROFILE_FUNCTION();
//   DoSomethingImportant();
// }
// it will measure entire funcion passing its name as a parameter
// ==============================================================
#define NFS_PROFILE_FUNCTION() NFS_PROFILE_SCOPE(NFS_FUNC_SIG)

#else

#define NFS_PROFILE_BEGIN_SESSION(name, filepath)
#define NFS_PROFILE_END_SESSION()
#define NFS_PROFILE_SCOPE(name)
#define NFS_PROFILE_FUNCTION()

#endif