#pragma once

#include <chrono>

namespace NFSEngine {

    class Timer {
    public:
        Timer() { Reset(); }

        void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }

        float Elapsed() {
            auto rawTime = std::chrono::high_resolution_clock::now() - m_Start;
            auto nanoTime = std::chrono::duration_cast<std::chrono::nanoseconds>(rawTime);
            return static_cast<float>(nanoTime.count()) * 0.001f * 0.001f * 0.001f;
        }

        float ElapsedMillis() { return Elapsed() * 1000.0f; }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };

} // namespace NFSEngine