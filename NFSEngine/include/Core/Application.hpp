#pragma once

#include "Core/LayerStack.hpp"
#include "Platforms/Window.hpp"
#include "Events/ApplicationEvent.hpp"
#include "Events/KeyEvent.hpp"

#include <string>
#include <memory>
#include <stdint.h>

int main(int argc, char** argv);

namespace NFSEngine
{

struct ApplicationConfig
{

    std::string WindowTitle = "NFS Engine Game";
    uint32_t WindowWidth = 1920;
    uint32_t WindowHeight = 1080;
    bool VSync = true;
};

class Application
{
public:
    Application(const ApplicationConfig& config);
    virtual ~Application();
    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    void Close();

    void OnEvent(Event& e);

    static Application& Get() { return *s_Instance; };
    Window& GetWindow() { return *m_Window; };
    const ApplicationConfig& GetConfig() const { return m_Config; };

private:
    void Run();
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
    bool OnKeyPressed(KeyPressedEvent& e);

private:
    ApplicationConfig m_Config;
    LayerStack m_LayerStack;
    bool m_Running = true;
    bool m_Minimalized = false;
    std::unique_ptr<Window> m_Window = nullptr;
    float m_LastFrameTime = 0.0f;

private:
    static Application* s_Instance;
    friend int ::main(int argc, char** argv);
};

Application* CreateApplication();

}