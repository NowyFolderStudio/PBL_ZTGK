#include "GameManager.hpp"

#include "Layers/MainMenuLayer.hpp"
#include "Layers/GameLayer.hpp"
#include "Layers/PauseLayer.hpp"

GameManager& GameManager::Get() {
    static GameManager instance;
    return instance;
}

void GameManager::Init() { m_CurrentState = GameState::MainMenu; }

void GameManager::ChangeState(GameState newState) {
    auto& app = NFSEngine::Application::Get();

    if (m_CurrentState == newState && m_CurrentLayer != nullptr) return;

    if (m_CurrentLayer) {
        app.PopLayer(m_CurrentLayer);
        delete m_CurrentLayer;
        m_CurrentLayer = nullptr;
    }

    if (m_PauseOverlay) {
        app.PopOverlay(m_PauseOverlay);
        delete m_PauseOverlay;
        m_PauseOverlay = nullptr;
    }

    m_CurrentState = newState;

    switch (m_CurrentState) {
    case GameState::MainMenu:
        m_CurrentLayer = new MainMenuLayer();
        app.PushLayer(m_CurrentLayer);
        break;
    case GameState::Playing:
        m_CurrentLayer = new GameLayer();
        app.PushLayer(m_CurrentLayer);
        break;
    case GameState::Paused:
        break;
    }
}

void GameManager::TogglePause() {
    if (m_CurrentState == GameState::MainMenu) return;

    auto& app = NFSEngine::Application::Get();

    if (m_CurrentState == GameState::Playing) {
        m_CurrentState = GameState::Paused;
        m_PauseOverlay = new PauseLayer();
        app.PushOverlay(m_PauseOverlay);

    } else if (m_CurrentState == GameState::Paused) {
        m_CurrentState = GameState::Playing;
        app.PopOverlay(m_PauseOverlay);
        delete m_PauseOverlay;
        m_PauseOverlay = nullptr;
    }
}

GameState GameManager::GetCurrentState() const { return m_CurrentState; }