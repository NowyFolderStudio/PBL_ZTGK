#pragma once

#ifdef _WIN32
#ifdef _WIN64
#define NFS_PLATFORM_WINDOWS
#else
#error "x86 Builds are not supported!"
#endif
#elif defined(__linux__)
#define NFS_PLATFORM_LINUX
#else
#error "Unknown platform!"
#endif