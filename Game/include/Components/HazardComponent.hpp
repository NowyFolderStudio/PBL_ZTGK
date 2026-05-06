#pragma once

#include "Components/Component.hpp"
#include "Core/GameObject.hpp"
#include "Core/Physics/PhysicsSystem.hpp"

#include <functional>

namespace NFSEngine {

    class HazardComponent : public Component {
    public:
        explicit HazardComponent(GameObject* owner)
            : Component(owner) { }

        std::string GetName() const override { return "HazardComponent"; }

        void SetTarget(GameObject* target) { m_Target = target; }
        std::function<void()> OnPlayerHit;

        float HitCooldown = 1.5f;

    protected:
        void OnUpdate(DeltaTime deltaTime) override {
            if (!m_Target) return;

            if (m_CooldownTimer > 0.0f) {
                m_CooldownTimer -= deltaTime.GetSeconds();
                return;
            }

            if (PhysicsSystem::CheckCollision(m_Owner, m_Target).IsColliding) {
                m_CooldownTimer = HitCooldown;
                if (OnPlayerHit) OnPlayerHit();
            }
        }

    private:
        GameObject* m_Target = nullptr;
        float m_CooldownTimer = 0.0f;
    };

} // namespace NFSEngine
