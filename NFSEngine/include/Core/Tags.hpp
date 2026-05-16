#pragma once

#include <cstdint>

namespace NFSEngine {

    namespace Tags {
        inline constexpr uint32_t Untagged = 0;
        inline constexpr uint32_t Player = 1 << 0;
        inline constexpr uint32_t Enemy = 1 << 1;
        inline constexpr uint32_t WallJumpSurface = 1 << 2;
        inline constexpr uint32_t DeadlyHazard = 1 << 3;
        inline constexpr uint32_t BouncySurface = 1 << 4;
        inline constexpr uint32_t LivesManager = 1 << 5;
        inline constexpr uint32_t ScoreManager = 1 << 6;
        inline constexpr uint32_t Coin = 1 << 7;
    } // namespace Tags

} // namespace NFSEngine
