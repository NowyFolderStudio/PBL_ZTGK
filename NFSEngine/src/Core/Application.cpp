#include "Core/Application.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/InputAction.hpp"
#include "Debug/Profiler.hpp"
#include "Platforms/WindowsInput.hpp"
#include "Platforms/Window.hpp"
#include "Core/Log.hpp"
#include <memory>
#include <GLFW/glfw3.h>
#include "Renderer/Renderer.hpp"
#include "Debug/DebugCamera.hpp"

namespace NFSEngine {

    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationConfig& config)
        : m_Config(config) {
        NFS_PROFILE_FUNCTION();
        s_Instance = this;

        std::string title = config.WindowTitle;
        int width = static_cast<int>(config.WindowWidth);
        int height = static_cast<int>(config.WindowHeight);

        m_Window = Window::Create(title, width, height);
        static WindowsInput windowsInput;
        Input::instance = &windowsInput;

        m_Window->SetEventCallback([this](Event& e) { this->OnEvent(e); });

        Renderer::Init();

        DebugCamera::Init(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

        m_ImGuiLayer = new ImGuiLayer();
        PushLayer(m_ImGuiLayer);
    }

    Application::~Application() { NFS_PROFILE_FUNCTION(); }

    void Application::PushLayer(Layer* layer) {
        NFS_PROFILE_FUNCTION();
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer) {
        NFS_PROFILE_FUNCTION();
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer* layer) {
        NFS_PROFILE_FUNCTION();
        m_LayerStack.PopLayer(layer);
        layer->OnDetach();
    }

    void Application::PopOverlay(Layer* layer) {
        NFS_PROFILE_FUNCTION();
        m_LayerStack.PopOverlay(layer);
        layer->OnDetach();
    }

    void Application::Close() { m_Running = false; }

    void Application::Run() {
        NFS_PROFILE_FUNCTION();
        NFS_CORE_INFO("Application started.");

        while (m_Running && !m_Window->ShouldClose()) {
            NFS_PROFILE_SCOPE("RunLoop");

            auto time = static_cast<float>(glfwGetTime());
            DeltaTime deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            Input::UpdateStates();
            InputActionManager::Get().UpdateAll();

            {
                NFS_PROFILE_SCOPE("LayerStack Logic Update");

                for (size_t i = 0; i < m_LayerStack.size(); ++i) {
                    m_LayerStack[i]->OnUpdate(deltaTime);
                }
            }

            DebugCamera::OnUpdate(deltaTime);

            {
                NFS_PROFILE_SCOPE("LayerStack Rendering Preparation");
                for (size_t i = 0; i < m_LayerStack.size(); ++i) {
                    m_LayerStack[i]->OnRender();
                }
            }

            m_ImGuiLayer->Begin();
            {
                NFS_PROFILE_SCOPE("LayerStack OnImGuiRender");

                for (size_t i = 0; i < m_LayerStack.size(); ++i) {
                    m_LayerStack[i]->OnImGuiRender();
                }
            }
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) { return this->OnWindowResize(event); });
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) { return this->OnWindowClose(event); });
        dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) { return this->OnKeyPressed(event); });

        if (!e.Handled) {
            for (int i = static_cast<int>(m_LayerStack.size()) - 1; i >= 0; --i) {
                m_LayerStack[i]->OnEvent(e);

                if (e.Handled) {
                    break;
                }
            }
        }

        if (!e.Handled) {
            DebugCamera::OnEvent(e);
        }
    }

    bool Application::OnWindowResize(WindowResizeEvent& e) {
        NFS_PROFILE_FUNCTION();
        if (e.GetWidth() == 0 || e.GetHeight() == 0) {
            m_Minimalized = true;
            return false;
        }
        m_Minimalized = false;

        m_Config.WindowHeight = e.GetHeight();
        m_Config.WindowWidth = e.GetWidth();

        Renderer::GetAPI().SetViewport(0, 0, e.GetWidth(), e.GetHeight());
        return false;
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        this->Close();
        return true;
    }

    bool Application::OnKeyPressed(KeyPressedEvent& e) {
        if (e.GetKeyCode() == Key::Escape) {
            this->Close();
            return true;
        }
        return false;
    }
} // namespace NFSEngine