#include "Core/Application.hpp"
#include "Core/DeltaTime.hpp"
#include "Debug/Profiler.hpp"
#include "Platforms/WindowsInput.hpp"
#include "Platforms/Window.hpp"
#include "Core/Log.hpp"
#include <memory>
#include <GLFW/glfw3.h>
#include "Renderer/Renderer.hpp"

namespace NFSEngine
{

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationConfig& config)
    : m_Config(config)
{
    NFS_PROFILE_FUNCTION();
    s_Instance = this;

    std::string title = config.WindowTitle;
    int width = config.WindowWidth;
    int height = config.WindowHeight;

    m_Window = Window::Create(title, width, height);
    static WindowsInput windowsInput;
    Input::instance = &windowsInput;

    m_Window->SetEventCallback([this](Event& e) { this->OnEvent(e); });

    Renderer::Init();
}

Application::~Application() { NFS_PROFILE_FUNCTION(); }

void Application::PushLayer(Layer* layer)
{
    NFS_PROFILE_FUNCTION();
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    NFS_PROFILE_FUNCTION();
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Close() { m_Running = false; }

void Application::Run()
{
    NFS_PROFILE_FUNCTION();
    NFS_CORE_INFO("Application started.");

    while (m_Running && !m_Window->ShouldClose())
    {
        auto time = static_cast<float>(glfwGetTime());
        DeltaTime deltaTime = time - m_LastFrameTime;
        m_LastFrameTime = time;

        Input::UpdateStates();

        for (Layer* layer : m_LayerStack)
        {
            layer->OnUpdate(deltaTime);
        }

        m_Window->OnUpdate();
    }
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) { return this->OnWindowResize(event); });
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) { return this->OnWindowClose(event); });
    dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) { return this->OnKeyPressed(event); });

    if (!e.Handled)
    {

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {

            (*it)->OnEvent(e);
            if (e.Handled)
            {
                break;
            }
        }
    }
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    NFS_PROFILE_FUNCTION();
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        m_Minimalized = true;
        return false;
    }
    m_Minimalized = false;

    m_Config.WindowHeight = e.GetHeight();
    m_Config.WindowWidth = e.GetWidth();

    glViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false; // false poniewaz event bo uzyciu nie moze zniknac tylko musi zostacc wykorzystany wiele razy - tyle
                  // ile warstw go potrzebuje
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    this->Close();
    return true;
}

bool Application::OnKeyPressed(KeyPressedEvent& e)
{
    if (e.GetKeyCode() == Key::Escape)
    {
        this->Close();
        return true;
    }
    return false;
}
}