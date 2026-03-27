#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>	
#include <memory>
#include "Window.hpp"
#include "AudioManager.hpp"
#include "Game.hpp"
#include "Input.hpp"




class Engine {
public:
	Engine();
	~Engine();

	bool Init();
	void Run();
	void Cleanup();

	inline Window& GetWindow() { return *m_Window; }
	inline static Engine& Get() { return *s_Instance; }

private:
	static Engine* s_Instance;
	bool m_Running = true;

	std::unique_ptr<Window> m_Window;

	AudioManager m_AudioManager;
	Game* m_ActiveGame = nullptr;

	void ProcessInput();
	void Update();
	void Render();
};
