#include "GameManager.hpp"

#include "Layers/MainMenuLayer.hpp"
#include "Layers/GameLayer.hpp"
#include "Layers/PauseLayer.hpp"
#include "Layers/UILayer.hpp"
#include "Layers/OptionsLayer.hpp"
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

    if (m_GameUILayer) {
        app.PopOverlay(m_GameUILayer);
        m_GarbageLayers.push_back(m_GameUILayer);
        m_GameUILayer = nullptr;
    }

    if (m_PauseOverlay) {
        app.PopOverlay(m_PauseOverlay);
        m_GarbageLayers.push_back(m_PauseOverlay);
        m_PauseOverlay = nullptr;
    }

    if (m_OptionsOverlay) {
        app.PopOverlay(m_OptionsOverlay);
        m_GarbageLayers.push_back(m_OptionsOverlay);
        m_OptionsOverlay = nullptr;
    }

    m_CurrentState = newState;

    switch (m_CurrentState) {
    case GameState::MainMenu:
        app.GetWindow().SetCursorMode(NFSEngine::CursorMode::Normal);

        m_CurrentLayer = new MainMenuLayer();
        app.PushLayer(m_CurrentLayer);
        break;
    case GameState::Playing: {
        app.GetWindow().SetCursorMode(NFSEngine::CursorMode::Locked);
        // m_CurrentLayer = new GameLayer();
        // app.PushLayer(m_CurrentLayer);

        m_GameUILayer = new UILayer();
        app.PushOverlay(m_GameUILayer);

        // Here we should change layerexample constructor
        m_CurrentLayer = new LayerExample(static_cast<UILayer*>(m_GameUILayer));
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

        app.GetWindow().SetCursorMode(NFSEngine::CursorMode::Normal);

        m_PauseOverlay = new PauseLayer();
        app.PushOverlay(m_PauseOverlay);

    } else if (m_CurrentState == GameState::Paused) {
        m_CurrentState = GameState::Playing;

        app.GetWindow().SetCursorMode(NFSEngine::CursorMode::Locked);

        app.PopOverlay(m_PauseOverlay);
        m_GarbageLayers.push_back(m_PauseOverlay);
        m_PauseOverlay = nullptr;
    }
}

void GameManager::OpenOptions() {
    if (m_OptionsOverlay) return;

    auto& app = NFSEngine::Application::Get();

    m_OptionsOverlay = new OptionsLayer();

    app.PushOverlay(m_OptionsOverlay);
}

void GameManager::CloseOptions() {
    if (!m_OptionsOverlay) return;

    auto& app = NFSEngine::Application::Get();

    app.PopOverlay(m_OptionsOverlay);

    m_GarbageLayers.push_back(m_OptionsOverlay);
    m_OptionsOverlay = nullptr;
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