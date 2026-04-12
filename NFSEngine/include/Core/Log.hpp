#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace NFSEngine
{

class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

}

// ====================================================================
// EXAMPLE OF THE LOG
//
// NFS_CORE_INFO("Kitten said \"{}\" {} times. Is it happy: {}", "Meow", 5, true);
// Result:
// [03:30:14] ENGINE: Kitten said "Meow" 5 times. Is it happy: true
// ====================================================================

// ====================================================================
// MACROS ONLY FOR THE ENGINE (USE ONLY IN THE NFSENGINE PROJECT)
// ====================================================================
#define NFS_CORE_TRACE(...) ::NFSEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NFS_CORE_INFO(...) ::NFSEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NFS_CORE_WARN(...) ::NFSEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NFS_CORE_ERROR(...) ::NFSEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NFS_CORE_CRITICAL(...) ::NFSEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// ====================================================================
// MACROS ONLY FOR THE GAME (USE ONLY IN THE GAME PROJECT)
// ====================================================================
#define NFS_TRACE(...) ::NFSEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NFS_INFO(...) ::NFSEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define NFS_WARN(...) ::NFSEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NFS_ERROR(...) ::NFSEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define NFS_CRITICAL(...) ::NFSEngine::Log::GetClientLogger()->critical(__VA_ARGS__)