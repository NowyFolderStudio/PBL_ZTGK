#pragma once

#include "Core/PlatformDetection.hpp"

#include <memory>

#ifdef NFS_DEBUG
#if defined(NFS_PLATFORM_WINDOWS)
#define NFS_DEBUGBREAK() __debugbreak()
#elif defined(NFS_PLATFORM_LINUX)
#include <csignal>
#define NFS_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define NFS_ENABLE_ASSERTS
#else
#define NFS_DEBUGBREAK()
#endif

#define NFS_EXPAND_MACRO(x) x
#define NFS_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define NFS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Core/Log.hpp"
#include "Core/Assert.hpp"