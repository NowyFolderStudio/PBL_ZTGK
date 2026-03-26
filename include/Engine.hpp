#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AudioManager.hpp"
#include "Shader.hpp"
#include "Game.hpp"
#include "Input.hpp"

class Engine {
public:
	Engine();
	~Engine();

	bool Init();
	void Run();
	void Cleanup();

	inline static Engine& Get() {return *instance;}
	inline GLFWwindow* GetNativeWindow() const {return window;}

private:
	static Engine* instance;

	GLFWwindow* window;
	AudioManager audioManager;
	Game* activeGame = nullptr;

	void ProcessInput();
	void Update();
	void Render();
};
