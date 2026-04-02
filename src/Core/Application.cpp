#include "Core/Application.hpp"
#include <memory>

namespace NFSEngine {

    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationConfig& config) : m_Config(config) {
		s_Instance = this;

        std::string title = config.WindowTitle;
        int width = config.WindowWidth;
        int height = config.WindowHeight;

        m_Window = Window::Create(title, width, height);
	}

    Application::~Application() {
        
	}

    void Application::PushLayer(Layer* layer) {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer) {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
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