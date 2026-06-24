#pragma once

#include <NFSEngine.h>
#include "GameManager.hpp"

class ScoreManager : public NFSEngine::Component {
public:
    inline static ScoreManager* Instance = nullptr;

    explicit ScoreManager(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    ~ScoreManager() override {
        if (Instance == this) {
            Instance = nullptr;
        }
    }

    std::string GetName() const override { return "ScoreManager"; }

    NFSEngine::Action<int> OnScoreChanged;

    void AddScore(int points) {
        GameManager::Get().PlayerScore += points;
        OnScoreChanged.Invoke(GameManager::Get().PlayerScore);
    }

    int GetScore() const { return GameManager::Get().PlayerScore; }

    void ResetScore() {
        GameManager::Get().PlayerScore = 0;
        OnScoreChanged.Invoke(0);
    }

protected:
    void OnAwake() override {
        if (Instance != nullptr && Instance != this) return;
        Instance = this;
    }
};
