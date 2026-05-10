#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"

#include <functional>

namespace NFSEngine {

    class HazardComponent : public Component {
    public:
        explicit HazardComponent(GameObject* owner)
            : Component(owner) {}

        std::string GetName() const override { return "HazardComponent"; }

        std::function<void()> OnPlayerHit;
        float HitCooldown = 1.5f;

    protected:
        void OnStart() override {
            auto* collider = m_Owner->GetComponent<ColliderComponent>();
            if (collider) {
                collider->IsTrigger = true;
                collider->OnTriggerEnter = [this](GameObject* other) {
                    if (other->name != "Player") return;
                    if (m_CooldownTimer > 0.0f) return;

                    m_CooldownTimer = HitCooldown;

                    if (OnPlayerHit)
                        OnPlayerHit();
                };
                collider->OnTriggerStay = [this](GameObject* other) {
                    if (other->name != "Player") return;
                    if (m_CooldownTimer > 0.0f) return;

                    m_CooldownTimer = HitCooldown;
                    if (OnPlayerHit) OnPlayerHit();
                };
            }
        }

        void OnFixedUpdate(DeltaTime fixedDeltaTime) override {
            if (m_CooldownTimer > 0.0f) {
                m_CooldownTimer -= (float)fixedDeltaTime;
            }
        }

    private:
        float m_CooldownTimer = 0.0f;
    };

} // namespace NFSEngine
