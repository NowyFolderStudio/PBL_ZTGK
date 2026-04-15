#include "Platforms/Desktop/WindowsWIndow.hpp"
#include "Platforms/OpenGL/OpenGLContext.hpp"
#include <memory>

#include "Events/ApplicationEvent.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

namespace NFSEngine {

    std::unique_ptr<Window> Window::Create(const std::string& title, int width, int height) {
        return std::make_unique<WindowsWindow>(title, width, height);
    }

    WindowsWindow::WindowsWindow(const std::string& title, int width, int height) { Init(title, width, height); }

    WindowsWindow::~WindowsWindow() { Shutdown(); }

    void WindowsWindow::Init(const std::string& title, int width, int height) {
        if (!glfwInit()) {
            return;
        }

        m_Data.Title = title;
        m_Data.Width = width;
        m_Data.Height = height;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        m_Context = new OpenGLContext(m_Window);
        m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, false);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, true);
                data.EventCallback(event);
                break;
            }
            }
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

    void WindowsWindow::Shutdown() {
        delete m_Context;
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void WindowsWindow::OnUpdate() {
        glfwPollEvents();
        m_Context->SwapBuffers();
    }

    void WindowsWindow::SetCursorMode(CursorMode mode) {
        int glfwMode = GLFW_CURSOR_NORMAL;
        switch (mode) {
        case CursorMode::Normal:
            glfwMode = GLFW_CURSOR_NORMAL;
            break;
        case CursorMode::Hidden:
            glfwMode = GLFW_CURSOR_HIDDEN;
            break;
        case CursorMode::Locked:
            glfwMode = GLFW_CURSOR_DISABLED;
            break;
        }
        glfwSetInputMode(m_Window, GLFW_CURSOR, glfwMode);
    }

    CursorMode WindowsWindow::GetCursorMode() const {
        int mode = glfwGetInputMode(m_Window, GLFW_CURSOR);
        if (mode == GLFW_CURSOR_DISABLED) return CursorMode::Locked;
        if (mode == GLFW_CURSOR_HIDDEN) return CursorMode::Hidden;
        return CursorMode::Normal;
    }

} // namespace NFSEngine