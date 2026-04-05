#include "Core/Application.hpp"
#include "Platforms/WindowsInput.hpp"
#include "Platforms/Window.hpp"
#include <memory>
#include <GLFW/glfw3.h>

namespace NFSEngine {

    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationConfig& config) : m_Config(config) {
		s_Instance = this;

        std::string title = config.WindowTitle;
        int width = config.WindowWidth;
        int height = config.WindowHeight;

        m_Window = Window::Create(title, width, height);
        Input::instance = new WindowsInput();
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
        while (m_Running && !m_Window->ShouldClose()) {
            Input::instance->UpdateStatesImpl();

            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate();
            }

            if (Input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
                m_Running = false;
            }

            m_Window->OnUpdate();
        }
    }

}