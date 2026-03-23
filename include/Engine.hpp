#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AudioManager.hpp"
#include "Shader.hpp"

class Engine {
public:
	Engine();
	~Engine();

	bool Init();
	void Run();
	void Cleanup();

private:
	GLFWwindow* window;
	AudioManager audioManager;

	void ProcessInput();
	void Update();
	void Render();
};