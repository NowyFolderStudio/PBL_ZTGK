#pragma once

#include "Components/Component.hpp"
#include <functional>

namespace NFSEngine {

    class ScoreManagerComponent : public Component {
    public:
        explicit ScoreManagerComponent(GameObject* owner)
            : Component(owner) {}

        std::string GetName() const override { return "ScoreManagerComponent"; }

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

} // namespace NFSEngine
