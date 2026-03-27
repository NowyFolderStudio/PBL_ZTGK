#pragma once
#include "Renderer/GraphicsContext.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenGLContext : public GraphicContext {
public:
	OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) {}

	virtual void Init() override {
		glfwMakeContextCurrent(m_WindowHandle);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	}

	virtual void SwapBuffers() override {
		glfwSwapBuffers(m_WindowHandle);
	}

private:
	GLFWwindow* m_WindowHandle;
};