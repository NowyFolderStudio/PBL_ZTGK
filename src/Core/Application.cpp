#include "Core/Application.hpp"

namespace NFSEngine {

    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationConfig& config) : m_Config(config) {
		s_Instance = this;
	}

    Application::~Application()
	{
        
	}

    void Application::Close() {
        m_Running = false;
    }

    void Application::Run() {
        while (m_Running) {
            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate();
            }
        }
    }

}