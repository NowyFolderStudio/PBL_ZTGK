#pragma once

#include "Core/Application.hpp"
#include "Core/Log.hpp"
#include "Debug/Profiler.hpp"
#include <filesystem>

extern NFSEngine::Application* NFSEngine::CreateApplication();

int main(int argc, char** argv) {
    NFSEngine::Log::Init();

    // Setting current working directory to be the same as executable file
    try {
        // argv[0] is a path to executable file
        std::filesystem::path executablePath = std::filesystem::absolute(argv[0]);
        std::filesystem::path executableDirectory = executablePath.parent_path();
        std::filesystem::current_path(executableDirectory);
    } catch (const std::exception& e) {
        NFS_CORE_ERROR("ENTRYPOINT::CURRENT WORKING DIRECTORY PATH IS NOT SET::{}", e.what());
    }

    NFSEngine::ProfilingSession session;
    session.name = "Startup";
    session.filePath = "NFSEngine_Startup.json";

    NFS_PROFILE_BEGIN_SESSION(session);
    auto app = NFSEngine::CreateApplication();
    NFS_PROFILE_END_SESSION();

    session.name = "Runtime";
    session.filePath = "NFSEngine_Runtime.json";
    NFS_PROFILE_BEGIN_SESSION(session);
    app->Run();
    NFS_PROFILE_END_SESSION();

    session.name = "Shutdown";
    session.filePath = "NFSEngine_Shutdown.json";
    NFS_PROFILE_BEGIN_SESSION(session);
    delete app;
    NFS_PROFILE_END_SESSION();
}