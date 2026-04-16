#pragma once

#include "Core/PlatformDetection.hpp"

#ifdef NFS_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#ifdef __cplusplus

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>

#include "Core/Base.hpp"
#include "Core/Log.hpp"
#include "Debug/Profiler.hpp"

#endif // __cplusplus