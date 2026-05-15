#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Components/LivesManagerComponent.hpp"

namespace NFSEngine {

    class HazardComponent : public Component {
    public:
        explicit HazardComponent(GameObject* owner)
            : Component(owner) {}

        std::string GetName() const override { return "HazardComponent"; }

        float HitCooldown = 1.5f;

    protected:
        void OnStart() override {
            auto* collider = m_Owner->GetComponent<ColliderComponent>();
            if (!collider) return;
            collider->IsTrigger = true;

            auto* scene = m_Owner->GetScene();
            auto* gm = scene ? scene->FindWithTag(Tags::GameManager) : nullptr;
            auto* livesComp = gm ? gm->GetComponent<LivesManagerComponent>() : nullptr;

            auto hitAction = [this, livesComp](GameObject* other) {
                if (!other->CompareTag(Tags::Player)) return;
                if (m_CooldownTimer > 0.0f) return;
                m_CooldownTimer = HitCooldown;
                if (livesComp) livesComp->LoseHeart();
            };

            collider->OnTriggerEnter = hitAction;
            collider->OnTriggerStay = hitAction;
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
