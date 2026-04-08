#include "Core/Application.hpp"
#include "Core/DeltaTime.hpp"
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
        static WindowsInput windowsInput;
        Input::instance = &windowsInput;
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

            float time = glfwGetTime();
            DeltaTime deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            Input::UpdateStates();

            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate(deltaTime);
            }

            if (Input::IsKeyPressed(Key::Escape)) {
                m_Running = false;
            }

            m_Window->OnUpdate();
        }
    }

}