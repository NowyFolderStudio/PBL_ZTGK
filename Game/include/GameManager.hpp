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

    GameManager() = default;

public:
    static GameManager& Get();
    void Init();
    void ChangeState(GameState newState);
    void TogglePause();
    GameState GetCurrentState() const;
};