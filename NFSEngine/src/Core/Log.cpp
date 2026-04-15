#include "Core/Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace NFSEngine {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        // Setting format pattern
        // %^...%$ - color text
        // %T - time (HH:MM:SS)
        // %n - logger name (ENGINE lub APP)
        // %v - message
        spdlog::set_pattern("%^[%T] %n: %v%$");

        // Logger for the NFSEngine
        s_CoreLogger = spdlog::stdout_color_mt("ENGINE");
        s_CoreLogger->set_level(spdlog::level::trace);

        // Logger for the Game
        s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace);
    }

} // namespace NFSEngine