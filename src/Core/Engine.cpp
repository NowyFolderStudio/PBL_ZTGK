#include "Core/Engine.hpp"
#include "Platforms/WindowsInput.hpp"
#include "Platforms/Window.hpp"
#include <iostream>

namespace NFSEngine {

	Engine* Engine::s_Instance = nullptr;

	Engine::Engine() {
		s_Instance = this;
	}

	Engine::~Engine() {
		Cleanup();
	}

	bool Engine::Init() {
		m_Window = std::unique_ptr<Window>(Window::Create("Nowy Folder Studio", 1280, 720));
		if (!m_Window) return false;

		Input::instance = new WindowsInput();

		if (!m_AudioManager.Init()) return false;

		m_ActiveGame = new Game();
		m_ActiveGame->Init();

		m_AudioManager.LoopSound("PrototypeMusicProjectFile.wav", 0.05f);

		return true;
	}

	void Engine::Run() {
		while (m_Running && !m_Window->ShouldClose()) {

			m_Window->OnUpdate();

			if (Input::instance) {
				Input::instance->UpdateStatesImpl();
			}

			ProcessInput();
			Update();
			Render();
		}
	}

	void Engine::ProcessInput() {
		if (Input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
			m_Running = false;
		}
	}

	void Engine::Update() {
		m_AudioManager.Update();
		if (m_ActiveGame) m_ActiveGame->Update();
	}

	void Engine::Render() {
		glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_ActiveGame) m_ActiveGame->Render();
	}

	void Engine::Cleanup() {
		m_AudioManager.Cleanup();

		if (m_ActiveGame) {
			delete m_ActiveGame;
			m_ActiveGame = nullptr;
		}
	}
}