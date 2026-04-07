#include "Platforms/Desktop/WindowsWIndow.hpp"
#include "Platforms/OpenGL/OpenGLContext.hpp"
#include <memory>

#include "Events/ApplicationEvent.hpp"

namespace NFSEngine {
	
	std::unique_ptr<Window> Window::Create(const std::string& title, int width, int height) {
		return std::make_unique<WindowsWindow>(title, width, height);
	}
	
	WindowsWindow::WindowsWindow(const std::string& title, int width, int height) {
		Init(title, width, height);
	}
	
	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}
	
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

}