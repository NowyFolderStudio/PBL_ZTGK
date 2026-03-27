#pragma once

#include "Core/LayerStack.hpp"

#include <string>
#include <stdint.h>

namespace NFSEngine {

    struct ApplicationConfig {

        std::string WindowTitle = "NFS Engine Game";
        uint32_t WindowWidth = 1920;
        uint32_t WindowHeight = 1080;
        bool VSync = true;
    };

    class Application {
    public:
        Application(const ApplicationConfig& config);
        virtual ~Application();

        
        void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
        
        void Close();
        void Run();

    private:
        ApplicationConfig m_Config;
        LayerStack m_LayerStack;
        bool m_Running = true;
        bool m_Minimalized = false;

        // Add std::unique_ptr<Window> after implementing Window class

        static Application* s_Instance;
    };

}