#pragma once

#include <NFSEngine.h>

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
        m_Score += points;
        OnScoreChanged.Invoke(m_Score);
    }

    int GetScore() const { return m_Score; }
    void ResetScore() {
        m_Score = 0;
        OnScoreChanged.Invoke(m_Score);
    }

protected:
    void OnAwake() override {
        if (Instance != nullptr && Instance != this) return;
        Instance = this;
    }

private:
    int m_Score = 0;
};
