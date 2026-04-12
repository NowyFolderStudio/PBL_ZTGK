#pragma once

#include "Core/Application.hpp"
#include "Core/Log.hpp"
#include "Debug/Profiler.hpp"
#include <filesystem>

extern NFSEngine::Application* NFSEngine::CreateApplication();

int main(int argc, char** argv)
{
    NFSEngine::Log::Init();

    // Setting current working directory to be the same as executable file
    try
    {
        // argv[0] is a path to executable file
        std::filesystem::path executablePath = std::filesystem::absolute(argv[0]);
        std::filesystem::path executableDirectory = executablePath.parent_path();
        std::filesystem::current_path(executableDirectory);
    }
    catch (const std::exception& e)
    {
        NFS_CORE_ERROR("ENTRYPOINT::CURRENT WORKING DIRECTORY PATH IS NOT SET::{}", e.what());
    }

    NFS_PROFILE_BEGIN_SESSION("Startup", "NFSEngine_Startup.json");
    auto app = NFSEngine::CreateApplication();
    NFS_PROFILE_END_SESSION();

    NFS_PROFILE_BEGIN_SESSION("Runtime", "NFSEngine_Runtime.json");
    app->Run();
    NFS_PROFILE_END_SESSION();

    NFS_PROFILE_BEGIN_SESSION("Shutdown", "NFSEngine_Shutdown.json");
    delete app;
    NFS_PROFILE_END_SESSION();
}