#pragma once

#include <NFSEngine.h>

class LivesManager : public NFSEngine::Component {
public:
    inline static LivesManager* Instance = nullptr;

    explicit LivesManager(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    ~LivesManager() override {
        if (Instance == this) {
            Instance = nullptr;
        }
    }

    static constexpr int k_MaxLives = 3;

    std::string GetName() const override { return "LivesManager"; }

    NFSEngine::Action<int> OnLivesChanged;

    int LoseHeart() {
        if (m_Lives <= 0) return 0;
        m_Lives--;
        OnLivesChanged.Invoke(m_Lives);
        return m_Lives;
    }

    int GetLives() const { return m_Lives; }
    void ResetLives() {
        m_Lives = k_MaxLives;
        OnLivesChanged.Invoke(m_Lives);
    }

protected:
    void OnAwake() override {
        if (Instance != nullptr && Instance != this) return;
        Instance = this;
    }

private:
    int m_Lives = k_MaxLives;
};
