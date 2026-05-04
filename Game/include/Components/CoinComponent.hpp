#pragma once

#include "Components/Component.hpp"
#include "Core/GameObject.hpp"
#include "Core/PhysicsSystem.hpp"

#include <functional>

namespace NFSEngine {

    class CoinComponent : public Component {
    public:
        explicit CoinComponent(GameObject* owner)
            : Component(owner) {}

        std::string GetName() const override { return "CoinComponent"; }

        void SetTarget(GameObject* target) { m_Target = target; }
        std::function<void()> OnCollected;

    protected:
        void OnUpdate(DeltaTime) override {
            if (!m_Target || m_Collected) return;

            if (PhysicsSystem::CheckCollision(m_Owner, m_Target)) {
                m_Collected = true;

                if (OnCollected) OnCollected();

                m_Owner->SetActive(false);
            }
        }

    private:
        GameObject* m_Target = nullptr;
        bool m_Collected = false;
    };

} // namespace NFSEngine
