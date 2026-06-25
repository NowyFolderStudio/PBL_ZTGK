#include "GameManager.hpp"

#include "Layers/MainMenuLayer.hpp"
#include "Layers/GameLayer.hpp"
#include "Layers/PauseLayer.hpp"
#include "Layers/OptionsLayer.hpp"
#include "Layers/LayerExample.hpp"
#include "Layers/LayerZmoreq.hpp"
#include "Layers/LayerGuga.hpp"
#include "Layers/LayerExample2.hpp"
#include "Layers/LayerExample3.hpp"
#include "Layers/LayerExample4.hpp"
#include "Layers/IntroLayer.hpp"
#include "Layers/OutroLayer.hpp"
#include "Core/AudioManager.hpp"

GameManager& GameManager::Get() {
    static GameManager instance;
    return instance;
}

void GameManager::Init() { ChangeState(GameState::MainMenu); }

void GameManager::LoadLevel(const std::string& levelPath) {
    m_CurrentLevelPath = levelPath;

    m_PendingState = GameState::Playing;
    m_HasPendingState = true;
}

void GameManager::ChangeState(GameState newState) {
    auto& app = NFSEngine::Application::Get();

    // if (m_CurrentState == newState && m_CurrentLayer != nullptr) return;

    for (auto* garbage : m_GarbageLayers) {
        delete garbage;
    }
    m_GarbageLayers.clear();

    NFSEngine::DialogueManager::Get().HideMessage();

    NFSEngine::AudioManager::ClearAllPatterns();

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

    if (m_OptionsOverlay) {
        app.PopOverlay(m_OptionsOverlay);
        m_GarbageLayers.push_back(m_OptionsOverlay);
        m_OptionsOverlay = nullptr;
    }

    m_CurrentState = newState;

    switch (m_CurrentState) {
    case GameState::MainMenu:
        app.GetWindow().SetCursorMode(NFSEngine::CursorMode::Normal);
        ResetLevelPath();
        m_CurrentLayer = new MainMenuLayer();
        app.PushLayer(m_CurrentLayer);
        break;
    case GameState::Intro:
        m_CurrentLayer = new IntroLayer();
        app.PushLayer(m_CurrentLayer);
        break;

    case GameState::Playing: {
        app.GetWindow().SetCursorMode(NFSEngine::CursorMode::Locked);

        if (m_CurrentLevelPath == "Example") {
            m_CurrentLayer = new LayerExample();
        } else if (m_CurrentLevelPath == "Example2") {
            m_CurrentLayer = new LayerExample2();
        } else if (m_CurrentLevelPath == "Example3") {
            m_CurrentLayer = new LayerExample3();
        } else if (m_CurrentLevelPath == "Example4") {
            m_CurrentLayer = new LayerExample4();
        } else {
            m_CurrentLayer = new LayerExample2();
        }

        app.PushLayer(m_CurrentLayer);
        break;
    }
    case GameState::Outro:
        m_CurrentLayer = new OutroLayer();
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

void GameManager::ResetLevelPath() {
    m_CurrentLevelPath = "assets/scenes/Poziomix_v2_export.json";
}

GameState GameManager::GetCurrentState() const { return m_CurrentState; }