#pragma once

#include "Core/Application.hpp"
#include <filesystem>
#include <iostream>

extern NFSEngine::Application* NFSEngine::CreateApplication();

int main(int argc, char** argv)
{
    // Setting current working directory to be the same as executable file
    try {
        // argv[0] is a path to executable file
        std::filesystem::path executablePath = argv[0];
        std::filesystem::path executableDirectory = executablePath.parent_path();
        std::filesystem::current_path(executableDirectory);
    } 
    catch (const std::exception& e) {
        std::cerr << "ENTRYPOINT::ERROR::CURRENT WORKING DIRECTORY PATH IS NOT SET" << std::endl;
    }

    auto app = NFSEngine::CreateApplication();

    app->Run();

    delete app;
}