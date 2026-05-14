#pragma once

#include <NFSEngine.h>

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver
};

class GameManager {
private:
    GameState m_CurrentState;
    NFSEngine::Layer* m_MainMenuLayer = nullptr;
    NFSEngine::Layer* m_CurrentLayer = nullptr;
    NFSEngine::Layer* m_PauseOverlay = nullptr;

    std::vector<NFSEngine::Layer*> m_GarbageLayers;
    bool m_HasPendingState = false;
    GameState m_PendingState;

    GameManager() = default;

public:
    static GameManager& Get();
    void Init();
    void ChangeState(GameState newState);
    void TogglePause();

    void RequestStateChange(GameState newState);
    void ProcessStateChange();

    GameState GetCurrentState() const;
};