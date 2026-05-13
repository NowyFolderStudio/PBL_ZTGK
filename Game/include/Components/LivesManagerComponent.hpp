#pragma once

#include "Components/Component.hpp"
#include <functional>

namespace NFSEngine {

    class LivesManagerComponent : public Component {
    public:
        explicit LivesManagerComponent(GameObject* owner)
            : Component(owner) {}

        static constexpr int k_MaxLives = 3;

        std::string GetName() const override { return "LivesManagerComponent"; }

        int LoseHeart() {
            if (m_Lives <= 0) return 0;
            m_Lives--;
            if (m_OnLivesChanged)
                m_OnLivesChanged(m_Lives);
            return m_Lives;
        }

        int GetLives() const { return m_Lives; }
        void ResetLives() { m_Lives = k_MaxLives; if (m_OnLivesChanged) m_OnLivesChanged(m_Lives); }

        std::function<void(int)> m_OnLivesChanged;

    private:
        int m_Lives = k_MaxLives;
    };

} // namespace NFSEngine
