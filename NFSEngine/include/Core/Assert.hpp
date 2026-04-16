#pragma once

#include "Core/Base.hpp"
#include "Core/Log.hpp"
#include <filesystem>

#ifdef NFS_ENABLE_ASSERTS

#define NFS_INTERNAL_ASSERT_IMPL(type, check, msg, ...)                                                                          \
    {                                                                                                                            \
        if (!(check)) {                                                                                                          \
            NFS##type##ERROR(msg, __VA_ARGS__);                                                                                  \
            NFS_DEBUGBREAK();                                                                                                    \
        }                                                                                                                        \
    }
#define NFS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) NFS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define NFS_INTERNAL_ASSERT_NO_MSG(type, check)                                                                                  \
    NFS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", NFS_STRINGIFY_MACRO(check),                       \
                             std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define NFS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define NFS_INTERNAL_ASSERT_GET_MACRO(...)                                                                                       \
    NFS_EXPAND_MACRO(NFS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, NFS_INTERNAL_ASSERT_WITH_MSG, NFS_INTERNAL_ASSERT_NO_MSG))

#define NFS_ASSERT(...) NFS_EXPAND_MACRO(NFS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define NFS_CORE_ASSERT(...) NFS_EXPAND_MACRO(NFS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define NFS_ASSERT(...)
#define NFS_CORE_ASSERT(...)
#endif