#pragma once

#include <functional>

class ScoreManager {
public:
    ScoreManager() = default;

    void AddScore(int points) {
        m_Score += points;
        if (m_OnScoreChanged)
            m_OnScoreChanged(m_Score);
    }

    int GetScore() const { return m_Score; }
    void ResetScore() { m_Score = 0; if (m_OnScoreChanged) m_OnScoreChanged(m_Score); }

    std::function<void(int)> m_OnScoreChanged;

private:
    int m_Score = 0;
};
