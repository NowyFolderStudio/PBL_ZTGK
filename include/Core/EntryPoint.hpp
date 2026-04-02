#pragma once

#include "Core/Application.hpp"


extern NFSEngine::Application* NFSEngine::CreateApplication();

int main(int argc, char** argv)
{
    auto app = NFSEngine::CreateApplication();

    app->Run();

    delete app;
}