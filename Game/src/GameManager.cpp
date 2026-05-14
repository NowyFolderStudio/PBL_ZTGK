#include "GameManager.hpp"

#include "Layers/MainMenuLayer.hpp"
#include "Layers/GameLayer.hpp"
#include "Layers/PauseLayer.hpp"
#include "Layers/UILayer.hpp"
#include "Layers/LayerExample.hpp"

GameManager& GameManager::Get() {
    static GameManager instance;
    return instance;
}

void GameManager::Init() { ChangeState(GameState::MainMenu); }

void GameManager::ChangeState(GameState newState) {
    auto& app = NFSEngine::Application::Get();

    if (m_CurrentState == newState && m_CurrentLayer != nullptr) return;

    for (auto* garbage : m_GarbageLayers) {
        delete garbage;
    }
    m_GarbageLayers.clear();

    if (m_CurrentLayer) {
        app.PopLayer(m_CurrentLayer);
        m_GarbageLayers.push_back(m_CurrentLayer);
        m_CurrentLayer = nullptr;
    }

    if (m_PauseOverlay) {
        app.PopOverlay(m_PauseOverlay);
        m_GarbageLayers.push_back(m_PauseOverlay);
        m_PauseOverlay = nullptr;
    }

    m_CurrentState = newState;

    switch (m_CurrentState) {
    case GameState::MainMenu:
        m_CurrentLayer = new MainMenuLayer();
        app.PushLayer(m_CurrentLayer);
        break;
    case GameState::Playing: {

        // m_CurrentLayer = new GameLayer();
        // app.PushLayer(m_CurrentLayer);

        auto* uiLayer = new UILayer();
        app.PushOverlay(uiLayer);
        m_CurrentLayer = new LayerExample(uiLayer);
        app.PushLayer(m_CurrentLayer);
        break;
    }
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
        m_GarbageLayers.push_back(m_PauseOverlay);
        m_PauseOverlay = nullptr;
    }
}

void GameManager::RequestStateChange(GameState newState) {
    if (m_CurrentState == newState) return;
    m_PendingState = newState;
    m_HasPendingState = true;
}

void GameManager::ProcessStateChange() {
    if (!m_HasPendingState) return;

    ChangeState(m_PendingState);

    m_HasPendingState = false;
}

GameState GameManager::GetCurrentState() const { return m_CurrentState; }