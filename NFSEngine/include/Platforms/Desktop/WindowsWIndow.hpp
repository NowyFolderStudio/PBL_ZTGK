#pragma once
#include <glad/glad.h>
#include "Platforms/Window.hpp"
#include "Renderer/GraphicsContext.hpp"
#include <GLFW/glfw3.h>

namespace NFSEngine
{

class WindowsWindow : public Window
{
public:
    WindowsWindow(const std::string& title, int width, int height);
    virtual ~WindowsWindow();

    void OnUpdate() override;
    bool ShouldClose() const override { return glfwWindowShouldClose(m_Window); }
    void* GetNativeWindow() const override { return m_Window; }
    uint32_t GetWidth() const override { return m_Data.Width; }
    uint32_t GetHeight() const override { return m_Data.Height; }

    inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

private:
    void Init(const std::string& title, int width, int height);
    void Shutdown();

    GLFWwindow* m_Window;
    GraphicContext* m_Context;

    struct WindowData
    {
        std::string Title;
        int Width, Height;
        EventCallbackFn EventCallback;
    };

    WindowData m_Data;
};

}