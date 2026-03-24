#include "Engine.hpp"
#include <iostream>

Engine::Engine() : window(nullptr) {}

Engine::~Engine() {
	Cleanup();
}

bool Engine::Init() {
	if (!glfwInit()) return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1280, 720, "Nowy Folder Studio", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return false;
	}

	if (!audioManager.Init()) return false;
	
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

	// temporary?
	activeGame = new Game();
	activeGame->Init();

	audioManager.LoopSound("PrototypeMusicProjectFile.wav", 0.05f);
	// temporary?
	
	return true;
}

void Engine::Run() {
	while (!glfwWindowShouldClose(window)) {
		ProcessInput();

		Update();

		Render();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

void Engine::ProcessInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void Engine::Update() {
	audioManager.Update();

	if (activeGame) activeGame->Update();
}

void Engine::Render() {
	glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (activeGame) activeGame->Render();
}

void Engine::Cleanup() {
	audioManager.Cleanup();

	if (activeGame) {
		delete activeGame;
		activeGame = nullptr;
	}

	if (window) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
	glfwTerminate();
}