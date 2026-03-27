#pragma once
#include <glad/glad.h>
#include "Window.hpp"
#include "Renderer/GraphicsContext.hpp"
#include <GLFW/glfw3.h>

class WindowsWindow : public Window {
public:
	WindowsWindow(const std::string& title, int width, int height);
	virtual ~WindowsWindow();

	void OnUpdate() override;
	bool ShouldClose() const override { return glfwWindowShouldClose(m_Window); }
	void* GetNativeWindow() const override { return m_Window; }

private:
	void Init(const std::string& title, int width, int height);
	void Shutdown();

private:
	GLFWwindow* m_Window;
	GraphicContext* m_Context;
};